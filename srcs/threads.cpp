/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: %F{207}%n%f <%F{207}%n%f@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 16:48:12 by %F{207}%n%f       #+#    #+#             */
/*   Updated: 2022/02/09 16:50:04 by %F{207}%n%f      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static bool	run = true;

void	sighandler(int signum)
{
	run = false;
}

void	*routine(void *args) {
	std::vector<Conf> confs = *(static_cast<std::vector<Conf> *>(args));
	Server	s(confs.front());
	int		*err = new int;
	
    // signal(SIGINT, &sighandler);	// making it easy to close the program
	// signal(SIGQUIT, &sighandler);
	s._confs = confs;
	*err = s.start();
	return (err);
}

std::vector<Conf>   *get_confs(const Conf &current, const std::vector<Conf> &confs) {
    std::vector<Conf>	*output = new std::vector<Conf>;
    
    output->push_back(current);
	
    //loop to make vector of confs with same host:ip
    for (std::vector<Conf>::const_iterator it = confs.begin(); it != confs.end(); it++)
    {
        if (it->_name != current._name)
        {
            for (Conf::host_list::const_iterator it2 = it->_hosts.begin(); it2 != it->_hosts.end(); it2++)
                if (*it2 == current._hosts.front())
                    output->push_back(*it);
        }
    }
    return (output);
}