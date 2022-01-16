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

class Conf {
	public:
		typedef std::string                     name_type;
		typedef std::string                     host_type;
		typedef short                           port_type;
		typedef std::string                     file_type;
		typedef size_t                          code_type;
		typedef std::pair<code_type, file_type> error_type;
		typedef size_t                          size_type;
		typedef bool                            dir_listing_type;
		typedef std::string                     path_type;
		typedef class Route {
			friend class Conf;
			private:
				path_type                        _path;
				std::vector<name_type>          _methods;
				std::pair<code_type, path_type>  _redir;
				path_type                        _root;
				file_type                       _file;
				dir_listing_type                _dir_listing;
				path_type                        _upload_path;
				std::map<name_type, path_type>  _cgis;
		}                       route_type;
	private:
		name_type                       _name;
		host_type                       _host;
		port_type                       _port;
		std::map<code_type, file_type>  _error_pages;
		size_type                       _client_body_size;
		std::vector<name_type>          _methods;
		dir_listing_type                _dir_listing;
		path_type                       _upload_path;
		std::vector<route_type>         _routes;
		
		bool                            parse_file(std::ifstream &ifs);
		
		std::string                     parse_param(std::string &line, const char sep);
		std::string                     parse_value(std::string &line,std::string param);
		bool                            set_param(std::string &line, std::string param);
		bool							set_param_indent1(std::string &line, std::string param);

		std::string						remove_comments(std::string &line, const char sep);
		std::string						remove_whitespaces(std::string line);


		port_type                       string_to_port(std::string value);
		code_type                       string_to_code(std::string value);
		error_type                      string_to_error(std::string error);
		size_type                       string_to_client_body_size(std::string value);
		
	public:
		Conf();
		Conf(file_type  conf_file);
		~Conf();

		bool        set_name(name_type name);
		bool        set_host(host_type host);
		bool        set_port(port_type port);
		bool        add_error(error_type  error);
		bool        set_client_body_size(size_type client_body_size);
		bool        add_method(name_type method);
		bool        set_dir_listing(dir_listing_type dir_listing);
		bool        set_upload_path(path_type upload_path);
		bool        set_routes(std::vector<route_type> routes);
};

#endif