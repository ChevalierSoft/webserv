/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 00:54:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/11 05:26:49 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_H
# define CONF_H

# include <string>
# include <map>
# include <vector>
# include <fstream>

# include "typing.hpp"
# include "Route.hpp"

class Conf {
	public:
		name_type			_name;
		host_type			_host;
		port_type			_port;
		error_list			_error_pages;
		size_type			_client_body_size;
		method_list			_methods;
		dir_listing_type	_dir_listing;
		path_type			_upload_path;
		route_list			_routes;

		std::string			_error_message;
		bool				_err;

	private:

		bool                            parse_file(std::ifstream &ifs);
		std::string                     parse_param(std::string &line, const char sep);
		std::string                     parse_value(std::string &line,std::string param);
		bool                            set_param(std::string &line, std::vector<std::string> param, size_t indent);

		bool							zero_indent(std::string	param, std::string value);
		bool							one_indent(std::vector<std::string> params, std::string value);
		bool							two_indent(std::vector<std::string> params, std::string value);
		bool							three_indent(std::vector<std::string> params, std::string value);

		std::string						remove_comments(std::string &line, const char sep);
		std::string						remove_whitespaces(std::string line);


		port_type                       string_to_port(std::string value);
		code_type                       string_to_code(std::string value);
		error_type                      string_to_error(std::string error);
		size_type                       string_to_client_body_size(std::string value);
		method_list						string_to_methods(std::string value);
		dir_listing_type				string_to_dir_listing(std::string value);
		route_type    					string_to_route(std::string value);
		
	public:
		Conf();
		Conf(file_type  conf_file);
		Conf(Conf &copy);
		Conf &operator=(Conf & rhs) const;
		~Conf();

		bool        set_name(name_type name);
		bool        set_host(host_type host);
		bool        set_port(port_type port);
		bool        add_error(error_type  error);
		bool        set_client_body_size(size_type client_body_size);
		bool        add_method(method_type method);
		bool		set_methods(method_list methods);
		bool        set_dir_listing(dir_listing_type dir_listing);
		bool        set_upload_path(path_type upload_path);
		bool        add_route(route_type route);

		void		print();

		bool		set_error_message(std::string error_message);
};

#endif