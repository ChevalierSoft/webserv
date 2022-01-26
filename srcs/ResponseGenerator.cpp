/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 19:59:34 by lpellier         ###   ########.fr       */
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
		_conf = copy._conf;
	}
	return (*this);
}

void				ResponseGenerator::set_conf(const Conf * c)
{
	_conf = c;
}

/**
 * @brief Set the content-type value of a returned content.
 * 
 * @param extention Extention of the file that will be sent.
 * @return std::string The right content-type.
 */
std::string			ResponseGenerator::set_file_content_type(const std::string & extention) const
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
std::string			ResponseGenerator::get_file_content(const std::string &path) const
{
	std::ifstream	i_file;
	std::string		tmp;
	std::string		s_file_content = "";
	std::string		s_full_content;

	i_file.open((path).c_str());

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
std::string			ResponseGenerator::perform_GET_methode(const Request& rq) const
{
	struct stat s;

	if ( !(stat((rq._path).c_str(), &s)) )
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			if (rq._route._dir_listing) // check if directory listing is on
			{
				return (directory_listing(rq._path));
			}
			else
				return ("HTTP/1.1 403 Forbidden\r\n\r\n"); // TODO : 403 forbidden 
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			return (get_file_content(rq._path));
		}
		else
		{
			// ? error: it's not a directory or a file.
			// ? not sure if symlinks must work.
		}
	}
	else
	{
		//TODO write function that sends error pages
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

bool				ResponseGenerator::generate(Client& client) const
{
	client._response.clear();
	
	// Check asked path (route/location) and set a variable with the real location on this hard drive.

	Request request(parse_request_route(client._request));
	// ;
	int	rc = access(request._path.c_str(), (client._request._method == "GET" ? R_OK : W_OK) | F_OK);
	if (rc < 0) {
		perror("	access to route failed");
		return (true);
	}

	// ? check which method should be called
	if (client._request._method == "GET")
		client._response.append_buffer(this->perform_GET_methode(request));
	else
		std::cerr << CYN << "(client._request._method != \"GET\")" << std::endl;

	client._response_ready = true;

	return (false);
}

bool		ResponseGenerator::is_directory(const std::string path) const{
	struct stat s;

	if ( lstat(path.c_str(), &s) == 0 )
    	if (S_ISDIR(s.st_mode))
			return (true);
	return (false);
}

Request 	ResponseGenerator::parse_request_route(Request  const &input_request) const{
	const char					sep = '/';
	int							found  = 0;
	Conf::route_list			routes((*_conf)._routes);
	std::string					file = std::string();
	std::string					path;
	Request						output_request;
	while (found <= input_request._path.size())
	{
		if ((found = input_request._path.find(sep, found)) == std::string::npos)
			found = input_request._path.size();
		for (Conf::route_list::iterator it = routes.begin(); it != routes.end(); it++)
		{
			if (it->_path == input_request._path.substr(0,found)+"/")
			{
				output_request._route = *it;
				if (found < input_request._path.size())
					file = input_request._path.substr(found + 1, input_request._path.size() - found);
				else
					file = "";
			}
		}
		found++;
	}
	output_request._path = output_request._route._location+file;
	if (is_directory(output_request._path))
	{
		if (*(output_request._path.end() - 1) != '/')
			output_request._path+="/";
		if (output_request._route._default_file != "") // check if default file is defined
			output_request._path+=output_request._route._default_file; // adding default file to path
	}
	return (output_request);
}