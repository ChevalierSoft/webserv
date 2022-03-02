/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 15:11:11 by ljurdant          #+#    #+#             */
/*   Updated: 2022/03/02 10:48:18 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void	sig_join(int sig)
{
	(void)sig;
	signal(SIGINT, &sig_join);
	signal(SIGQUIT, &sig_join);
	return ;
}

int main(int argc, char **argv)
{
	int					*status;
	int					err = 0;
	std::string			conf_file("resources/default.conf");
	
	if (argc == 2)
		conf_file = argv[1];
	else if (argc > 2)
	{
		std::cerr << "Error: too many arguments" << std::endl;
		return (2);
	}
	Parser	p(conf_file);	
	pthread_t	threads[p._hosts.size()];
	if (p._err)
	{
		std::cerr << "Error: " << p._error_message << std::endl;
		return (20);
	}
	else
	{
		// ? p._hosts -> one conf file per host:port
		for (Parser::conf_list::iterator it = p._hosts.begin(); it != p._hosts.end(); it++)
			pthread_create(&threads[it - p._hosts.begin()], NULL, routine, static_cast<void *>(get_confs(*it, p._confs)));
	}
	for (size_t i = 0; i < p._hosts.size(); i++)
	{
		signal(SIGINT, &sig_join);
		signal(SIGQUIT, &sig_join);
		pthread_join(threads[i], reinterpret_cast<void **>(&status));
		if (*status)
		{
			std::cerr << "exit code : " << RED << err <<RST << std::endl;
			err = *status;
		}
		else
			std::cerr << GRN << "success" << RST << std::endl;
		delete status;
	}
    return (err);
}
