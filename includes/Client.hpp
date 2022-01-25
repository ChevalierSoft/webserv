/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 00:54:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 18:07:47 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <iostream>
#include "color.h"
#include "Response.hpp"
#include "Request.hpp"
#include "Conf.hpp"
#include "ResponseGenerator.hpp"

#define CLIENT_TIMEOUT 10 		// time (s) after which the client connection will be closed if there is no event

class ResponseGenerator;

/**
 * @brief Client will store inputs and outputs
 * 
 * @details This class need to call a parser every time an input is added
 *          in i_msg. at the same time a response will be generated.
 * 
 */
class Client // * ______________________________________________________________
{

	friend class			ResponseGenerator;

	/// * Variables ____________________________________________________________

private:
	Request					_request;
	Response				_response;
	bool					_request_ready;	// will stop the reading to send o_msg's content
	bool					_response_ready;
	Response::it_chunk		_it_chunk;			// points on the begining of o_msg
	struct timeval			_life_time;			// will be updated every event. after CLIENT_TIMEOUT the client is erased and the connection is closed
	// const Conf*				_conf;

	/// * Constructors & Destructors ___________________________________________

public:
	// ? (1) default. should not be used without beeing set
	Client ();

	// ? (2) with server's config
	Client (const Conf* c);

	// ? (3) by copy
	Client (const Client & copy);

	~Client ();

	/// * Member function ______________________________________________________

	Client&	operator= (const Client& copy);

	void	parse_response ();

	bool	send_response (int sd_out);

	void	update ();

	bool	is_timed_out ();

	void	add_input_buffer (const char *buffer, int len);

	bool	is_request_parsed ();

	bool	is_response_ready ();

}; // * ________________________________________________________________________
