/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 04:37:45 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/12 21:44:38 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <sys/time.h>

/**
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client () : response_generated(false)
{
	gettimeofday(&life_time, NULL);
}

/**
 * @brief Destroy the Client:: Client object
 * 
 */
Client::~Client ()
{
}

/**
 * @brief Construct a new Client by copy
 * 
 * @param copy 
 */
Client::Client (const Client & copy)
{
	*this = copy;
	return ;
}

/**
 * @brief overload of operator=
 * 
 * @param copy 
 * @return Client& 
 */
Client&		Client::operator= (const Client& copy)
{
	if (this != &copy)
	{
		i_msg = copy.i_msg;
		o_msg = copy.o_msg;
		response_generated = copy.response_generated;
		life_time = copy.life_time;
		// TODO finish this copy
	}
	return (*this);
}

/**
 * @brief This is where the client input is parsed and where the response is generated.
 * 
 * @details If o_msg is ready response_generated is set to true and it_chunk to o_bsg.begin()
 * 
 * @return true The response has an error and nothing has to be sent to client
 * @return false To get the rest of the input, or if o_msg is ready. 
 */
bool		Client::parse_and_generate_response ()
{
	// TODO parse the input and generate the message for the client
	// ? exemple :

	std::cout << GRN << "  parse_and_generate_response" << RST << std::endl;
	
	this->o_msg.push_back("HTTP/1.1 200 OK\r\nDate: Tue, 24 Aug 2021 06:20:56 WEST\r\nServer: webser:42 (popOS)\r\nLast-Modified: Wed, 24 Aug 2021 06:20:56 WEST\r\nContent-Length: 120\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html>\r\n<body>\r\n<h1>peepowidehappy</h1>\r\n</body>\r\n<img src='https://cdn.frankerfacez.com/emoticon/359928/2'/>\r\n</html>\r\n");
	this->response_generated = true;
	this->it_chunk = this->o_msg.begin();
	return (false);
}

/**
 * @brief Send the content of o_msg to the client.
 * 
 * @param sd_out File descriptor on wish the return message will be sent
 * @return true The message has been fully sent or there is an error
 * @return false The message is not completly sent
 */
bool		Client::send_response (int sd_out)
{
	int	rc;

	std::cout << GRN << "  sending response" << RST << std::endl;

	rc = send(sd_out, this->it_chunk->c_str(), this->it_chunk->size(), 0);
	if (rc < 0)
	{
		perror("  send() failed");
		return (true);
	}

	// ? get to the next output message chunk
	++this->it_chunk;
	if (this->it_chunk == this->o_msg.end())
		return (true);

	return (false);
}

/**
 * @brief Update life_time counter.
 */
void		Client::update ()
{
	gettimeofday(&this->life_time, NULL);
}

/**
 * @brief store a buffer in i_msg.
 * 
 * @param buffer an input buffer
 * @param len the lenght of 'buffer'
 */
void		Client::add_input_buffer (const char *buffer, int len)
{
	this->i_msg.push_back(std::string(buffer, len));
}

/**
 * @brief check if life_time is greater than CLIENT_TIMEOUT.
 * 
 * @return true last event was before CLIENT_TIMEOUT.
 * @return false last event was close enough.
 */
bool		Client::is_timed_out ()
{
	struct timeval	tv_now;
	struct timezone	tv_zone;

	// TODO make this cleaner without gettimeofday
	gettimeofday(&tv_now, &tv_zone);
	
	if (tv_now.tv_sec - this->life_time.tv_sec > CLIENT_TIMEOUT
		|| (tv_now.tv_sec < this->life_time.tv_sec && tv_now.tv_sec > CLIENT_TIMEOUT))
		return (true);
	return (false);
}

/**
 * @brief Getter on response_generated.
 * 
 * @return true o_msg is ready to be sent to the client.
 * @return false o_msg is still beeing made.
 */
bool		Client::is_output_ready ()
{
	return (this->response_generated);
}
