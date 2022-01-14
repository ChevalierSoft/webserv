/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 06:25:14 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/13 03:50:21 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>			// debug : usleep
#include "Server.hpp"
#include "ft_print_memory.h"
#include "color.h"

#define __DEB(s)	std::cerr << MAG << s << RST << std::endl; 

/**
 * @brief Default Constructor for a new Server:: Server object.
 */
Server::Server () {}

/**
 * @brief Construct a new Server:: Server object and initialise it.
 * 
 * @param p Port on which the socket will listen.
 */
Server::Server (int p) : _port(p)
{
	init(_port);
}

/**
 * @brief Destroy the Server:: Server object.
 */
Server::~Server ()
{
	clients.clear();
	close(_listen_sd);
}

/**
 * @brief Construct a new Server:: Server object.
 * 
 * @param other Another instance of Server object to copy from.
 */
Server::Server (const Server & other)
{
	*this = other;
}

/**
 * @brief Overload of the operator= for Server::.
 * 
 * @param rhs Another instance of Server object to deep copy from.
 * @return Server& A reference to this object.
 */
Server&			Server::operator= (const Server &rhs)
{
	_port = rhs._port;
	dup2(rhs._listen_sd, _listen_sd);
	// _fds = rhs._fds;				// ! need a deep copy
	return (*this);
}

/**
 * @brief Bind the listening socket
 * 
 * @return true sucess.
 * @return false bind() failed.
 */
bool			Server::socket_bind ()
{
	struct sockaddr_in6	addr;
	int					rc;

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
		return (false);
	}
	return (true);
}

/**
 * @brief Initialise the server.
 * 
 * @param p Port on which the socket will listen.
 * @return int On success returns 0.
 *         A positive number is returned in case of error.
 */
int				Server::init (int p)
{
	int					rc;
	int					on = 1;

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
	// ? Set _listen_sd (and incoming accepted sockets from it) to be nonblocking
	rc = ioctl(_listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(_listen_sd);
		return (3);
	}

	// ? Bind the socket
	if (this->socket_bind() == false)
		return (4);

	// ? init fd number
	this->_nb_fds = 0;

	// ? init the entier array
	memset(_fds, 0 , sizeof(_fds));
	
	std::cout << "ready to listen on port " << _port << std::endl;
	return (0);
}

/**
 * @brief Add a new client 
 * 
 * @return true On success.
 * @return false client's connection can not be handled.
 */
bool			Server::add_new_client ()
{
	int	new_sd;

	std::cout << "  Listening socket is readable\n";

	new_sd = accept(_listen_sd, NULL, NULL);
	if (new_sd < 0)
	{
		std::cerr << "error: can't accept client: " << new_sd << std::endl;
		return (false);
	}

	std::cout << YEL << "  New incoming connection fd : " << RED << new_sd << RST << std::endl;
	_fds[_nb_fds].fd = new_sd;
	_fds[_nb_fds].events = POLLIN;
	_nb_fds++;

	clients[new_sd] = Client();

	return (true);
}

/**
 * @brief remove a client of clients and close it's linked fd
 * 
 * @param i Index from server_poll_loop's for loop.
 */
void			Server::remove_client (int i)
{
	close(_fds[i].fd);
	_fds[i].fd = -1;
	clients.erase(_fds[i].fd);
	squeeze_fds_array();
}

/**
 * @brief Record the client's inputs non blockingly.
 * 
 * @param i Index from server_poll_loop's for loop.
 * 
 * @return true connection to the client must be closed because of an error
 *         or we don't need more data to generate the output.
 * @return false the client has to send more data to generate the return message.
 */
bool			Server::record_client_input (const int &i)
{
	char	buffer[BUFFER_SIZE];
	bool	close_conn;
	int		rc;

	std::cout << YEL << "  Descriptor " << RED << _fds[i].fd << YEL << " is readable\n" << RST;

	// ? update client's life_timej
	clients[_fds[i].fd].update();

	rc = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0); // MSG_DONTWAIT | MSG_ERRQUEUE);	// ? errors number can be checked with the flag MSG_ERRQUEUE (man recv)

	// ? connection closed by client or error while recv.
	// ? EAGAIN would tell us if this was the end of the client's message or ...
	// ? if there is an error. because we can not use errno in this project,
	// ? we will handle this while parsing the input in the client's parsing.
	// if (rc <= 0)
	// {
	//	 if (errno == EAGAIN)
	//		 break ;
	// 	std::cout << YEL << "  Connection closed\n" << RST;
	// 	remove_client(i);
	// 	return (true);
	// }

	buffer[rc] = '\0';										// ? closing the char array
	clients[_fds[i].fd].add_input_buffer(buffer, rc + 1);	// ? store the buffer

	// ? debug
	std::cout << YEL << "  " << rc << " bytes received : " << RST << std::endl;
	ft_print_memory(buffer, rc);
	// std::cout << "[" << GRN << buffer << RST << "]" << std::endl;

	if (clients[_fds[i].fd].is_output_ready() == false)
		close_conn = clients[_fds[i].fd].parse_and_generate_response();

	if (clients[_fds[i].fd].is_output_ready() == true)
		close_conn = clients[_fds[i].fd].send_response(_fds[i].fd);

	if (close_conn)
	{
		std::cerr <<MAG<< "close_conn" <<RST<< std::endl;
		remove_client(i);
		return (true);
	}
	return (false);
}

