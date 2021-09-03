/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:39 by dait-atm          #+#    #+#             */
/*   Updated: 2021/09/03 06:13:12 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <list>
# include "webserv.hpp"

class Server
{
private:
    int _port;
    int _listen_sd;
    int _new_sd;
    
    // list of sockets beggining with the listening socket
    std::list<struct pollfd> _fds;
    // struct pollfd _fds[200];

    Server();

public:
    Server(int p);
    ~Server();


};

#endif