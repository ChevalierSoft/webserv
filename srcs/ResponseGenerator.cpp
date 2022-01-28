/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/28 04:28:24 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>			// stat
#include <sys/stat.h>			// stat
#include <unistd.h>				// stat
#include <fstream>				// ifstream
#include <unistd.h>				// execve
#include <sys/wait.h>			// waitpid
#include <sys/types.h>			// waitpid
#include <fcntl.h>				// fcntl
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
	s_header += this->set_file_content_type(ext);
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

std::string			ResponseGenerator::get_error_file (Conf::code_type err) const
{
	std::string							s_file_content = "";
	std::string							s_full_content;
	std::ifstream						i_file;
	std::string							tmp;
	Conf::error_list::const_iterator	it = _conf->_error_pages.find(err);

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
		// std::cout << "Couldn't open file\n";
		return (this->get_error_file(403));	// 403 ?
	}

	s_full_content = set_header(0, get_file_extention(get_file_name(path)), s_file_content.size());

	s_full_content += s_file_content;

	return (s_full_content);
}

void				ResponseGenerator::set_cgi_env (Client & client, std::vector<std::string> & s_envs, std::vector<char *> & a_envs) const
{
	// TODO : add the rest + add env passed to main()
	s_envs.push_back("SERVER_NAME=" + _conf->_name);
	// s_envs.push_back("SCRIPT_NAME=" + );
	// s_envs.push_back("SCRIPT_FILE_NAME=" + );
	// s_envs.push_back("DOCUMENT_ROOT" + );	 // TODO : add location or route here
	s_envs.push_back("PWD=" + std::string("./"));
	s_envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
	s_envs.push_back("REMOTE_ADDR=" + client._ip);
	s_envs.push_back("REMOTE_PORT=" + client._port);
	s_envs.push_back("SERVER_ADDR=" + this->_conf->_hosts.begin()->first);
	s_envs.push_back("SERVER_PORT=" + ft_to_string(this->_conf->_hosts.begin()->second));
	// s_envs.push_back("QUERY_STRING=");		// TODO add GET arguments here
	s_envs.push_back("REQUEST_SCHEME=http");
	s_envs.push_back("REQUEST_METHOD=" + client._request._method);
	s_envs.push_back("SERVER_ADMIN=dait-atm or lpellier or ljurdan @student.42.fr"); // not sure about this one
	s_envs.push_back("SERVER_SIGNATURE=Webserv 42");
	// s_envs.push_back("CONTEXT_PREFIX=/cgi-bin/");
	s_envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
	s_envs.push_back("SHLVL=1");
	s_envs.push_back("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");	// #sharingan
	s_envs.push_back("REQUEST_SCHEME=http");
	// CONTEXT_DOCUMENT_ROOT= // ? add cgi complete link

	// TODO : add request's headers

	int i = 0;
	for (std::vector<std::string>::const_iterator cit = s_envs.begin();
			cit != s_envs.end() ; ++cit)
		a_envs.push_back(&s_envs[i++][0]);
	
	a_envs.push_back(NULL);

	return ;
}

void				ResponseGenerator::start_cgi (Client & client, std::string url, int cgi_pipe[2]) const
{
	char						*exe[2];
	std::vector<std::string>	s_envs;
	std::vector<char *>			a_envs;

	set_cgi_env(client, s_envs, a_envs);
	exe[0] = &url[0];
	exe[1] = NULL;
	dup2(cgi_pipe[0], 0);
	dup2(cgi_pipe[1], 1);
	execve(exe[0], exe, a_envs.data());
	// TODO : clean memory. maybe by an ugly exception
	std::cerr << CYN << "execve_failed" << std::endl;
	exit(66);
}

std::string			ResponseGenerator::listen_cgi (Client & client,
													std::string url,
													int cgi_pipe[2],
													pid_t child ) const
{
	int							err;
	std::string					response;
	std::string					page;
	char						buff[CGI_BUFF_SIZE];

	// ! need to use WNOHANG and check every loop (when it will be implemented)
	// ? https://cboard.cprogramming.com/c-programming/138057-waitpid-non-blocking-fork.html
	waitpid(-1, &child, 0);

	// ! avoid this loop by entering/leaving this function until child is exited
	while (1)
	{
		memset(buff, 0, CGI_BUFF_SIZE);
		err = read(cgi_pipe[0], buff, CGI_BUFF_SIZE - 1);

		if (err <= 0)
			break ;

		response += buff;
		// std::cerr << ">>>>[" << response << "]<<<<" << std::endl;
	}

	page = "HTTP/1.1 200 OK\r\n";
	page += "Server: Webserv 42\r\n";	// TODO : set a cool header
	page += "Content-Length: ";
	page += ft_to_string(response.length()) + "\r\n";
	page += response;

	return (page);
}

