#include <iostream>
#include "webserv.hpp"

int main(int argc, char **argv)
{
	int	err;

	// TODO (1) parse the config file (default or provided) and feed a list of object with the configurations

	// TODO (2) loop over the config_object list and create that many Server objects.
	Server s(12345);
	
	// TODO (3) it could be great to start each Server on a thread
	err = s.start();

    std::cout << "done " << RED << err <<RST << std::endl;
    return (err);
}