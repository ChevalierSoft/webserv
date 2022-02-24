/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/24 08:21:36 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

# define BACK_LOG				123
# define REQUEST_BUFFER_SIZE	1024

# define TIMEOUT				10 * 1000

class ServerExeption : public std::exception
{
	// std::string	msg;
public:
	// ServerExeption(const std::string & s) : msg(s) {}
	// ServerExeption(const ServerExeption & copy) : msg(copy.msg)	{}
	// // todo : add operator =
	// virtual ~ServerExeption() {}
	virtual const char* what() const throw ()
	{
		// return msg.c_str();
		// return ("Child terminated before execve");
		return ("CHILD TERMINATED BEFORE EXECVE");
	}
};

// typedef void (*ScriptFunction)(::Client &) const;

/**
 * @brief a http/1.1 server working for one config
 */
class Server // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	int								_listen_sd;
	std::vector<struct pollfd>		_fds;
	std::map<int, Client>			_clients;
	ResponseGenerator				_response_generator;

	std::map<int, int>	_listeners;

public:
	Conf							_conf;
	std::vector<Conf>				_confs;

	/// * Constructors & Destructors ___________________________________________
public:
	// ? default (1)
	Server ();

	// ? Constructor (2) taking a config
	Server (const Conf &c);

	// ? Constructor (3) by copy
	Server (const Server &other);

	~Server ();

	// * Member functions ______________________________________________________
	
	Server	&operator= (const Server &rhs);

	// ? init the server with a conf_object (for now just a port number).
	// ? this will allow us to reload the server or set it up after using the default constructor.
	int		init (const Conf & c);

	// ? Server's main loop
	int		start ();

private:
	bool	socket_bind ();

	void	set_client_to_pollout (int i);

	bool	server_poll_loop ();

	bool	is_client_fd (const int i) const;

	bool	add_new_client ();

	void	add_cgi_listener (const int i);

	int		pipe_to_client (int fd);

	bool	record_client_input (const int &i);

	bool	check_timed_out_client (const int i);

	int		remove_client (int i);

	void	squeeze_fds_array ();

	// * debug

	void	aff_fds() const;

	void	aff_clients() const;

}; // * ________________________________________________________________________