bool				ResponseGenerator::cgi_send_body (Client & client, int cgi_pipe[2]) const
{
	int	err;

	if (client._request._method != "POST")
	{
		client._body_sent = true;
		return (false);
	}

	std::cerr << "sending body" << std::endl;

	// TODO : when non blocking will be a thing, iterate piece by piece
	for (std::vector<std::string>::const_iterator cit = client._request.begin_body();
		cit != client._request.end_body(); ++cit)
	{
		err = write(cgi_pipe[1], cit->c_str(), cit->length());
		if (err < 0)
			return (true);
	}
	
	// TODO : if (cit == client._request.end_body())
	std::cerr << "body sent" << std::endl;
	client._body_sent = true;

	return (false);
}

std::string			ResponseGenerator::cgi_handling (Client & client, std::string url) const
{
	int				cgi_pipe[2];
	pid_t			child;
	std::string		response;
	
	if (pipe(cgi_pipe))
		return (get_error_file(500));

	// ? set non blocking the read part of the pipe
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
		this->start_cgi(client, url, cgi_pipe);

	if (client._body_sent == false)
	{
		if (cgi_send_body(client, cgi_pipe))
			return (get_error_file(500));
	}

	response = listen_cgi(client, url, cgi_pipe, child);

	return (response);
}

std::string		ResponseGenerator::get_redirection(const Route::redir_type & redir) const {
	return ("HTTP/1.1 " + ft_to_string(redir.first) + " " + _ss_error_messages.at(redir.first) + "\r\nLocation: " + redir.second + "\r\n\r\n");
}

/**
 * @brief generate a response following GET method specificationns.
 * 
 * @details https://greenbytes.de/tech/webdav/draft-ietf-httpbis-p2-semantics-26.html#GET
 * 
 * @return std::string a string containing the response to the client.
 */
std::string			ResponseGenerator::perform_GET_method(const Request & rq) const
{
	struct stat s;

	// ? redirects if there is a redirection in appropriate route AND if what is typed in the url corresponds to location in conf
	if (rq._redir != Route::redir_type())
		return (get_redirection(rq._redir));

	if ( !(stat((rq._path).c_str(), &s)) )
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			if (rq._route._dir_listing) // check if directory listing is on
				return (directory_listing(rq._path));
			else
				return (get_error_file(403));
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			return (get_file_content(rq._path));
		}
		else
		{
			// ? error: it's there, but it's not a directory or a file.
			// ? not sure if symlinks must work.
			return (get_error_file(418));
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
	// ! clear at the creation of the client. here it will erase the response each loop 
	client._response.clear();

	// ? __testing cgi __
	std::cout << *_conf->_cgi.begin() << std::endl;
	client._response_ready = true;
	client._response.append_buffer(this->cgi_handling(client, *_conf->_cgi.begin()));
	return (false);
	// ? ________________

	Request request(parse_request_route(client._request));

	// ? check which method should be called
	if (client._request._method == "GET")
		client._response.append_buffer(this->perform_GET_method(request));
	else
	{
		std::cerr << CYN << "(client._request._method != \"GET\")" << std::endl;
		client._response.append_buffer(get_error_file(501));
	}

	client._response_ready = true;

	return (false);
}

bool				ResponseGenerator::is_directory(const std::string path) const{
	struct stat	s;

	if ( lstat(path.c_str(), &s) == 0 )
    	if (S_ISDIR(s.st_mode))
			return (true);
	return (false);
}

Request 			ResponseGenerator::parse_request_route(Request  const &input_request) const{
	const char					sep = '/';
	int							found  = 0;
	Conf::route_list			routes((*_conf)._routes);
	std::string					file = std::string();
	std::string					path;
	Request						output_request;
	std::string					location;
	
	while (found <= input_request._path.size())
	{
		if ((found = input_request._path.find(sep, found)) == std::string::npos)
			found = input_request._path.size();
		for (Conf::route_list::iterator it = routes.begin(); it != routes.end(); it++)
		{
			location = input_request._path.substr(0,found);
			if (*(location.end() - 1) != '/')
				location+="/";
			if (it->_path == location)
			{
				output_request._route = *it;
				std::cout << "hello it->path = " << output_request._route._path << std::endl;
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
		if (output_request._route._default_file != "")
		{
			if (*(input_request._path.end() - 1) != '/')
				output_request._redir = std::make_pair(301, input_request._path+"/"+output_request._route._default_file);
			else
				output_request._redir = std::make_pair(301, input_request._path+output_request._route._default_file);
		}	
	}
	std::cout << "input = " << input_request._path << ", output = " << output_request._route._path << std::endl;
	if (input_request._path == output_request._route._path && output_request._route._redir != Route::redir_type())
		output_request._redir = output_request._route._redir;
	return (output_request);
}