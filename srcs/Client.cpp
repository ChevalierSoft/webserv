/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 04:37:45 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 18:11:21 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "webserv.hpp"
#include "Request.hpp"
#include <sys/time.h>

/**
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client () : _request_ready(false), _response_ready(false)
{
	gettimeofday(&_life_time, NULL);
}

/**
 * @brief Construct a new Client:: Client object with it's conf
 * 
 */
Client::Client (const Conf* c) : _request_ready(false), _response_ready(false)
{
	gettimeofday(&_life_time, NULL);
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
		_request = copy._request;
		_response = copy._response;
		_request_ready = copy._request_ready;
		_response_ready = copy._response_ready;
		_it_chunk = copy._it_chunk;
		_life_time = copy._life_time;
	}
	return (*this);
}

/**
 * @brief This is where the client input is parsed.
 */
void		Client::parse_response ()
{
	int	end_of_request;

	while (this->_request._in_header && (end_of_request = this->_request.update_header()) == 0);
	if (this->_request._error > 0) {
		this->_request_ready = true;
		std::cout << RED << "Error in header : " << this->_request._error << " (refer to errors enum)" << RST << std::endl;
		return ;
	}

	if (!this->_request._in_header && this->_request._method != "POST") {
		std::cout << MAG << "alloOOOOOO0" << std::endl;
		this->_request_ready = true;
		end_of_request = 2;
	}
	if (!this->_request_ready && !this->_request._in_header)
		while ((end_of_request = this->_request.update_body()) == 1);
	
	if (end_of_request == 2) {
		// ? to output contents of map header
		this->_it_chunk = this->_request.begin_header();
		std::cout << GRN << "HEADER" << RST << std::endl;
		std::cout << RED << "Method: " << this->_request._method << RST << std::endl;
		std::cout << RED << "path: " << this->_request._path << RST << std::endl;
		std::cout << RED << "http-version: " << this->_request._http_version << RST << std::endl;
		for (; _it_chunk != this->_request.end_header(); _it_chunk++)
			std::cout << RED << (*(_it_chunk)).first << ": " << (*(_it_chunk)).second << RST << std::endl;
		std::vector<std::string>::iterator it_test = this->_request.begin_body();
		std::cout << GRN << "BODY" << RST << std::endl;
		for (; it_test != this->_request.end_body(); it_test++)
			std::cout << RED << *it_test << RST << std::endl;
		this->_request_ready = true;
	}

	return ;
}

/**
 * @brief store a buffer in i_msg.
 * 
 * @param buffer an input buffer
 * @param len the lenght of 'buffer'
 */
void		Client::add_input_buffer (const char *buffer, int len)
{
	this->_request.append_buffer(std::string(buffer, len));
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

	// ? clear request since response is generated
	this->_request.clear();
	// rc = send(sd_out, ((*(this->_it_chunk)).second).c_str(), ((*(this->_it_chunk)).second).size(), 0);
	// ? For now, sending default response in one go
	rc = send(sd_out, this->_response.get_buffer().c_str(), this->_response.get_buffer().size(), 0);
	if (rc < 0)
	{
		perror("  send() failed");
		return (true);
	}
	// ? Setting generated response to false after each send for now
	this->_request_ready = false;
	return true;

	// ? get to the next output message chunk
	// ++this->_it_chunk;
	// if (this->_it_chunk == this->_response.end_header())
	// 	return (true);

	// return (false);
}

/**
 * @brief Update _life_time counter.
 */
void		Client::update ()
{
	gettimeofday(&this->_life_time, NULL);
}

/**
 * @brief check if _life_time is greater than CLIENT_TIMEOUT.
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
	
	if (tv_now.tv_sec - this->_life_time.tv_sec > CLIENT_TIMEOUT
		|| (tv_now.tv_sec < this->_life_time.tv_sec && tv_now.tv_sec > CLIENT_TIMEOUT))
		return (true);
	return (false);
}

bool		Client::is_response_ready ()
{
	return (this->_response_ready);
}

bool		Client::is_request_parsed ()
{
	return (this->_request_ready);
}
