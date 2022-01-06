/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 00:45:14 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <vector>
// # include <stdint.h>		// INTMAX_MAX (because cstdint is c++11)
# include "webserv.hpp"

# define BACK_LOG		123
// # define BUFFER_SIZE	INTMAX_MAX
# define BUFFER_SIZE	64
# define MAX_FDS		234

// ? a client linked to a Server
class Client // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________

	int							_id;		// it's index in Server's _fds array
	std::vector<std::string>	_i_msg;		// the input message buffer_size by buffer_size

	/// * Constructors & Destructors ___________________________________________

	Client();
	
	~Client();

}; // * ________________________________________________________________________


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

	void	start ();

}; // * ________________________________________________________________________

#endif