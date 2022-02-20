/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:55:53 by ljurdant          #+#    #+#             */
/*   Updated: 2022/02/20 07:23:59 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

# define CGI_BUFF_SIZE	16384
# define FILE_BUFF_SIZE	65536

class Client;

/**
 * @brief Thanks to the parsed Request,
 * 	      ResponseGenerator will generate the response for the client
 */
class ResponseGenerator // * ___________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	const Conf										*_conf;
	const std::vector<Conf>							*_confs;
	static const std::map<std::string, std::string>	_ss_content_types;
	static const std::map<int, std::string>			_ss_error_messages;

	/// * Constructors & Destructors ___________________________________________
public:
	// ? (1) default
	ResponseGenerator (void);

	// ? (2) by copy
	ResponseGenerator (const ResponseGenerator & copy);

	~ResponseGenerator (void);

	ResponseGenerator&	operator= (const ResponseGenerator &	copy);

	/// * Member function ______________________________________________________
public:
	bool				generate (Client & client) const;

	void				set_conf (const Conf * c);

	void				set_confs(const std::vector<Conf> * confs);

	bool				listen_cgi (Client & client) const;
	
private:

	void				get_file_content (Client & client) const;

	std::string			set_file_content_type (const std::string & extention) const;

	std::string			set_header (int err, std::string ct, size_t size) const;

	std::string			generic_error (int err) const;

	void				get_error_file (Client & client, int err) const;

	void				set_cgi_env (Client & client, std::string path, std::vector<std::string> & se, std::vector<char *> & ae) const;

	void				start_cgi (Client & client, std::string url, std::string path, int cgi_pipe[2]) const;

	bool				cgi_send_body (Client & client, int cgi_pipe[2]) const;

	void				cgi_handling (Client & client, std::string url, std::string path) const;

	void				perform_method (Client & client) const;

	void				perform_delete(Client & client) const;

	int					is_method(std::string method, Request const & rq) const;

	void				set_conf_index(Client &client) const;

	void				parse_request_route(Client &client) const;

	bool				is_directory(const std::string path) const;

	std::string			get_redirection(const Route::redir_type & redir) const;

}; // * ________________________________________________________________________
