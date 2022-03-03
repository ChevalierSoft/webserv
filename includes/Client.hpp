/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 00:54:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/03/03 16:09:32 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

#define CLIENT_TIMEOUT 10 		// time (s) after which the client connection will be closed if there is no event

class ResponseGenerator;

enum e_preforms
{
	FF_NOT_SET = 0,
	FF_GET_FILE,
	FF_GET_CGI,
	FF_FILE_WAITING_TO_BE_IN__FDS,
	FF_CGI_WAITING_TO_BE_IN__FDS,
	FF_READY,
	FF_REDIRECT
};

/**
 * @brief Client will store inputs and outputs
 * 
 * @details This class need to call a parser every time an input is added
 *          in i_msg. at the same time a response will be generated.
 * 
 */
class Client // * ______________________________________________________________
{

	friend class					ResponseGenerator;

	/// * Variables ____________________________________________________________

private:
	Request							_request;
	std::string						_response;
	bool							_request_ready;			// will stop the reading to send o_msg's content
	bool							_response_ready;
	struct timeval					_life_time;				// will be updated every event. after CLIENT_TIMEOUT the client is erased and the connection is closed
	int								_webserv_pipe[2];
	int								_cgi_pipe[2];
	e_preforms						_fast_forward;
	bool							_request_parsed;
	pid_t							_child;
	Route::cgi_list::const_iterator	_cgi;
	std::ifstream					_input_file;
	std::vector<std::string>		s_envs;

public:
	std::string						_ip;
	std::string						port;

	/// * Constructors & Destructors ___________________________________________

public:
	// ? (1) default. should not be used without beeing set
	Client ();

	// ? (2) with client ip and client port
	Client  (std::string ip, std::string port);

	// ? (3) by copy
	Client (const Client & copy);

	~Client ();

	/// * Member function ______________________________________________________

	Client&		operator= (const Client& copy);

	void		parse_response ();

	bool		send_response (int sd_out);

	void		clean_cgi ();

	void		update ();

	bool		is_timed_out () const;

	void		add_input_buffer (const char *buffer, int len);

	bool		is_request_parsed () const;

	bool		is_response_ready () const;

	e_preforms	get_performing_state() const;

	int			get_cgi_input_fd() const;

}; // * ________________________________________________________________________
