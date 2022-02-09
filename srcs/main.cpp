/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: %F{207}%n%f <%F{207}%n%f@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 16:44:33 by %F{207}%n%f       #+#    #+#             */
/*   Updated: 2022/02/09 16:44:37 by %F{207}%n%f      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv)
{
	void	*status;
	int		err;
	std::vector<Server>	servers;
	std::string			conf_file("tst/conf/webserv.conf");
	// TODO mutex for printing
	
	// TODO signal handler that works for threads
	// signal(SIGINT, &sighandler);	// making it easy to close the program
	// signal(SIGQUIT, &sighandler);
	if (argc == 2)
		conf_file = argv[1];
	else if (argc > 2)
	{
		std::cerr << "Error: too many arguments" << std::endl;
		return (2);
	}
	Parser	p(conf_file);	
	// p.print();
	pthread_t	threads[p._hosts.size()];
	if (p._err)
	{
		std::cerr << "Error: " << p._error_message << std::endl;
		return (20);
	}
	else
	{
		// ?? p._hosts -> one conf file per host:port
		for (Parser::conf_list::iterator it = p._hosts.begin(); it != p._hosts.end(); it++)
			pthread_create(&threads[it - p._hosts.begin()], NULL, routine, static_cast<void *>(get_confs(*it, p._confs)));
	}
	for (int i = 0; i < p._hosts.size(); i++)
		pthread_join(threads[i], &status);
	err = *static_cast<int *>(status);
	std::cout << "exit code : " << RED << err <<RST << std::endl;
    return (err);
}
