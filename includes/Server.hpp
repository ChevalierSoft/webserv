/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 01:27:14 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

#include <list>
# include <sys/ioctl.h>
# include <sys/socket.h>	// listen
# include <sys/poll.h>		// poll_wait...
# include <errno.h>			// errno
# include <string.h>		// strlen
# include <stdio.h>			// perror
# include <netinet/in.h>	// htons, sockaddr_in6

# define BACK_LOG		123
# define BUFFER_SIZE	64
# define MAX_FDS		2		// ? must be > 1 for the listening socket

// ? a http/1.1 server working for one config
class Server // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	int							_port;			// soon this will be in an object sent from the conf parsing
	int							_listen_sd;
	struct pollfd				_fds[MAX_FDS];	// list of sockets beggining with the listening socket
	int							_nb_fds;

	/// * Constructors & Destructors ___________________________________________
private:
	// ? Constructor (1) default construtor that will not be accessible
	Server ();
public:
	// ? Constructor (2) taking a port in argument.
	// ? After the parsing of a conf file an object will be passed to it
	Server (int p);

	// ? Constructor (3) by copy
	Server (const Server & other);

	~Server ();

	// * Member functions ______________________________________________________
	
	Server	&operator= (const Server &rhs);

	// ? init the server with a conf_object (for now just a port number).
	// ? this will allow us to reload the server or set it up after using the default constructor.
	int		init (int port);

	void	start ();

}; // * ________________________________________________________________________

#endif