/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 04:37:45 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 08:01:00 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void)
{
}

Client::~Client(void)
{
}

Client::Client(const Client & copy)
{
	*this = copy;
	return ;
}

Client&		Client::operator=(const Client& copy)
{
	if (this != &copy)
	{
		i_msg = copy.i_msg;
		o_msg = copy.o_msg;
		response_generated = copy.response_generated;
		// TODO finish this copy
	}
	return (*this);
}

// ? return true if the response has an error and nothing has to be sent to client
// ? else return false to get the rest of the input,
// ?   or if o_msg is ready (also set response_generated to true and it_chunk to o_bsg.begin())
bool		Client::parse_and_generate_response()
{
	// TODO parse the input and generate the message for the client
	// ? exemple :

	std::cout << YEL << "parse_and_generate_response" << RST << std::endl;
	
	this->o_msg.push_back("HTTP/1.1 200 OK\r\nDate: Tue, 24 Aug 2021 06:20:56 WEST\r\nServer: webser:42 (popOS)\r\nLast-Modified: Wed, 24 Aug 2021 06:20:56 WEST\r\nContent-Length: 120\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html>\r\n<body>\r\n<h1>peepowidehappy</h1>\r\n</body>\r\n<img src='https://cdn.frankerfacez.com/emoticon/359928/2'/>\r\n</html>\r\n");
	this->response_generated = true;
	this->it_chunk = this->o_msg.begin();
	return (false);
}

// ? return true if the message has been fully sent or if there is an error
// ? return false if the message is not completly sent
bool		Client::send_response(int sd_out)
{
	int											rc;

	std::cout << YEL << "sending response" << RST << std::endl;

	rc = send(sd_out, this->it_chunk->c_str(), this->it_chunk->size(), 0);
	if (rc < 0)
	{
		perror("  send() failed");
		return (true);
	}

	// ? get to the next output message chunk
	++this->it_chunk;
	if (it_chunk == this->o_msg.end())
		return (true);

	return (false);
}