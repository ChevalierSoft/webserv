/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljurdant <ljurdant@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:56:26 by ljurdant          #+#    #+#             */
/*   Updated: 2022/02/11 13:56:33 by ljurdant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Conf::Conf(): _name(name_type()),
_hosts(host_list()),
_client_body_size(size_t()),
_methods(method_list()),
_dir_listing(-1),
_upload_path(path_type()),
_routes(route_list()),
_cgis(cgi_list()),
_error_message(std::string()),
_err(0) {
	// std::string	error_path = "./resources/error_pages/html/";
	// _error_pages[400] = error_path + "400.html";
	// _error_pages[401] = error_path + "401.html";
	// _error_pages[402] = error_path + "402.html";
	// _error_pages[404] = error_path + "404.html";
	// _error_pages[406] = error_path + "406.html";
	// _error_pages[407] = error_path + "407.html";
	// _error_pages[408] = error_path + "408.html";
	// _error_pages[409] = error_path + "409.html";
	// _error_pages[410] = error_path + "410.html";
	// _error_pages[411] = error_path + "411.html";
	// _error_pages[417] = error_path + "417.html";
	// _error_pages[429] = error_path + "429.html";
	// _error_pages[451] = error_path + "451.html";
}

Conf::~Conf() {}

Conf	&Conf::operator=(Conf const &rhs) {
	_name = rhs._name;
	_hosts = rhs._hosts;
	_error_pages = rhs._error_pages;
	_client_body_size = rhs._client_body_size;
	_methods = rhs._methods;
	_dir_listing = rhs._dir_listing;
	_upload_path = rhs._upload_path;
	_routes = rhs._routes;
	_cgis = rhs._cgis;
	_error_message = rhs._error_message;
	_err = rhs._err;

	return (*this);
}

Conf::Conf(Conf const &copy) {
	*this = copy;
}

bool	isnotdigit(int c) {
	return (!(std::isdigit(c)));
}

int	my_strtoi(std::string s)
{
	int	n = 0;
	for (std::string::size_type i = 0; i < s.size(); i++)
	{
		if (!std::isdigit(s[i]) || n * 10 < n)
			return (-1);
		n = n * 10 + s[i] - 48;
	}
	return (n);
}

Conf::port_type	Conf::string_to_port(std::string value) {
	if (value == "" || count_if(value.begin(), value.end(), isnotdigit))
		return (-1);
	return (my_strtoi(value));
}

Conf::host_pair	Conf::string_to_host_pair(std::string value) {
	size_t	found;
	const char	sep = ':';
	host_pair	host;

	found = value.find(sep);
	host.first = value.substr(0, found);
	found++;
	if (found != std::string::npos)
		host.second = string_to_port(value.substr(found, value.size() - found));
	else
		host.second = -1;
	return (host);
}

Conf::host_list	Conf::string_to_hosts(std::string value) {
	const char	sep = ',';
	size_t		start = 0;
	size_t		end;
	host_list	hosts;

	while (start <= value.size())
	{
		if ((end = value.find(sep, start)) == std::string::npos)
			end = value.size();
		hosts.push_back(string_to_host_pair(value.substr(start, end - start)));
		start = ++end;
	}
	return (hosts);
}

Conf::code_type     Conf::string_to_code(std::string value) {
	if (value == "" || count_if(value.begin(), value.end(), isnotdigit))
		return (-1);
	return (my_strtoi(value));
}

Conf::route_type    Conf::string_to_route(std::string value) {
	if (*(value.end() -1) != '/')
		value+="/";
	return (Route(value, _methods, _dir_listing, _upload_path, _cgis));
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

Conf::method_list		Conf::string_to_methods(std::string value) {
	const char	sep = ',';
	size_t		start = 0;
	size_t		end;
	method_list	methods;

	while (start <= value.size())
	{
		if ((end = value.find(sep, start)) == std::string::npos)
			end = value.size();
		methods.push_back(value.substr(start, end - start));
		start = ++end;
	}
	return (methods);
}

Conf::dir_listing_type	Conf::string_to_dir_listing(std::string value) {
	if (value == "off")
		return (0);
	else if (value == "on")
		return (1);
	return (-1);
}

bool    Conf::set_name(name_type name) {
	if (name == "")
		return (set_error_message("Invalid value: name"));
	else if (_name != name_type())
		return (set_error_message("Double parameter: name"));
	_name = name;
	return (true);
}

bool	Conf::add_host(host_pair host) {
	for (host_list::iterator it = _hosts.begin(); it != _hosts.end(); it++)
		if (*it == host)
			return (set_error_message("Double parameter: host"));
	if (host.first == "" || host.second == -1)
		return (set_error_message("Invalid value: host"));
	_hosts.push_back(host);
	return (true);
}

bool	Conf::set_hosts(host_list hosts) {
	if (_hosts != host_list())
		return (set_error_message("Double parameter: hosts"));
	for (host_list::iterator it = hosts.begin(); it != hosts.end(); it++)
		if (!add_host(*it))
			return (false);
	return (true);
}

bool    Conf::add_error(error_type  error) {
	if (error.first < 0 || error.second == "")
		return (set_error_message("Invalid value: error_pages"));
	_error_pages[error.first]=error.second;
	return (true);
}

bool	Conf::set_client_body_size(size_type client_body_size) {
	if (client_body_size < 0)
		return (set_error_message("Invalid value: client_body_size"));
	else if (_client_body_size != size_type())
		return (set_error_message("Double parameter: client_body_size"));
	_client_body_size = client_body_size;
	return (true);
}

bool	Conf::add_method(method_type method) {
	for (method_list::iterator it = _methods.begin(); it != _methods.end(); it++)
		if (*it == method)
			return (set_error_message("Double parameter: method"));
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (set_error_message("Invalid value: methods"));
	_methods.push_back(method);
	return (true);
}

bool	Conf::set_methods(method_list methods) {
	if (_methods != method_list())
		return (set_error_message("Double parameter: methods"));
	for (method_list::iterator it = methods.begin(); it != methods.end(); it++)
		if (!add_method(*it))
			return (false);
	return (true);
}

bool    Conf::add_cgi(cgi_type  cgi) {
	if (cgi.first == "" || cgi.second == "")
		return (set_error_message("Invalid value: cgis"));
	_cgis[cgi.first]=cgi.second;
	return (true);
}

bool	Conf::set_cgi(cgi_list cgi) {
	for (cgi_list::iterator it = cgi.begin(); it != cgi.end(); it++)
		if (!add_cgi(*it))
			return (false);
	return (true);
}

bool	Conf::set_dir_listing(dir_listing_type dir_listing) {
	if (_dir_listing != -1)
		return (set_error_message("Double parameter: directory_listing"));
	else if (dir_listing < 0)
		return (set_error_message("Invalid value: directory_listing"));
	_dir_listing = dir_listing;
	return (true);
}

bool	Conf::set_upload_path(path_type upload_path) {
	if (_upload_path != path_type())
		return (set_error_message("Double parameter: uploads"));
	else if (upload_path == "")
		return (set_error_message("Invalid value: uploads"));
	_upload_path = upload_path;
	return (true);
}

bool	Conf::add_route(route_type route) {
	for (route_list::iterator it = _routes.begin(); it != _routes.end(); it++)
		if (it->_path == route._path)
			return (set_error_message(route._path+": already exists"));
	if (route._err)
		return (set_error_message(route._error_message));
	_routes.push_back(route);
	return (true);
}

void	Conf::print() {
	std::cout << "name = " << _name << std::endl;
	std::cout << "hosts = ";
	for (host_list::iterator it = _hosts.begin(); it != _hosts.end(); it++)
	{
		if (it != _hosts.begin())
			std::cout << ", ";
		std::cout << it->first << ":" << it->second;
	}
	std::cout << std::endl;
	if (_error_pages != error_list())
		std::cout << "error_pages:" << std::endl;
	for (error_list::iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
		std::cout << "\t" << (*it).first << ": " << (*it).second << std::endl;
	std::cout << "client_body_size = " << _client_body_size << std::endl;
	std::cout << "methods = ";
	for (method_list::iterator it = _methods.begin(); it != _methods.end(); it++)
	{
		if (it != _methods.begin())
			std::cout << ", ";
		std::cout << *it;
	}
	if (_error_pages != error_list())
		std::cout << std::endl;
	if (_dir_listing != -1)
		std::cout << "directory listing = " << _dir_listing << std::endl;
	if (_upload_path != path_type())
		std::cout << "uploads = " << _upload_path << std::endl;
	std::cout << "routes:" << std::endl;
	for (route_list::iterator it = _routes.begin(); it != _routes.end(); it++)
	{
		(*it).print();
		std::cout << std::endl;
	}
}

bool		Conf::set_error_message(std::string error_message) {
	_error_message = _name+": "+error_message;
	_err = true;
	return (false);
}

bool		Conf::check() {
	if (_name == name_type())
		return (set_error_message("Required value: server_name"));
	else if (_hosts == host_list())
		return (set_error_message("Required value: host"));
	else if (_client_body_size == size_type())
		return (set_error_message("Required value: client_body_size"));
	else if (_routes == route_list())
		return (set_error_message("Required value: routes"));
	for (route_list::iterator it = _routes.begin(); it != _routes.end(); it++)
	{
		if (!it->check())
			return (set_error_message(it->_error_message));
		/*Recursive redirection check*/
		if (it->_redir != route_type::redir_type())
		{
			for (route_list::iterator it2 = _routes.begin(); it2 != _routes.end(); it2++)
			{
				if (it2->_path == it->_redir.second && it2->_redir != route_type::redir_type()\
				 && it2->_redir.second == it->_path)
				 	return (set_error_message("Redirections: Recursive loop"));

			}
		}
	}
	return (true);
}