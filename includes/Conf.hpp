/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: %F{207}%n%f <%F{207}%n%f@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 16:44:53 by %F{207}%n%f       #+#    #+#             */
/*   Updated: 2022/02/09 16:44:58 by %F{207}%n%f      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_H
# define CONF_H

#include "webserv.hpp"
class Route;

class Conf {
	public:
		typedef std::string                     name_type;
		typedef std::string                     host_type;
		typedef int                             port_type;
		typedef std::pair<host_type, port_type>	host_pair;
		typedef std::vector<host_pair>			host_list;
		typedef std::string                     file_type;
		typedef int                             code_type;
		typedef std::pair<code_type, file_type> error_type;
		typedef	std::map<code_type, file_type>	error_list;
		typedef int                             size_type;
		typedef std::string						method_type;
		typedef std::vector<method_type>		method_list;
		typedef short                           dir_listing_type;
		typedef std::string                     path_type;
		typedef class Route                     route_type;
		typedef std::vector<route_type>			route_list;
		typedef bool                            errno_type;
		typedef std::string                     error_message_type;
		typedef std::pair<name_type, path_type>	cgi_type;
		typedef std::map<name_type, path_type>	cgi_list;

		name_type			_name;
		host_list			_hosts;
		error_list			_error_pages;
		size_type			_client_body_size;
		method_list			_methods;
		dir_listing_type	_dir_listing;
		path_type			_upload_path;
		route_list			_routes;
		cgi_list			_cgis;

		std::string			_error_message;
		bool				_err;

		Conf();
		Conf(file_type  conf_file);
		Conf(Conf const &copy);
		Conf &operator=(Conf const & rhs);
		~Conf();

		port_type                       string_to_port(std::string value);
		host_pair						string_to_host_pair(std::string value);
		host_list						string_to_hosts(std::string value);
		code_type                       string_to_code(std::string value);
		error_type                      string_to_error(std::string error);
		size_type                       string_to_client_body_size(std::string value);
		method_list						string_to_methods(std::string value);
		dir_listing_type				string_to_dir_listing(std::string value);
		route_type    					string_to_route(std::string value);
	
		bool        set_name(name_type name);
		bool		add_host(host_pair host);
		bool        set_hosts(host_list hosts);
		bool        add_error(error_type  error);
		bool        set_client_body_size(size_type client_body_size);
		bool        add_method(method_type method);
		bool		set_methods(method_list methods);
		bool        add_cgi(cgi_type cgi);
		bool		set_cgi(cgi_list cgi);
		bool        set_dir_listing(dir_listing_type dir_listing);
		bool        set_upload_path(path_type upload_path);
		bool        add_route(route_type route);

		void		print();

		bool		set_error_message(std::string error_message);

		bool		check();
};

#endif