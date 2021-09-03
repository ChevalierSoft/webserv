/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 06:25:14 by dait-atm          #+#    #+#             */
/*   Updated: 2021/09/03 11:33:08 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <list>
#include "Server.hpp"
#include "ft_print_memory.h"

Server::Server() {}

Server::Server(int p) : _port(p)
{
	int					rc;
	int					on = 1;
	struct sockaddr_in6	addr;

	// AF_INET6 stream socket to receive incoming connections on
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
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port        = htons(_port);
	rc = bind(_listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(_listen_sd);
		throw (-4);
	}

	ft_print_memory(&addr.sin6_addr.__in6_u, 16);
	std::cout << std::endl;
	std::cout << "ready for listening on port " << _port << std::endl;
}

Server::~Server()
{
	_fds.clear();
}

Server::Server(const Server & other)
{
	*this = other;
}

Server &    Server::operator= (const Server &rhs)
{
	_port = rhs._port;
	_listen_sd = rhs._listen_sd;
	_new_sd = rhs._new_sd;
	_fds = rhs._fds;
	return *this;
}

void	Server::start()
{

}