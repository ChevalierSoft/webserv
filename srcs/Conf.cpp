
#include "Conf.hpp"
#include <fstream>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

Conf::Conf(){}

Conf::Conf(file_type conf_file): _errno(0) {
	std::ifstream ifs(conf_file.c_str(), std::ifstream::in);

	_errno |= !(ifs.is_open());
	if (!_errno)
		_errno |= !parse_file(ifs);
	else
		_error_message = "File not found";
}

Conf::~Conf() {}

bool    Conf::parse_file(std::ifstream &ifs) {
	const char  sep = ':';
	const char	comment_sep = '#';
	const char	indent_sep = '\t';
	std::string line;
	std::vector<std::string> params;
	size_t		indent;
	
	
	while (std::getline(ifs, line).good() && line != "")
	{
		line = remove_comments(line, comment_sep);
		indent = 0;
		while (line[indent] == indent_sep)
			indent++;;
		line = remove_whitespaces(line);
		if (*(params.insert(params.begin() + indent, parse_param(line, sep))) == "error")
			return (false);
		else if (!(set_param(line, params, indent)))
			return (false);
	}
	return (true);
}

std::string Conf::parse_param(std::string &line, const char sep) {
	size_t      found;

	if ((found = line.find(sep)) != std::string::npos)
		return (remove_whitespaces(line.substr(0, found)));
	_error_message = "Missing separator";
	return ("error");
}

std::string Conf::parse_value(std::string &line, std::string param) {
	return (std::string(line.substr(param.size() + 1)));
}

bool    Conf::set_param(std::string &line, std::vector<std::string> params, size_t indent) {
	std::string value(parse_value(line, params[indent]));

	if (indent ==  0)
		return (zero_indent(params[0], value));
	else if (indent == 1)
		return (one_indent(params, value));
	else if (indent == 2)
		return (two_indent(params, value));
	else if (indent == 3)
		return (three_indent(params, value));
	_error_message = "Bad indentation";
	return (false);
}

std::string	Conf::remove_comments(std::string &line, const char sep) {
	size_t      found;

	if ((found = line.find(sep)) != std::string::npos)
		return (line.substr(0, found));
	else
		return (line);
}

bool	Conf::zero_indent(std::string param, std::string value)
{
	if (param == "server_name")
		return (set_name(value));
	else if (param == "host")
		return (set_host(value));
	else if (param == "port")
		return (set_port(string_to_port(value)));
	else if (param == "client_body_size")
		return (set_client_body_size(string_to_client_body_size(value)));
	else if (param == "methods")
		return (set_methods(string_to_methods(value)));
	else if (param == "directory_listing")
		return (set_dir_listing(string_to_dir_listing(value)));
	else if (param == "uploads")
		return (set_upload_path(value));
	else if (param == "error_pages")
		return (true);
	else if (param == "routes")
		return (true);
	_error_message = "Invalid parameter";
	return (false);
}

bool	Conf::one_indent(std::vector<std::string> params, std::string value)
{
	if (params[0] == "error_pages")
		return (add_error(std::make_pair(string_to_code(params[1]), value)));
	else if (params[0] == "routes")
		return (add_route(string_to_route(params[1])));
	_error_message = "Invalid parameter";
	return (false);
}

bool	Conf::two_indent(std::vector<std::string> params, std::string value)
{
	if (params[0] == "routes")
	{
		Route	&target = _routes.back();
		if (params[2] == "methods")
			return (target.set_methods(string_to_methods(value)));
		else if (params[2] == "root")
			return (target.set_root(value));
		else if (params[2] == "directory_listing")
			return (target.set_dir_listing(string_to_dir_listing(value)));
		else if (params[2] == "file")
			return (target.set_file(value));
		else if (params[2] == "uploads")
			return (target.set_upload_path(value));
		else if (params[2] == "redirections")
			return (true);
		else if (params[2] == "cgi")
			return (true);
	}
	_error_message = "Invalid parameter";
	return (false);
}

bool	Conf::three_indent(std::vector<std::string> params, std::string value) {
	if (params[0] == "routes")
	{
		Route	&target = _routes.back();
		if (params[2] == "redirections")
			return (target.set_redir(std::make_pair(string_to_code(params[3]), value)));
		else if (params[2] == "cgi")
			return  (target.add_cgi(std::make_pair(params[3], value)));
	}
	_error_message = "Invalid parameter";
	return (false);
}

bool is_whitespace (const char c) { 
	return (c == ' ' || c == '\t'); 
}

