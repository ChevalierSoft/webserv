/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 04:37:45 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/20 05:21:12 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "webserv.hpp"
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
		// i_msg = copy.i_msg;
		// o_msg = copy.o_msg;
		_response = copy._response;
		_request = copy._request;
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
	// int	end_of_response;

	// std::cout << GRN << "  parse_and_generate_response" << RST << std::endl;


	// std::cout <<CYN << this->_request._path <<RST<< std::endl;

	std::string	root = "/mnt/nfs/homes/dait-atm/Git/webserv/";
	this->_response.append_buffer(directory_listing(root, this->_request._path).c_str());


	// this->_response.append_buffer("HTTP/1.1 200 OK\r\nDate: Tue, 24 Aug 2021 06:20:56 WEST\r\nServer: webserv:42 (popOS)\r\nLast-Modified: Wed, 24 Aug 2021 06:20:56 WEST\r\nContent-Length: 120\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html>\r\n<body>\r\n<h1>peepowidehappy</h1>\r\n</body>\r\n<img src='https://cdn.frankerfacez.com/emoticon/359928/2'/>\r\n</html>\r\n");
	// ? response if for the time being default
	// while ((end_of_response = this->_response.update_header()) == 0);
	// if (end_of_response == 2)
	// this->response_generated = true;
	// if (!this->response_generated){
		// while ((end_of_response = this->_response.update_body()) == 1);
		// if (end_of_response == 2)
	this->response_generated = true;
	// }
	this->_it_chunk = this->_response.begin_header();
	// std::cout << RED << "TEST :" << (*(this->_it_chunk)).second << std::endl;
	return (true);
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
	this->response_generated = false;
	return true;

	// ? get to the next output message chunk
	// ++this->_it_chunk;
	// if (this->_it_chunk == this->_response.end_header())
	// 	return (true);

	// return (false);
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
	int	end_of_request;

	std::cout << GRN << "BEFORE APPEND" << std::endl;
	ft_print_memory((void *)(_request.get_buffer().c_str()), _request.get_buffer().size());
	this->_request.append_buffer(std::string(buffer, len));
	std::cout << GRN << "AFTER APPEND" << std::endl;
	ft_print_memory((void *)(_request.get_buffer().c_str()), _request.get_buffer().size());
	while (this->_request._in_header && (end_of_request = this->_request.update_header()) == 0);

	if (!this->_request._in_header && this->_request._method != "POST") {
		this->response_generated = true;
		end_of_request = 2;
	}
	if (!this->response_generated && !this->_request._in_header)
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
		this->response_generated = true;
	}
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
