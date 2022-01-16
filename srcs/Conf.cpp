
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
	std::string param;
	size_t		indent;
	
	while (std::getline(ifs, line).good() && line == "")
	{
		line = remove_comments(line, comment_sep);
		indent = line.find(indent_sep);
		if (indent == std::string::npos)
		{
			if ((param = parse_param(line, sep)) != "error")
				return (false);
			else if (!(set_param(line, param)))
				return (false);
		}
		else if (indent == 1)
		{
			set_param_indent1(line, param);
		}
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
	return (remove_whitespaces(std::string(line.substr(param.size() + 1))));
}

bool    Conf::set_param(std::string &line, std::string param) {
	std::string value(parse_value(line, param));

	if (!param.compare("server_name"))
		return (set_name(value));
	else if (!param.compare("host"))
		return (set_host(value));
	else if (!param.compare("port"))
		return (set_port(string_to_port(value)));
	else if (!param.compare("error_pages"))
		return (true);
	else if (!param.compare("client_body_size"))
		return (set_client_body_size(string_to_client_body_size(value)));
	else
		return (false);
}

bool	Conf::set_param_indent1(std::string &line, std::string param)
{
	if (!param.compare("error_pages"))
		return (add_error(string_to_error(line)));
	else
		return (false);
}

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
	_client_body_size = client_body_size;
	return (true);
}

