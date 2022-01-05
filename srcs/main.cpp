#include <iostream>
#include "webserv.hpp"

int main(int argc, char **argv)
{
	Server s(12345);
	s.start();
    std::cout << "oui" << std::endl;
    return (0);
}