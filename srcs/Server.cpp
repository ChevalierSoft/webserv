/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 06:25:14 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 01:30:33 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
# include <unistd.h>	// debug : usleep
#include "Server.hpp"
#include "ft_print_memory.h"
#include "color.h"

Server::Server () {}

// TODO split initialisation from contructor
Server::Server (int p) : _port(p)
{
	int					rc;
	int					on = 1;
	struct sockaddr_in6	addr;

	// AF_INET6 stream socket to receive incoming connections
	_listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
	if (_listen_sd < 0)
	{
		perror("socket() failed");
		throw (-1);
	}

	/*************************************************************/
	/* Allow socket descriptor to be reuseable                   */
	/*************************************************************/
	rc = setsockopt(_listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                  (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(_listen_sd);
		throw (-2);
	}
	/*************************************************************/
	/* Set socket to be nonblocking. All of the sockets for      */
	/* the incoming connections will also be nonblocking since   */
	/* they will inherit that state from the listening socket.   */
	/*************************************************************/
	rc = ioctl(_listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(_listen_sd);
		throw (-3);
	}
	/*************************************************************/
	/* Bind the socket                                           */
	/*************************************************************/
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family      = AF_INET6;
	// ? in6addr_any is a like 0.0.0.0 and gets any address for binding
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port        = htons(_port);
	rc = bind(_listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(_listen_sd);
		throw (-4);
	}

	// init fd number
	_nb_fds = 0;

	// init the entier array
	memset(_fds, 0 , sizeof(_fds));
	
	std::cout << "ready to listen on port " << _port << std::endl;
}

Server::~Server ()
{
	// close every fd / sd ?
}

Server::Server (const Server & other)
{
	*this = other;
}

Server&		Server::operator= (const Server &rhs)
{
	_port = rhs._port;
	dup2(rhs._listen_sd, _listen_sd);
	// _fds = rhs._fds;				// ! need a deep copy
	return (*this);
}

// ? debug
std::ostream&	operator<<(std::ostream &o, struct pollfd &pfd)
{
	o << "pfd.fd : "		<< pfd.fd		<< std::endl;
	o << "pfd.events : "	<< pfd.events	<< std::endl;
	o << "pfd.revents : "	<< pfd.revents	<< std::endl;
	return (o);
}

// ? This function is the main loop of the server.
void		Server::start ()
{
	bool				end_server = false;
	bool				compress_array;
	bool				close_conn;
	int					new_sd;
	int					rc;
	char				buffer[BUFFER_SIZE];		// this will feed the client's _i_msg
	int					timeout = 3 * 60 * 1000;	// will be 0 after debug

	/*************************************************************/
	/* Set the listen back log (how many events at the same time)*/
	/*************************************************************/
	rc = listen(_listen_sd, BACK_LOG);
	if (rc < 0)
	{
		perror("listen() failed");
		close(_listen_sd);
		throw(-5);
	}

	/*************************************************************/
	/* Set up the initial listening socket                       */
	/*************************************************************/
	_fds[0].fd = _listen_sd;
	_fds[0].events = POLLIN;
	_nb_fds = 1;

	// std::cerr << _fds[0] << std::endl;

	do
	{
		std::cout << "Waiting on poll()...\n";
		rc = poll(_fds, _nb_fds, timeout);

		/***********************************************************/
		/* Check to see if the poll call failed.                   */
		/***********************************************************/
		if (rc < 0)
		{
			perror("  poll() failed");
			close(_listen_sd);
			break;
		}

		/***********************************************************/
		/* Check to see if the 3 minute time out expired.          */
		/***********************************************************/
		if (rc == 0)
		{
			std::cout << "  poll() timed out.  End program.\n";
			break;
		}

		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		int	current_size = _nb_fds;
		for (int i = 0; i < current_size; i++)
		{
		/*********************************************************/
		/* Loop through to find the descriptors that returned    */
		/* POLLIN and determine whether it's the listening       */
		/* or the active connection.                             */
		/*********************************************************/
		if (_fds[i].revents == 0)
			continue;

		/*********************************************************/
		/* If revents is not POLLIN, it's an unexpected result,  */
		/* log and end the server.                               */
		/*********************************************************/
		if (_fds[i].revents != POLLIN)
		{
			std::cout << "  Error! revents = " << _fds[i].revents << std::endl;
			end_server = true;
			break;
		}
		if (_fds[i].fd == _listen_sd)
		{
			/*******************************************************/
			/* Listening descriptor is readable.                   */
			/*******************************************************/
			std::cout << "  Listening socket is readable\n";

			/*******************************************************/
			/* Accept all incoming connections that are            */
			/* queued up on the listening socket before we         */
			/* loop back and call poll again.                      */
			/*******************************************************/
			do
			{
			/*****************************************************/
			/* Accept each incoming connection. If               */
			/* accept fails with EWOULDBLOCK, then we            */
			/* have accepted all of them. Any other              */
			/* failure on accept will cause us to end the        */
			/* server.                                           */
			/*****************************************************/
			new_sd = accept(_listen_sd, NULL, NULL);
			if (new_sd < 0)
			{
				if (errno != EWOULDBLOCK)
				{
				perror("  accept() failed");
				end_server = true;
				}
				break;
			}

			/*****************************************************/
			/* Add the new incoming connection to the            */
			/* pollfd structure                                  */
			/*****************************************************/
			std::cout << "  New incoming connection fd = " << new_sd << std::endl;
			_fds[_nb_fds].fd = new_sd;
			_fds[_nb_fds].events = POLLIN;
			_nb_fds++;

			/*****************************************************/
			/* Loop back up and accept another incoming          */
			/* connection                                        */
			/*****************************************************/
			} while (new_sd != -1);
		}

		/*********************************************************/
		/* This is not the listening socket, therefore an        */
		/* existing connection must be readable                  */
		/*********************************************************/

		else
		{
			std::cout << "  Descriptor " << CYN << _fds[i].fd << RST << " is readable\n";
			close_conn = false;
			/*******************************************************/
			/* Receive all incoming data on this socket            */
			/* before we loop back and call poll again.            */
			/*******************************************************/

			do
			{
				/*****************************************************/
				/* Receive data on this connection until the         */
				/* recv fails with EWOULDBLOCK. If any other         */
				/* failure occurs, we will close the                 */
				/* connection.                                       */
				/*****************************************************/
				rc = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
				std::cout << buffer << std::endl;
				if (rc < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						perror("  recv() failed");
						close_conn = true;
					}
					break;
				}

				/*****************************************************/
				/* Check to see if the connection has been           */
				/* closed by the client                              */
				/*****************************************************/
				if (rc == 0)
				{
					std::cout << "  Connection closed\n";
					close_conn = true;
					break;
				}

				/*****************************************************/
				/* Data was received                                 */
				/*****************************************************/
				std::cout << "  " << rc << " bytes received" << std::endl;

				/*****************************************************/
				/* Echo the data back to the client                  */
				/*****************************************************/
				
				std::string msg = "HTTP/1.1 200 OK\nDate: Tue, 24 Aug 2021 06:20:56 WEST\nServer: webser:42 (popOS)\nLast-Modified: Wed, 24 Aug 2021 06:20:56 WEST\nContent-Length: 128\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<body>\n<h1>peepowidehappy</h1>\n</body>\n<img src='https://cdn.frankerfacez.com/emoticon/359928/2'/>\n</html>";
				rc = send(_fds[i].fd, msg.c_str(), msg.size(), 0);

				if (rc < 0)
				{
					perror("  send() failed");
					close_conn = true;
					break;
				}

			} while (true);

			/*******************************************************/
			/* If the close_conn flag was turned on, we need       */
			/* to clean up this active connection. This            */
			/* clean up process includes removing the              */
			/* descriptor.                                         */
			/*******************************************************/
			if (close_conn)
			{
				close(_fds[i].fd);
				_fds[i].fd = -1;
				compress_array = true;
			}


		}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */

		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		// ? it's where we will be removing clients and move the others
		if (compress_array)
		{
			compress_array = false;
			for (int i = 0; i < _nb_fds; i++)
			{
				if (_fds[i].fd == -1)
				{
					for(int j = i; j < _nb_fds; j++)
					{
						_fds[j].fd = _fds[j + 1].fd;
					}
					i--;
					_nb_fds--;
				}
			}
		}

	} while (end_server == false);
}
