/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 13:15:36 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>	// stat
#include <sys/stat.h>	// stat
#include <unistd.h>		// stat
#include <fstream>		// ifstream
#include "ResponseGenerator.hpp"
#include "webserv.hpp"
#include "ft_to_string.hpp"

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

/**
 * @brief get the requested file and fill a string with it.
 * 
 * @param root the site's root
 * @param path the requested file
 * @return std::string file content as string
 */
std::string			ResponseGenerator::get_file_content(std::string root, std::string path)
{
	std::ifstream	i_file;
	std::string		tmp;
	std::string		s_file_content = "";
	std::string		s_full_content;

	i_file.open((root + path).c_str());

	if (i_file.is_open())
	{
		while (i_file.good())
		{
			std::getline(i_file, tmp);
			s_file_content += (tmp + "\r\n");
		}
	}
	else
	{
		// TODO : send error page
		std::cout << "Couldn't open file\n";
	}

	// ? add the appropriate header depending on the type of the file (need to read the rfc)
	s_full_content = "HTTP/1.1 200 OK\r\n";
	s_full_content += "webser: 42\r\n";
	s_full_content += "Content-Type: text/html\r\n";
	s_full_content += "Content-Length: ";
	s_full_content += ft_to_string(s_file_content.size());
	s_full_content += "\r\n\r\n";

	s_full_content += s_file_content;

	return (s_full_content);
}

/**
 * @brief generate a string with the content requested in rq
 * 
 * @param rq the client's _request
 * @return std::string a string containing a file.
 */
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
			
			return (directory_listing(root, rq._path));
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			return (get_file_content(root, rq._path));
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