std::string	Conf::remove_whitespaces(std::string s) {
	size_t      found;

	std::string::iterator end_pos = std::remove_if(s.begin(), s.end(), is_whitespace);
	s.erase(end_pos, s.end());
	return (s);
}

port_type   Conf::string_to_port(std::string value) {
	if (value == "")
		return (-1);
	return (atoi(value.c_str()));
}

code_type     Conf::string_to_code(std::string value) {
	if (value == "")
		return (-1);
	return (atoi(value.c_str()));
}

route_type    Conf::string_to_route(std::string value) {
	return (Route(value, _methods, _dir_listing, _upload_path));
}

bool	isnotdigit(int c) {
	return (!(std::isdigit(c)));
}

size_type		Conf::string_to_client_body_size(std::string value) {
	std::string	scale = "MB";

	std::string::iterator	end_pos = std::find_if(value.begin(), value.end(), isnotdigit);
	if (end_pos != value.end())
		scale = &(*end_pos);
	if (scale == "kB")
		return (atoi(value.c_str()) * 1000);
	else if (scale == "MB")
		return (atoi(value.c_str()) * 1000000);
	else if (scale == "GB")
		return (atoi(value.c_str()) * 1000000000);
	return (-1);
}

method_list		Conf::string_to_methods(std::string value) {
	const char	sep = ',';
	size_t		start = 0;
	size_t		end;
	method_list	methods;

	while (start < value.size())
	{
		if ((end = value.find(start, sep)) == std::string::npos)
			end = value.size();
		methods.push_back(value.substr(start, end));
		start = end;
	}
	return (methods);
}

dir_listing_type	Conf::string_to_dir_listing(std::string value) {
	if (value == "off")
		return (0);
	else if (value == "on")
		return (1);
	return (-1);
}

bool    Conf::set_name(name_type name) {
	if (name == "")
		return (set_error_message("Invalid value: name"));
	_name = name;
	return (false);
}

bool    Conf::set_host(host_type host) {
	if (host == "")
		return (set_error_message("Invalid value: host"));
	_host = host;
	return (true);
}

bool        Conf::set_port(port_type port) {
	if (port < 0)
		return (set_error_message("Invalid value: port"));
	_port = port;
	return (true);
}

bool    Conf::add_error(error_type  error) {
	if (error.first < 0)
		return (set_error_message("Invalid parameter"));
	else if (error.second == "")
		return (set_error_message("Invaild value: error_pages"));
	_error_pages.insert(error);
	return (true);
}

bool	Conf::set_client_body_size(size_type client_body_size) {
	if (client_body_size < 0)
		return (set_error_message("Invalid value: client_body_size"));
	_client_body_size = client_body_size;
	return (true);
}

bool	Conf::add_method(method_type method) {
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (set_error_message("Invalid value: methods"));
	_methods.push_back(method);
	return (true);
}

bool	Conf::set_methods(method_list methods) {
	for (method_list::iterator it = methods.begin(); it != methods.end(); it++)
		if (!add_method(*it))
			return (false);
	return (true);
}

bool	Conf::set_dir_listing(dir_listing_type dir_listing) {
	if (dir_listing < 0)
		return (set_error_message("Invalid value: directory_listing"));
	_dir_listing = dir_listing;
	return (true);
}

bool	Conf::set_upload_path(path_type upload_path) {
	if (upload_path == "")
		return(set_error_message("Invalid value: uploads"));
	_upload_path = upload_path;
	return (true);
}

bool	Conf::add_route(route_type route) {
	if (route._errno)
		return (set_error_message(route._error_message));
	_routes.push_back(route);
	return (true);
}

void	Conf::print() {
	std::cout << "name = " << _name << std::endl;
	std::cout << "host = " << _host << std::endl;
	std::cout << "port = " << _port << std::endl;
	std::cout << "error_pages:" << std::endl;
	for (error_list::iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
		std::cout << "\t" << (*it).first << ": " << (*it).second << std::endl;
	std::cout << "client_body_size = " << _client_body_size << std::endl;
	std::cout << "methods = ";
	for (method_list::iterator it = _methods.begin(); it != _methods.end(); it++)
	{
		if  (it != _methods.begin())
			std::cout << ", ";
		std::cout << *it;
	}
	std::cout << std::endl;
	std::cout << "directory listing = " << _dir_listing << std::endl;
	std::cout << "uploads = " << _upload_path << std::endl;
	std::cout << "routes:" << std::endl;
	for (route_list::iterator it = _routes.begin(); it != _routes.end(); it++)
		(*it).print();
}

bool		Conf::set_error_message(std::string error_message) {
	_error_message = error_message;
	return (false);
}