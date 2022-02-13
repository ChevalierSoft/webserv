/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljurdant <ljurdant@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 15:10:50 by ljurdant          #+#    #+#             */
/*   Updated: 2022/02/13 15:10:11 by ljurdant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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

void    mutex_print(std::ostream &o, std::string const s) {
    pthread_mutex_lock(&mutex);
    o << s << std::endl;
    pthread_mutex_unlock(&mutex);
}