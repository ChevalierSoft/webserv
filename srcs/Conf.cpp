
#include "Conf.hpp"
#include <fstream>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <algorithm>

Conf::Conf(){}

Conf::Conf(file_type conf_file){
	std::ifstream ifs(conf_file.c_str(), std::ifstream::in);

	if (ifs.is_open())
		parse_file(ifs);
	else
		std::cerr << "File not found" << std::endl;
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
		indent = line.find(indent_sep);
		line = remove_whitespaces(line);
		if (indent == std::string::npos)
			indent = -1;
		if ((params[indent + 1] = parse_param(line, sep)) == "error")
			return (false);
		else if (!(set_param(line, params, indent)))
			return (false);

		// else if (indent == 0)
		// {
		// 	if (!(set_param_indent1(line, param)))
		// 		return (false);
		// }
	}
	return (true);
}

std::string Conf::parse_param(std::string &line, const char sep) {
	size_t      found;

	if ((found = line.find(sep)) != std::string::npos)
		return (remove_whitespaces(line.substr(0, found)));
	return ("error");
}

std::string Conf::parse_value(std::string &line, std::string param) {
	return (std::string(line.substr(param.size() + 1)));
}

bool    Conf::set_param(std::string &line, std::vector<std::string> params, size_t indent) {
	std::string value(parse_value(line, params[indent]));

	if (params[0] == "server_name")
		return (set_name(value));
	else if (params[0] == "host")
		return (set_host(value));
	else if (params[0] == "port")
		return (set_port(string_to_port(value)));
	else if (params[0] == "error_pages")
		return (true);
	else if (params[0] == "client_body_size")
		return (set_client_body_size(string_to_client_body_size(value)));
	else if (params[0] == "methods")
		return (set_methods(string_to_methods(value)));
	else if (params[0] == "directory_listing")
		return (set_dir_listing(string_to_dir_listing(value)));
	else if (params[0] == "uploads")
		return (set_upload_path(value));
	else
		return (false);
}

// bool	Conf::set_param_indent1(std::string &line, std::string param)
// {
// 	if (param == "error_pages")
// 		return (add_error(string_to_error(line)));
// 	else if (param == "routes")
// 		return (true);
// 	else
// 		return (false);
// }

std::string	Conf::remove_comments(std::string &line, const char sep) {
	size_t      found;

	if ((found = line.find(sep)) != std::string::npos)
		return (line.substr(0, found));
	else
		return (line);
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

Conf::port_type   Conf::string_to_port(std::string value) {
	return (atoi(value.c_str()));
}

Conf::code_type     Conf::string_to_code(std::string value) {
	return (atoi(value.c_str()));
}

Conf::error_type    Conf::string_to_error(std::string value) {
	const char  sep = ':';
	std::string param(parse_param(value, sep));

	return (std::make_pair(string_to_code(param), parse_value(value, param)));
}

bool	isnotdigit(int c) {
	return (!(std::isdigit(c)));
}

Conf::size_type		Conf::string_to_client_body_size(std::string value) {
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

Conf::method_list	Conf::string_to_methods(std::string value) {
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

Conf::dir_listing_type	Conf::string_to_dir_listing(std::string value) {
	if (value == "off")
		return (0);
	else if (value == "on")
		return (1);
	else
		return (-1);
}

bool    Conf::set_name(name_type name) {
	_name = name;
	return (true);
}

bool    Conf::set_host(host_type host) {
	_host = host;
	return (true);
}

bool        Conf::set_port(port_type port) {
	_port = port;
	return (true);
}
bool    Conf::add_error(error_type  error) {
	_error_pages.insert(error);
	return (true);
}

bool	Conf::set_client_body_size(size_type client_body_size) {
	if (client_body_size < 0)
		return (false);
	_client_body_size = client_body_size;
	return (true);
}

bool	Conf::add_method(method_type method) {
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
		return (false);
	_dir_listing = dir_listing;
	return (true);
}

bool	Conf::set_upload_path(path_type upload_path) {
	_upload_path = upload_path;
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

}