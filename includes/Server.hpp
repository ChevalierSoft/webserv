/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 11:00:50 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <list>
# include <map>
# include <vector>
# include <sys/ioctl.h>
# include <sys/socket.h>	// listen
# include <sys/poll.h>		// poll_wait...
# include <errno.h>			// errno
# include <string.h>		// strlen
# include <stdio.h>			// perror
# include <netinet/in.h>	// htons, sockaddr_in6
# include "Client.hpp"
# include "Conf.hpp"

# define BACK_LOG		123
# define BUFFER_SIZE	64

# define TIMEOUT		10 * 1000

/**
 * @brief a http/1.1 server working for one config
 */
class Server // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	int							_listen_sd;
	Conf						_conf;
	std::vector<struct pollfd>	_fds;
	std::map<int, Client>		_clients;


	/// * Constructors & Destructors ___________________________________________
private:
	// ? Constructor (1) default construtor that will not be accessible
	Server ();
public:
	// ? Constructor (2) taking a port in argument.
	// TODO After the parsing of a conf file an object will be passed to it
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

	bool	add_new_client ();

	bool	record_client_input (const int &i);

	void	check_timed_out_client (const int i);

	void	remove_client (int i);	// 'i' beeing the index in the main for loop

	void	squeeze_fds_array ();

	// * debug

	void	aff_fds();

}; // * ________________________________________________________________________
