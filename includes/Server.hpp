/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/20 05:13:33 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

# define BACK_LOG				123
# define REQUEST_BUFFER_SIZE	1024

# define TIMEOUT				10 * 1000

/**
 * @brief a http/1.1 server working for one config
 */
class Server // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	int							_listen_sd;
	std::vector<struct pollfd>	_fds;
	std::map<int, Client>		_clients;
	ResponseGenerator			_response_generator;

	// std::map<int, Client>		_cgi_listeners;

public:
	Conf						_conf;
	std::vector<Conf>			_confs;

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

	bool	server_poll_loop ();

	bool	is_client_fd (const int i) const;

	bool	add_new_client ();

	void	add_cgi_listener (const int i);

	int		pipe_to_client (int fd);

	bool	record_client_input (const int &i);

	void	check_timed_out_client (const int i);

	void	remove_client (int i);

	void	squeeze_fds_array ();

	// * debug

	void	aff_fds();

}; // * ________________________________________________________________________
