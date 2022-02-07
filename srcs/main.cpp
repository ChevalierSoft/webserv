	#include <iostream>
#include <signal.h>			// signal,sigaction
#include "webserv.hpp"
#include <pthread.h>

static bool	run = true;

void	sighandler(int signum)
{
	run = false;
}

void	*routine(void *args) {
	Server	s(*static_cast<Conf *>(args));
	int		*err = static_cast<int*>(malloc(4));
	
	*err = s.start();
	return (err);
}

int main(int argc, char **argv)
{
	int	err = 0;
	std::vector<Server>	servers;
	// TODO mutex for printing
	
	// TODO signal handler that works for threads
	// signal(SIGINT, &sighandler);	// making it easy to close the program
	// signal(SIGQUIT, &sighandler);
	Parser	p("tst/conf/webserv.conf");
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
		// Server	s(*(p._hosts.begin()));
		// TODO Run same host:port with different server names
		// ?? p._hosts -> one conf file per host:port
		for (Parser::conf_list::iterator it = p._hosts.begin(); it != p._hosts.end(); it++)
			pthread_create(&threads[it - p._hosts.begin()], NULL, routine, static_cast<void *>(&(*it)));
		// err = s.start();
	}
	for (int i = 0; i < p._hosts.size(); i++)
		pthread_join(threads[i], NULL);
	std::cout << "exit code : " << RED << err <<RST << std::endl;
    return (err);
}
