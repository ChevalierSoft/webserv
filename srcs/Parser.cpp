#include "Parser.hpp"
#include <algorithm>
#include <iostream>

Parser::Parser(file_type conf_file): _err(0), line(std::string()), line_number(0) {
	std::ifstream ifs(conf_file.c_str(), std::ifstream::in);
	_err |= !(ifs.is_open());
	if (!_err)
	{
		_err |= !parse_file(ifs);
		if (_error_message == "")
		{
			if (!_confs.back()._err)
				_confs.back().set_error_message(_confs.back()._routes.back()._error_message);
			_error_message = _confs.back()._error_message;
		}
	}
	else
		_error_message = "File not found";
}

Parser::~Parser() {}

bool	Parser::parse_file_loop(param_list &params)
{
	std::string	parsed_line;
	indent_type		indent;
	sep_type  		sep = ':', comment_sep = '#', indent_sep = '\t';

	parsed_line = remove_comments(line, comment_sep);
	indent = 0;
	while (parsed_line[indent] == indent_sep)
		indent++;;
	parsed_line = remove_whitespaces(parsed_line);
	if (*(params.insert(params.begin() + indent, parse_param(parsed_line, sep))) == "error")
		return (false);
	else if (!(set_param(parsed_line, params, indent)))
		return (false);
	return (true);
}

bool    Parser::parse_file(std::ifstream &ifs) {

	param_list		params;
	bool			b = 1;
	sep_type  		comment_sep = '#';
	
	while (b)
	{	
		while (remove_comments(line, comment_sep) == "" && b)
		{
			line_number++;
			b = std::getline(ifs, line).good();
		}
		if (b || remove_comments(line, comment_sep) != "")
		{
			
			Conf	c;
			_confs.push_back(c);
			if (!parse_file_loop(params))
				return (false);
			while (line != "" && b)
			{	
				b = std::getline(ifs, line).good();
				line_number++;
				if (remove_comments(line, comment_sep) != "")
					if (!parse_file_loop(params))
						return (false);
			}
			params.clear();
		}
	}
	return (check());
}

Parser::param_type Parser::parse_param(line_type &line, sep_type sep) {
	size_t      found;

	if ((found = line.find(sep)) != std::string::npos)
		return (remove_whitespaces(line.substr(0, found)));
	_error_message = "Missing separator";
	return ("error");
}

Parser::param_type Parser::parse_value(line_type &line, param_type param) {
	return (std::string(line.substr(param.size() + 1)));
}

Parser::line_type	Parser::remove_comments(line_type line, sep_type sep) {
	size_t      found;

	if ((found = line.find(sep)) != std::string::npos)
		return (line.substr(0, found));
	else
		return (line);
}

bool    Parser::set_param(line_type &line, param_list params, indent_type indent) {
	std::string value(parse_value(line, params[indent]));
	if (indent ==  0)
		return (zero_indent(params[0], value));
	else if (indent == 1)
		return (one_indent(params, value));
	else if (indent == 2)
		return (two_indent(params, value));
	else if (indent == 3)
		return (three_indent(params, value));
	return (set_error_message("Bad indentation"));
}


bool	Parser::zero_indent(param_type param, std::string value)
{
	conf_type	&conf = _confs.back();

	if (param == "server_name")
		return (conf.set_name(value));
	else if (param == "host")
		return (conf.set_host(value));
	else if (param == "port")
		return (conf.set_port(conf.string_to_port(value)));
	else if (param == "client_body_size")
		return (conf.set_client_body_size(conf.string_to_client_body_size(value)));
	else if (param == "methods")
		return (conf.set_methods(conf.string_to_methods(value)));
	else if (param == "cgi")
		return (conf.set_cgi(conf.string_to_cgi(value)));
	else if (param == "directory_listing")
		return (conf.set_dir_listing(conf.string_to_dir_listing(value)));
	else if (param == "uploads")
		return (conf.set_upload_path(value));
	else if (param == "error_pages")
		return (true);
	else if (param == "routes")
		return (true);
	return (set_error_message("Invalid parameter"));
}

bool	Parser::one_indent(param_list params, std::string value)
{
	conf_type	&conf = _confs.back();

	if (params[0] == "error_pages")
		return (conf.add_error(std::make_pair(conf.string_to_code(params[1]), value)));
	else if (params[0] == "routes")
		return (conf.add_route(conf.string_to_route(params[1])));
	return (set_error_message("Invalid parameter"));
}

bool	Parser::two_indent(param_list params, std::string value)
{
	conf_type	&conf = _confs.back();

	if (params[0] == "routes")
	{
		Route	&route = conf._routes.back();
		if (params[2] == "methods")
			return (route.set_methods(conf.string_to_methods(value)));
		else if (params[2] == "location")
			return (route.set_location(value));
		else if (params[2] == "directory_listing")
			return (route.set_dir_listing(conf.string_to_dir_listing(value)));
		else if (params[2] == "default_file")
			return (route.set_default_file(value));
		else if (params[2] == "uploads")
			return (route.set_upload_path(value));
		else if (params[2] == "redirections")
			return (true);
		else if (params[2] == "cgi")
			return (route.set_cgi(conf.string_to_cgi(value)));
	}
	return (set_error_message("Invalid parameter"));
}

bool	Parser::three_indent(param_list params, std::string value) {
	conf_type	&conf = _confs.back();

	if (params[0] == "routes")
	{
		Route	&route = conf._routes.back();
		if (params[2] == "redirections")
			return (route.set_redir(std::make_pair(conf.string_to_code(params[3]), value)));
	}
	return (set_error_message("Invalid parameter"));
}

bool	is_whitespace (const char c) { 
	return (c == ' ' || c == '\t'); 
}

std::string	Parser::remove_whitespaces(std::string s) {
	size_t      found;

	std::string::iterator end_pos = std::remove_if(s.begin(), s.end(), is_whitespace);
	s.erase(end_pos, s.end());
	return (s);
}

bool		Parser::set_error_message(std::string error_message) {
	_error_message = error_message;
	return (false);
}

bool	Parser::check() {
	for (conf_list::iterator it = _confs.begin(); it != _confs.end(); it++)
		if (!it->check())
			return (set_error_message(it->_error_message));
	return (true);
}