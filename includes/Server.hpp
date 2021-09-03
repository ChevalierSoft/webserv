/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2021/09/03 11:34:29 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <list>
# include <stdint.h>		// INTMAX_MAX (because cstdint is c++11)
# include "webserv.hpp"

# define BACK_LOG		123
# define BUFFER_SIZE	INTMAX_MAX

class Server
{
private:
	int							_port;
	int							_listen_sd;
	int							_new_sd;
	
	// list of sockets beggining with the listening socket
	std::list<struct pollfd *>	_fds;
	int							_nb_fds;

	Server();

public:
	Server(int p);
	~Server();
	Server(const Server & other);
	Server	&operator= (const Server &rhs);

	void	start();
};


#endif