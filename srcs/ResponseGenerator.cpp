/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/27 16:07:07 by dait-atm         ###   ########.fr       */
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
 * @brief _ss_content_types and _ss_error_messages will be accessible
 *        (read only) by every ::ResponseGenerator objects
 */
const std::map<std::string, std::string>
	ResponseGenerator::_ss_content_types = set_content_types();

const std::map<int, std::string>
	ResponseGenerator::_ss_error_messages = set_error_map();

ResponseGenerator::ResponseGenerator (void) {}

ResponseGenerator::~ResponseGenerator (void) {}

ResponseGenerator::ResponseGenerator (const ResponseGenerator & copy)
{
	*this = copy;
	return ;
}

ResponseGenerator&	ResponseGenerator::operator= (const ResponseGenerator& copy)
{
	if (this != &copy)
	{
		_conf = copy._conf;
	}
	return (*this);
}

void				ResponseGenerator::set_conf (const Conf * c)
{
	_conf = c;
}

/**
 * @brief Set the content-type value of a returned content.
 * 
 * @param extention Extention of the file that will be sent.
 * @return std::string The right content-type.
 */
std::string			ResponseGenerator::set_file_content_type (const std::string & extention) const
{
	std::string											s_content_type;
	std::map<std::string, std::string>::const_iterator	cit;
	
	s_content_type = "content-type: ";
	cit = _ss_content_types.find(extention);

	if (cit == _ss_content_types.end())
		s_content_type += "application/octet-stream";
	else
		s_content_type += cit->second;

	s_content_type += "\r\n";
	return (s_content_type);
}

std::string			ResponseGenerator::set_header (int err, std::string ext, size_t size) const
{
	std::string		s_header;

	s_header = "HTTP/1.1 " + ft_to_string(err) + " " + _ss_error_messages.find(err)->second + "\r\n";
	s_header += "webser: 42\r\n";								// TODO : set a cool header
	s_header += this->set_file_content_type( ext );
	s_header += "Content-Length: ";
	s_header += ft_to_string(size);
	s_header += "\r\n\r\n";

	return (s_header);
}

std::string			ResponseGenerator::generic_error (int err) const
{
	std::string		s_file_content = "";
	std::string		s_full_content;

	s_file_content = ft_to_string(err) + " " + _ss_error_messages.find(err)->second + "\r\n";
	s_full_content = set_header(err, ".html", s_file_content.size()) + s_file_content;

	return (s_full_content);
}

std::string			ResponseGenerator::get_error_file(Conf::code_type err) const
{
	std::string							s_file_content = "";
	std::string							s_full_content;
	std::ifstream						i_file;
	std::string							tmp;
	Conf::error_list::const_iterator	it = _conf->_error_pages.find(err);

	// TODO : if the Accept is not html, just return a header with the correct error code

	if (it == _conf->_error_pages.end())
		return (generic_error(err));
	else
	{
		std::cerr << CYN << it->second << RST << std::endl;
		i_file.open(it->second.c_str());

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
			return (generic_error(err));
		}
	}

	s_full_content = set_header(err, ".html", s_file_content.size());
	s_full_content += s_file_content;
	
	return (s_full_content);
}

/**
 * @brief get the requested file and fill a string with it.
 * 
 * @param root the site's root
 * @param path the requested file
 * @return std::string file content as string
 */
std::string			ResponseGenerator::get_file_content(const std::string &root, const std::string &path) const
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
		// std::cout << "Couldn't open file\n";
		return (this->get_error_file(403));	// 403 ?
	}

	s_full_content = set_header(0, get_file_extention(get_file_name(path)), s_file_content.size());

	s_full_content += s_file_content;

	return (s_full_content);
}

void				ResponseGenerator::set_cgi_env (Client & client, std::vector<std::string> s_envs, std::vector<char *> a_envs) const
{
	// TODO : add the rest
	s_envs.push_back("PWD=" + std::string("./"));
	s_envs.push_back("REQUEST_SCHEME=http");
	s_envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
	s_envs.push_back("SERVER_ADDR=" + this->_conf->_host);
	s_envs.push_back("SERVER_PORT=" + ft_to_string(this->_conf->_port));
	s_envs.push_back("QUERY_STRING=");	// ? add GET arguments here
	s_envs.push_back("REQUEST_METHOD=" + client._request._method);

	int i = 0;
	for (std::vector<std::string>::const_iterator cit = s_envs.begin();
			cit != s_envs.end() ; ++cit)
		a_envs.push_back(&s_envs[i++][0]);
	
	a_envs.push_back(NULL);

	return ;
}

