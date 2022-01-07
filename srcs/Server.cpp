/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 06:25:14 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 20:52:06 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>			// debug : usleep
#include "Server.hpp"
#include "ft_print_memory.h"
#include "color.h"

Server::Server () {}

Server::Server (int p) : _port(p)
{
	init(_port);
}

int			Server::socket_bind(struct sockaddr_in6 &addr)
{
	int		rc;
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	// ? in6addr_any is a like 0.0.0.0 and gets any address for binding
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port = htons(_port);
	rc = bind(_listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(_listen_sd);
		return (4);
	}
	return (0);
}

int	Server::init (int p)
{
	int					rc;
	int					on = 1;
	struct sockaddr_in6	addr;

	// ? AF_INET6 stream socket to receive incoming connections
	_listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
	if (_listen_sd < 0)
	{
		perror("socket() failed");
		return (1);
	}

	// ? Allow socket descriptor to be reuseable
	rc = setsockopt(_listen_sd, SOL_SOCKET, SO_REUSEADDR,
					(char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(_listen_sd);
		return (2);
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
		return (3);
	}

	// ? Bind the socket
	socket_bind(addr);

	// ? init fd number
	_nb_fds = 0;

	// ? init the entier array
	memset(_fds, 0 , sizeof(_fds));
	
	std::cout << "ready to listen on port " << _port << std::endl;
	return (0);
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

int		Server::add_new_client()
{
	int		new_sd;

	std::cout << "  Listening socket is readable\n";

	new_sd = accept(_listen_sd, NULL, NULL);
	if (new_sd < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("  accept() failed");
			_end_server = true;
		}
		return (false);
	}

	std::cout << YEL << "  New incoming connection fd : " << RED << new_sd << RST << std::endl;
	_fds[_nb_fds].fd = new_sd;
	_fds[_nb_fds].events = POLLIN;
	_nb_fds++;

	clients[new_sd] = new Client();

	return (true);
}

int		Server::record_client_input(const int &i)
{
	char	buffer[BUFFER_SIZE];
	bool	close_conn;
	int		rc;
	
	std::cout << YEL << "  Descriptor " << RED << _fds[i].fd << YEL << " is readable\n" << RST;

	rc = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

	// ? connection closed by client or error while recv
	if (rc <= 0)
	{
		std::cout << YEL << "  Connection closed\n" << RST;
		_fds[i].fd = -1;
		clients.erase(_fds[i].fd);
		return (true);
	}

	buffer[sizeof(buffer) - 1] = '\0';				// ? closing the array
	clients[_fds[i].fd]->i_msg.push_back(buffer);	// ? store the buffer in the client

	// ? debug
	std::cout << YEL << "  " << rc << " bytes received : " << RST << std::endl;
	// std::cout << "[" << GRN << buffer << RST << "]" << std::endl;
	ft_print_memory(buffer, rc);

	if (clients[_fds[i].fd]->response_generated == false)
		close_conn = clients[_fds[i].fd]->parse_and_generate_response();
	else
		close_conn = clients[_fds[i].fd]->send_response(_fds[i].fd);

	if (close_conn)
	{
		close(_fds[i].fd);
		_fds[i].fd = -1;
		clients.erase(_fds[i].fd);
		return (true);
	}
	return (false);
}

static inline
void	clean_fds(struct pollfd *_fds, int &_nb_fds)
{
	for (int i = 0; i < _nb_fds; i++)	// can be faster
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

int		Server::server_poll_loop()
{
	int					rc;
	bool				need_cleaning;

	std::cout << "Waiting on poll()...\n";
	rc = poll(_fds, _nb_fds, TIMEOUT);
	if (rc < 0)
	{
		perror("  poll() failed");
		close(_listen_sd);
		return (false) ;
	}

	// ? Check to see if the 3 minute time out expired.
	// TODO : clean _fds of timed out fds and get back to the beggining of this function
	if (rc == 0)
	{
		std::cout << "  poll() timed out.  End program.\n";
		// ? we do not close _listen_sd here
		return (false);
	}

	// ? Loop through to find the descriptors that returned
	// ? POLLIN and determine whether it's the listening
	// ? or the active connection.
	for (int i = 0; i < _nb_fds; i++)
	{
		if (_fds[i].revents == 0)
			continue;

		// ? If revents is not POLLIN, it's an unexpected result,
		// ? log and end (or restart ?) the server
		if (_fds[i].revents != POLLIN)
		{
			std::cout << "  Error! revents = " << _fds[i].revents << std::endl;
			_end_server = true;
			break;
		}

		// ? check if it's a new client
		if (_fds[i].fd == _listen_sd)
		{
			if (add_new_client() == false)
				break ;
		}
		// ? else the event was triggered by a pollfd that is already in _fds
		else
		{
			need_cleaning = record_client_input(i);
			// if (need_cleaning)
			// 	break ;
		}

	}

	/***********************************************************/
	/* If the need_cleaning flag was turned on, we need        */
	/* to squeeze together the array and decrement the number  */
	/* of file descriptors. We do not need to move back the    */
	/* events and revents fields because the events will always*/
	/* be POLLIN in this case, and revents is output.          */
	/***********************************************************/
	// ? it's where we will be removing clients
	if (need_cleaning)
		clean_fds(_fds, _nb_fds);

	return (true);
}

// ? This function is the main loop of the server.
int		Server::start ()
{
	int		err;

	// ? Set the listen back log (how many events at the same time)
	err = listen(_listen_sd, BACK_LOG);
	if (err < 0)
	{
		perror("listen() failed");
		close(_listen_sd);
		return (5);
	}

	// ? Set up the initial listening socket
	_fds[0].fd = _listen_sd;
	_fds[0].events = POLLIN;
	_nb_fds = 1;

	while (server_poll_loop() == true)
		;

	close(_listen_sd);

	return (0);
}





// - traiter l'input pendant qu'elle arrive
// - si c'est \r\n (ou s'il y a un mauvais message)
// - envoyer un message au client pour fermer la connexion
//   - peut etre que le message peut etre envoyé par un thread pour éviter que ça prenne trop de temps ?
//		- ( + je n'ai plus à le gerer dans la loop principale | - le bottle neck c'est plutot l'i/o du kernel)
// - chaque client aura un compteur gettimeofday qui se reset apres que son event ai fait return poll.
// - si le compteur dépasse 'timeout' on close la connection.
// - si aucun event à eu lieu apres 'timeout' poll return une erreur.
// - cette erreur va permettre de loop sur _fds pour chercher s'il y a des kick à mettre
