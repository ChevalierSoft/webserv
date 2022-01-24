
#include "Conf.hpp"
#include <fstream>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

Conf::Conf(): _err(0) {}

Conf::~Conf() {}

Conf	&Conf::operator=(Conf const &rhs) {
	_name = rhs._name;
	_host = rhs._host;
	_port = rhs._port;
	_error_pages = rhs._error_pages;
	_client_body_size = rhs._client_body_size;
	_methods = rhs._methods;
	_dir_listing = rhs._dir_listing;
	_upload_path = rhs._upload_path;
	_routes = rhs._routes;
	_error_message = rhs._error_message;
	_err = rhs._err;

	return (*this);
}

Conf::Conf(Conf const &copy) {
	*this = copy;
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
		if ((end = value.find(sep, start)) == std::string::npos)
			end = value.size();
		methods.push_back(value.substr(start, end - start));
		start = ++end;
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
	return (true);
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
	if (route._err)
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