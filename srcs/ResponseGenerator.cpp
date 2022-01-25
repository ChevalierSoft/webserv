/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 18:16:58 by dait-atm         ###   ########.fr       */
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
#include "set_content_types.hpp"

/**
 * @brief _ss_content_types will be accessible (read only) by every ::ResponseGenerator objects
 */
const std::map<std::string, std::string>
	ResponseGenerator::_ss_content_types = set_content_types();

ResponseGenerator::ResponseGenerator(void) {}

ResponseGenerator::~ResponseGenerator(void) {}

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
 * @brief Set the content-type value of a returned content.
 * 
 * @param extention Extention of the file that will be sent.
 * @return std::string The right content-type.
 */
std::string			ResponseGenerator::set_file_content_type(const std::string & extention)
{
	std::string											s_content_type;
	std::map<std::string, std::string>::const_iterator	cit;
	
	s_content_type = "content-type: ";
	cit = _ss_content_types.find(extention);

	// ? debug
	// std::cout << "extention found : " << extention << std::endl;

	if (cit == _ss_content_types.end())
		s_content_type += "application/octet-stream";
	else
		s_content_type += cit->second;

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
			s_file_content += (tmp + "\n");
		}
	}
	else
	{
		// TODO : send error page
		std::cout << "Couldn't open file\n";
	}

	s_full_content = "HTTP/1.1 200 OK\r\n";
	s_full_content += "webser: 42\r\n";
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
 * @brief generate the response for the client
 * 
 * @param rq 
 * @return true internal error, need to close the client connexion without sending response
 * @return false all good
 */
bool				ResponseGenerator::generate(Client& client)
{
	client._response.clear();
	
	// TODO : Check asked path (route/location) and set a variable with the real location on this hard drive.

	// ? check which method should be called
	if (client._request._method == "GET")
		client._response.append_buffer(this->perform_GET_methode(client._request));
	else
		std::cerr << CYN << "(client._request._method != \"GET\")" << std::endl;

	client._response_ready = true;

	return (false);
}
