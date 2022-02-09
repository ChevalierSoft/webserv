	#include <iostream>
#include <signal.h>			// signal,sigaction
#include "webserv.hpp"

static bool	run = true;

void	sighandler(int signum)
{
	run = false;
}

int main(int argc, char **argv)
{
	int	err = 0;
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
	run = !p._err;
	
	// p.print();
	pthread_t	threads[p._hosts.size()];
	if (run == false)
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
		pthread_join(threads[i], NULL);
	std::cout << "exit code : " << RED << err <<RST << std::endl;
    return (err);
}
