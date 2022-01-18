/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 00:54:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/12 21:43:59 by dait-atm         ###   ########.fr       */
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

#define CLIENT_TIMEOUT 10 		// time (s) after which the client connection will be closed if there is no event

/**
 * @brief Client will store inputs and outputs
 * 
 * @details This class need to call a parser every time an input is added
 *          in i_msg. at the same time a response will be generated.
 * 
 */
class Client // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	// TODO i_msg and o_msg may be changed to vectors of char * to handle \0 in the message
	// ? i_msg and o_msg might be ported to std::list
	std::list<std::string>						i_msg;				// the input message buffer_size by buffer_size
	std::list<std::string>						o_msg;				// the output message to send to the client
	bool										response_generated;	// will stop the reading to send o_msg's content
	std::list<std::string>::const_iterator		it_chunk;			// points on the begining of o_msg
	struct timeval								life_time;			// will be updated every event. after CLIENT_TIMEOUT the client is erased and the connection is closed

	/// * Constructors & Destructors ___________________________________________

public:
	// (1) default
	Client ();

	// (2) by copy
	Client (const Client & copy);

	~Client ();

	/// * Member function ______________________________________________________

	Client&	operator= (const Client& copy);

	bool	parse_and_generate_response ();

	bool	send_response (int sd_out);

	void	update ();

	bool	is_timed_out ();

	void	add_input_buffer (const char *buffer, int len);

	bool	is_output_ready ();

}; // * ________________________________________________________________________