/**
 * @brief squeeze _fds when a fd is -1.
 * 
 * @param _fds array of pollfd
 * @param _nb_fds Size of _fds
 */
void			Server::squeeze_fds_array ()
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

/**
 * @brief Kick out a client if there life_time is too long.
 * 
 * @details Check if a specific client not sending event since too long,
 *          then decide if it should be timed out.
 * 
 * @param i Index from server_poll_loop's for loop.
 */
void			Server::check_timed_out_client (const int i)
{
	if (i < 0 || i == 0)
		return ;
	if (clients[_fds[i].fd].is_timed_out() == true)
	{
		std::cerr << "kicked fd : " << RED << _fds[i].fd << RST << std::endl;
		remove_client(i);
	}
}

/**
 * @brief This function handles the poll loop
 *        and call functions according to the events.
 * 
 * Loop over _fds each time a envent is registered.
 * Kick timed out connections.
 * Accept incomimg clients.
 * If poll is timed out Kick timed out connections and return;
 * 
 * @return true On success.
 * @return false An error occured while using poll. 
 */
bool			Server::server_poll_loop ()
{
	int					rc;
	bool				need_cleaning = 0;

	std::cout << "Waiting on poll()...\n";
	rc = poll(_fds, _nb_fds, TIMEOUT);

	if (rc < 0)
	{
		perror("  poll() failed.");
		close(_listen_sd);
		return (false) ;
	}

	// ? Check to see if the 3 minute time out expired.
	if (rc == 0)
	{
		std::cout << "  poll() timed out." << std::endl;
		// ? clean _fds of timed out fds, and return true too loop again.
		for (int i = 1; i < _nb_fds; ++i)
			check_timed_out_client(i);
		return (true);
	}

	// ? Loop through to find the descriptors that returned
	// ? POLLIN and determine whether it's the listening
	// ? or the active connection.
	for (int i = 0; i < _nb_fds; i++)
	{
		// ? if there is no event on the socket the loop continues
		if (_fds[i].revents == 0)
		{
			if (i != 0)	// ? not the listening socket
				check_timed_out_client(i);
			continue;
		}
 
		// ? If revents is not POLLIN, it's an unexpected result,
		// ? so it will be cleaned
		if (_fds[i].revents != POLLIN)
		{
			std::cout << "  error: revents = " << _fds[i].revents << std::endl;
			close(_fds[i].fd);
			_fds[i].fd = -1;
			break;
		}

		// ? check if it's a new client
		if (_fds[i].fd == _listen_sd)
		{
			if (_nb_fds < MAX_FDS)
				add_new_client();
		}
		// ? else the event was triggered by a pollfd that is already in _fds
		else
			need_cleaning |= record_client_input(i);

	}

	// ? it's where we will be removing clients and squeeze the array
	if (need_cleaning)
		squeeze_fds_array();

	return (true);
}

// ? This function is the main loop of the server.
/**
 * @brief This function starts the Server
 * 
 * @return int returns 0 on success, else a error code is returned.
 */
int				Server::start ()
{
	int		err = 0;

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

	__DEB(_listen_sd)

	while (server_poll_loop() == true)
		;

	clients.clear();
	// close(_listen_sd);

	return (0);
}

void	Server::aff_fds()
{
	std::cout << "_nb_fds : " << _nb_fds << std::endl;
	for (int i = 0; i < _nb_fds; ++i)
	{
		std::cout << i << " : " << _fds[i].fd << std::endl;
	}
}

// ?fonctionnement de la boucle principale

// ? traiter l'input pendant qu'elle arrive
// ? si c'est \r\n (ou s'il y a un mauvais message)
// ? envoyer un message au client pour fermer la connexion
// ?   peut etre que le message peut etre envoyé par un thread pour éviter que ça prenne trop de temps ?
// ?		( + je n'ai plus à le gerer dans la loop principale | - le bottle neck c'est plutot l'i/o du kernel)
// ? chaque client aura un compteur gettimeofday qui se reset apres que son event ai fait return poll.
// ? si le compteur dépasse 'timeout' on close la connection.
// ? si aucun event à eu lieu apres 'timeout' poll ferme les clients trop long
// ? server_poll_loop return true pour recommencer la boucle.
// ? s'il y a une erreur
