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
	
	s.start();
	return (NULL);
}

int main(int argc, char **argv)
{
	int	err = 0;
	std::vector<Server>	servers;
	pthread_t	*threads;
	
	signal(SIGINT, &sighandler);	// making it easy to close the program
	signal(SIGQUIT, &sighandler);
	
	Parser	p("tst/conf/webserv.conf");
	run = !p._err;
	
	// p.print();
	if (run == false)
	{
		std::cerr << "Error: " << p._error_message << std::endl;
		std::cerr << p.line_number << ": " << p.line << std::endl;
		return (20);
	}
	else
	{
		// TODO : loop over the config_object list and create that many Server objects.

		if (!(threads = static_cast<pthread_t *>(malloc(sizeof(*threads) * p._confs.size()))))
			run = false;
		int i = 0;
		for (Parser::conf_list::iterator it = p._confs.begin(); it != p._confs.end(); it++)
		{
			pthread_create(&threads[i], NULL, routine, static_cast<void *>(&(*it)));
			i++;
		}
		// err = s.start();
	}
	for (int i = 0; i < p._confs.size(); i++)
		pthread_join(threads[i], NULL);
    std::cout << "exit code : " << RED << err <<RST << std::endl;
    return (err);
}
