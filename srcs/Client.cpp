/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 04:37:45 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 11:23:40 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "webserv.hpp"
#include <sys/time.h>
#include <sys/types.h>	// stat
#include <sys/stat.h>	// stat
#include <unistd.h>		// stat

/**
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client () : response_generated(false)
{
	gettimeofday(&life_time, NULL);
}

/**
 * @brief Construct a new Client:: Client object with it's conf
 * 
 */
Client::Client (const Conf* c) : _conf(c), response_generated(false)
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
		_request = copy._request;
		_response = copy._response;
		response_generated = copy.response_generated;
		_it_chunk = copy._it_chunk;
		life_time = copy.life_time;
		_conf = copy._conf;
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
	struct stat s;

	this->_response.clear();
	std::string	root = ".";

	if ( ! stat((root + _request._path).c_str(), &s))
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			// TODO : check if directory indexation in on.

			// TODO : see if we have to redirect to index.html if it exists.
			
			this->_response.append_buffer(directory_listing(root, this->_request._path).c_str());
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			// TODO : generate a file
			// this->_response.append_buffer(get_file_content(root, this->_request._path).c_str());
			std::cout << "store '" << root + _request._path << "' through _response.append_buffer()" << std::endl;
		}
		else
		{
			// ? error: it's not a directory or a file.
			// ? not sure if symlinks must work. 
		}
	}
	else
	{
		// ? basically 404
		// ? error: wrong path || path too long || out of memory || bad address || ...
	}

	this->response_generated = true;
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
	// this->response_generated = false;
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
