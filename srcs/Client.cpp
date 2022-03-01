/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 04:37:45 by dait-atm          #+#    #+#             */
/*   Updated: 2022/03/01 12:25:37 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/**
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client ()
: _response(""), _request_ready(false), _response_ready(false), _ip(), _port(), _body_sent(false),
	_fast_forward(FF_NOT_SET), _request_parsed(false), _child(-1)
{
	_tmp_counter = 0;
	gettimeofday(&_life_time, NULL);
	_webserv_pipe[0] = -1;
	_webserv_pipe[1] = -1;
	_cgi_pipe[0] = -1;
	_cgi_pipe[1] = -1;
}

/**
 * @brief Construct a new Client:: Client object with it's conf
 * 
 */
Client::Client (const Conf* c, std::string ip, std::string port)
: _response(""), _request_ready(false), _response_ready(false), _ip(ip), _port(port), _body_sent(false),
	_fast_forward(FF_NOT_SET), _request_parsed(false), _child(-1)
{
	_tmp_counter = 0;
	gettimeofday(&_life_time, NULL);
	_webserv_pipe[0] = -1;
	_webserv_pipe[1] = -1;
	_cgi_pipe[0] = -1;
	_cgi_pipe[1] = -1;
}

/**
 * @brief Destroy the Client:: Client object
 * 
 */
Client::~Client ()
{
	this->clean_cgi();
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
		_life_time = copy._life_time;
		_ip = copy._ip;
		_port = copy._port;
		_body_sent = copy._body_sent;
		_fast_forward = copy._fast_forward;
		_cgi = copy._cgi;
		_child = copy._child;
		// _input_file = copy._input_file;
		_tmp_counter = copy._tmp_counter;
		_request_parsed = copy._request_parsed;
		_webserv_pipe[0] =  copy._webserv_pipe[0];
		_webserv_pipe[1] =  copy._webserv_pipe[1];
		_cgi_pipe[0] =  copy._cgi_pipe[0];
		_cgi_pipe[1] =  copy._cgi_pipe[1];
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
		// std::cout << RED << "Error in header : " << this->_request._error << " (refer to errors enum)" << RST << std::endl;
	}

	if (!this->_request._in_header && this->_request._method != "POST") {
		this->_request_ready = true;
		end_of_request = 2;
	}
	if (!this->_request_ready && !this->_request._in_header)
		while ((end_of_request = this->_request.update_body()) == 1);
	
	if (end_of_request == 2) {
		// this->_request.d_output(); // ? to output contents of map header
		this->_request_ready = true;
	}
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

	std::cout << "    sending response" << std::endl;
	rc = send(sd_out, this->_response.c_str(), this->_response.size(), 0);
	if (rc <= 0)
	{
		perror("    send() failed");
		exit(53);
		return (false);
	}
	this->_response.clear();
	return (true);
}

void		Client::clean_cgi ()
{
	if (_webserv_pipe[0] != -1)
	{
		close(_webserv_pipe[0]);
		_webserv_pipe[0] = -1;
	}
	if (_child > 0)
	{
		std::cout << "    tue " << _child << std::endl;
		kill(_child, SIGTERM);
		_child = -1;
		close(_webserv_pipe[1]);
		_webserv_pipe[1] = -1;
		close(_cgi_pipe[0]);
		_cgi_pipe[0] = -1;
		close(_cgi_pipe[1]);
		_cgi_pipe[1] = -1;
	}
}

/**
 * @brief Update _life_time counter.
 */
void		Client::update ()
{
	gettimeofday(&this->_life_time, NULL);
	// _life_time = std::clock();
}

/**
 * @brief check if _life_time is greater than CLIENT_TIMEOUT.
 * 
 * @return true last event was before CLIENT_TIMEOUT.
 * @return false last event was close enough.
 */
bool		Client::is_timed_out () const
{
	struct timeval	tv_now;
	struct timezone	tv_zone;

	// // TODO make this cleaner without gettimeofday
	gettimeofday(&tv_now, &tv_zone);

	// std::clock_t c_now = std::clock();
	
	// std::cout << (c_now - _life_time) / CLOCKS_PER_SEC << std::endl;

	// if ((c_now - _life_time) / CLOCKS_PER_SEC > CLIENT_TIMEOUT)
	if (tv_now.tv_sec - this->_life_time.tv_sec > CLIENT_TIMEOUT
		|| (tv_now.tv_sec < this->_life_time.tv_sec && tv_now.tv_sec > CLIENT_TIMEOUT))
		return (true);
	return (false);
}

bool		Client::is_response_ready () const
{
	return (this->_response_ready);
}

bool		Client::is_request_parsed () const
{
	return (this->_request_ready);
}

e_preforms	Client::get_performing_state() const
{
	return (this->_fast_forward);
}

int			Client::get_cgi_input_fd() const
{
	return (_webserv_pipe[0]);
}