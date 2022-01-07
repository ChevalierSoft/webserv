/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 00:54:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 07:56:35 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <iostream>
#include "color.h"

// ? Client will store it's input and the message we are sending back to him
class Client // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
public:
	std::vector<std::string>						i_msg;		// the input message buffer_size by buffer_size
	std::vector<std::string>						o_msg;		// the output message to send to the client
	bool											response_generated;		// will stop the reading to send o_msg's content
	std::vector<std::string>::const_iterator		it_chunk;
	// TODO : need to add a lifetime value

	/// * Constructors & Destructors ___________________________________________

	// (1) default
	Client ();

	// (2) by copy
	Client (const Client & copy);

	~Client ();

	/// * Member function ______________________________________________________

	Client&	operator= (const Client& copy);

	bool	parse_and_generate_response ();

	bool	send_response (int sd_out);

}; // * ________________________________________________________________________
