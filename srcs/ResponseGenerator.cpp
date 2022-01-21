/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 12:20:55 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>	// stat
#include <sys/stat.h>	// stat
#include <unistd.h>		// stat
#include "ResponseGenerator.hpp"
#include "webserv.hpp"

ResponseGenerator::ResponseGenerator(void)
{
}

ResponseGenerator::~ResponseGenerator(void)
{
}

ResponseGenerator::ResponseGenerator(const ResponseGenerator & copy)
{
	*this = copy;
	return ;
}

ResponseGenerator&	ResponseGenerator::operator=(const ResponseGenerator& copy)
{
	if (this != &copy)
	{
		// copy
	}
	return (*this);
}

std::string			ResponseGenerator::generate(Request& rq)
{
	struct stat s;
	std::string	root = ".";		// TODO : use the client->_conf one

	if ( ! stat(("." + rq._path).c_str(), &s))
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			// TODO : check if directory indexation in on.

			// TODO : see if we have to redirect to index.html if it exists.
			
			return (directory_listing(root, rq._path).c_str());
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			// TODO : generate a file
			// return (get_file_content(root, rq._path).c_str());
			std::cout << "store '" << root + rq._path << "' through _response.append_buffer()" << std::endl;
		}
		else
		{
			// ? error: it's not a directory or a file.
			// ? not sure if symlinks must work. 
		}
	}
	else
	{
		// ? basically 404
		// ? error: wrong path || path too long || out of memory || bad address || ...
	}
	return ("");
}