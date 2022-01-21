/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 17:41:41 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>	// stat
#include <sys/stat.h>	// stat
#include <unistd.h>		// stat
#include <fstream>		// ifstream
#include "ResponseGenerator.hpp"
#include "webserv.hpp"
#include "ft_to_string.hpp"
#include "utils.hpp"

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

std::string			ResponseGenerator::set_file_content_type(const std::string & extention)
{
	std::string	s_content_type = "content-type: ";

	std::cout << "extention found : " << extention << std::endl;

	// TODO : set a hash table at creation with every extentions, it will be cleaner that doing 1000 else if()

	if (extention == ".html")
		s_content_type += "text/html";
	else if (extention == ".css")
		s_content_type += "text/css";
	else
		s_content_type += "application/octet-stream";

	s_content_type += "\r\n";
	return (s_content_type);
}

/**
 * @brief get the requested file and fill a string with it.
 * 
 * @param root the site's root
 * @param path the requested file
 * @return std::string file content as string
 */
std::string			ResponseGenerator::get_file_content(const std::string &root, const std::string &path)
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

	s_full_content = "HTTP/1.1 200 OK\r\n";
	s_full_content += "webser: 42\r\n";

	// TODO :  add the appropriate header depending on the request (or the file extention ?)
	
	s_full_content += this->set_file_content_type(get_flie_extention(get_file_name(path)));

	s_full_content += "Content-Length: ";
	s_full_content += ft_to_string(s_file_content.size());
	s_full_content += "\r\n\r\n";

	s_full_content += s_file_content;

	return (s_full_content);
}

/**
 * @brief generate a response following GET method specificationns.
 * 
 * @details https://greenbytes.de/tech/webdav/draft-ietf-httpbis-p2-semantics-26.html#GET
 * 
 * @return std::string a string containing the response to the client.
 */
std::string			ResponseGenerator::perform_GET_methode(const Request& rq)
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
	return ("HTTP/1.1 404 Not Found\r\n\r\n");
}

/**
 * @brief generate a string with the content requested in rq
 * 
 * @param rq the client's _request
 * @return std::string a string containing the response to the client.
 */
std::string			ResponseGenerator::generate(const Request& rq)
{
	// check which method should be called
	if (rq._method == "GET")
		return (this->perform_GET_methode(rq));
	else
		std::cerr << CYN << "(rq._method != \"GET\")" << std::endl;

	return ("");
}