#include <sys/wait.h>			// waitpid
#include <sys/types.h>			// waitpid
#include <ext/stdio_filebuf.h>	// stdio_filebuf
#include <fcntl.h>				// fcntl

std::string			ResponseGenerator::open_cgi (Client & client, std::string url) const
{
	int							err;
	int							cgi_pipe[2];
	pid_t						child;
	char						buff[CGI_BUFF_SIZE];
	char						*exe[2];
	std::string					response = "";
	std::vector<std::string>	s_envs;
	std::vector<char *>			a_envs;
	
	if (pipe(cgi_pipe))
		return (get_error_file(500));

	// set non blocking pipes
	if (fcntl(cgi_pipe[0], F_SETFL, O_NONBLOCK) < 0)
		return (get_error_file(500));

	child = fork();
	if (child < 0)
	{
		close(cgi_pipe[0]);
		close(cgi_pipe[1]);
		return (get_error_file(500));
	}
	else if (!child)
	{
		set_cgi_env(client, s_envs, a_envs);
		exe[0] = &url[0];
		exe[1] = NULL;
		dup2(cgi_pipe[0], 0);
		dup2(cgi_pipe[1], 1);
		execve(exe[0], exe, a_envs.data());
		// TODO : clean memory. maybe by an ugly exception
		std::cerr << CYN << "execve_failed" << std::endl;
	}

	// std::cerr << "sending body" << std::endl;
	// err = write(cgi_pipe[1], "", 0);	// TODO : send request's body in case of POST
	// if (err < 0)
	// 	return (get_error_file(500));
	// std::cerr << "body sent" << std::endl;


	// __gnu_cxx::stdio_filebuf<char>	filebuf(cgi_pipe[0], std::ios::in);
	// std::istream is(&filebuf);
	// std::string	line;

	// ! need to use WNOHANG and check every loop (when it will be implemented)
	waitpid(-1, &child, 0);

	// ! It might be great to avoid this loop
	while (1)
	{
		// is.peek() != EOF

		memset(buff, 0, CGI_BUFF_SIZE);
		err = read(cgi_pipe[0], buff, CGI_BUFF_SIZE - 1);
		// if (err < 0)
		// 	return (get_error_file(500));
		response += buff;
		std::cerr << ">>>>[" << response << "]<<<<" << std::endl;
		
		if (err <= 0)
		{
			std::cerr << err << std::endl;
			break ;
		}
		// std::cerr << "read = " << err << std::endl;
		// buff[err] = '\0';
		// response += buff;
		// if (err < CGI_BUFF_SIZE)
		// 	break ;

		// getline(is, line);
		// response += line + "\n";
	}

	return (response);
}

/**
 * @brief generate a response following GET method specificationns.
 * 
 * @details https://greenbytes.de/tech/webdav/draft-ietf-httpbis-p2-semantics-26.html#GET
 * 
 * @return std::string a string containing the response to the client.
 */
std::string			ResponseGenerator::perform_GET_method(const Client& client) const
{
	struct stat s;
	std::string	root = ".";		// TODO : use the client->_conf one

	if ( ! stat(("." + client._request._path).c_str(), &s))
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			// TODO : check if default_file is present in this dir.

			// TODO : check if directory indexation in on.
			
			return (directory_listing(root, client._request._path));
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			return (get_file_content(root, client._request._path));
		}
		else
		{
			// ? error: it's there, but it's not a directory or a file.
			// ? not sure if symlinks must work.
			get_error_file(418);
		}
	}
	else
	{
		// ? error: wrong path || path too long || out of memory || bad address || ...
		return (get_error_file(404));
	}
	return (get_error_file(404));
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

	// ? __testing cgi __
	std::cout << *_conf->_cgi.begin() << std::endl;
	client._response_ready = true;
	client._response.append_buffer(this->open_cgi(client, *_conf->_cgi.begin()));
	return (false);
	// ? ________________

	// Check asked path (route/location) and set a variable with the real location on this hard drive.

	// std::string	actual_path(getcwd(NULL, 0));
	// if (actual_path.empty())
	// 	return (true);
	// actual_path.append(client._request._path);

	// int	rc = access(actual_path.c_str(), (client._request._method == "GET" ? R_OK : W_OK) | F_OK);
	// if (rc < 0) {
	// 	perror("	access to route failed");
	// 	client._request.clear();
	// 	return (true);
	// }

	// ? check which method should be called
	if (client._request._method == "GET")
		client._response.append_buffer(this->perform_GET_method(client));
	else
		std::cerr << CYN << "(client._request._method != \"GET\")" << std::endl;

	client._response_ready = true;

	return (false);
}
