/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/08 16:06:30 by lpellier         ###   ########.fr       */
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
#include <sstream>				
#include <limits.h>				// PATH_MAX
#include "ResponseGenerator.hpp"
#include "webserv.hpp"
#include "ft_to_string.hpp"
#include "utils.hpp"
#include "set_content_types.hpp"
#include <algorithm>
#include <cstdio>

#ifndef __DEB
# define __DEB(s) std::cerr << GRN << s << RST << std::endl;
#endif

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
	if (!err)
	{
		err = 200;
		s_header = "HTTP/1.1 " + ft_to_string(err) + " OK" + "\r\n";
	}
	else
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
std::string			ResponseGenerator::get_file_content(Client & client) const
{
	std::ifstream					i_file;
	std::string						tmp;
	std::string						s_file_content = "";
	std::string						s_full_content;
	Route::cgi_list::const_iterator	cgi;	

	i_file.open((client._request._path).c_str());


	if (i_file.is_open())
	{
		if ((cgi = client._request._route._cgis.find(get_file_extention((client._request._path)))) != client._request._route._cgis.end())
			return (cgi_handling(client, cgi->second, client._request._path)); // ! c'est ici qu'on retourne la reponse du cgi maintenant
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

	s_full_content = set_header(0, get_file_extention(get_file_name(client._request._path)), s_file_content.size());

	s_full_content += s_file_content;
	return (s_full_content);
}

void				ResponseGenerator::set_cgi_env (Client & client, std::string path, std::vector<std::string> & s_envs, std::vector<char *> & a_envs) const
{
	// TODO : this could be interesting to add env passed to main()
	char	cwd[PATH_MAX];
	
	getcwd(cwd, sizeof(cwd));

	s_envs.push_back("QUERY_STRING=" + client._request._get_query);

	if (client._request._method == "POST")
	{
		s_envs.push_back("CONTENT_LENGTH=" + ft_to_string(client._request.begin_body()->size()));
		s_envs.push_back("CONTENT_TYPE=" + client._request.find_header("Content-Type"));
	}
	else
	{
		s_envs.push_back("CONTENT_LENGTH=0");
	}

	s_envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
	s_envs.push_back("REQUEST_METHOD=" + client._request._method);
	s_envs.push_back("SERVER_NAME=" + std::string(cwd) + client._request._path);
	s_envs.push_back("SCRIPT_FILENAME="+ std::string(cwd) + "/" + path);
	s_envs.push_back("SCRIPT_NAME=" + client._request._path);
	s_envs.push_back("SCRIPT_FILE_NAME=" + client._request._path);
	s_envs.push_back("REDIRECT_STATUS=200");
	s_envs.push_back("REQUEST_URI=" + client._request._path + "?" + client._request._get_query);
	s_envs.push_back("REMOTE_ADDR=" + client._ip);
	s_envs.push_back("REMOTE_PORT=" + client._port);
	s_envs.push_back("SERVER_ADDR=" + this->_conf->_hosts.begin()->first);
	s_envs.push_back("SERVER_PORT=" + ft_to_string(this->_conf->_hosts.begin()->second));
	s_envs.push_back("REQUEST_SCHEME=http");
	s_envs.push_back("SERVER_SIGNATURE=42|webserv");
	s_envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
	s_envs.push_back("SHLVL=2");
	s_envs.push_back("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");	// TODO : use the one from main
	s_envs.push_back("REQUEST_SCHEME=http");
	s_envs.push_back("PATH_TRANSLATED=" + client._request._path);
	s_envs.push_back("PATH_INFO=" + client._request._path);
	// s_envs.push_back("CONTEXT_PREFIX=/`-bin/");
	// CONTEXT_DOCUMENT_ROOT= // ? add a complete link
	s_envs.push_back("AUTH_TYPE=BASIC");	// ? not needed

	// ? this adds request's headers to env
	for (Message::it_header it = client._request.begin_header();
			it != client._request.end_header(); ++it)
	{
		if (it->first == "Content-Type")
			continue ;
		std::string tmp = it->first;
		for (std::string::iterator sit = tmp.begin(); sit != tmp.end(); ++sit)
		{
			if (*sit >= 'a' && *sit <= 'z')
				*sit -= 'a' - 'A';
			else if (*sit == '-')
				*sit = '_';
		}
		s_envs.push_back("HTML_" + tmp + "=" + it->second);
	}

	// ? feeding the vector of char* with the pointers from s_envs
	int i = 0;
	for (std::vector<std::string>::const_iterator cit = s_envs.begin();
			cit != s_envs.end() ; ++cit)
		a_envs.push_back(&s_envs[i++][0]);
	a_envs.push_back(NULL);

	return ;
}

void				ResponseGenerator::start_cgi (Client & client, std::string cgi_url, std::string path, int cgi_pipe[2]) const
{
	char						*exe[3];
	std::vector<std::string>	s_envs;
	std::vector<char *>			a_envs;

	set_cgi_env(client, path, s_envs, a_envs);

	exe[0] = &cgi_url[0];
	exe[1] = &path[0];
	exe[2] = NULL;

	close(client._cgi_pipe[1]);
	close(client._webserv_pipe[0]);

	dup2(client._cgi_pipe[0], 0);
	dup2(client._webserv_pipe[1], 1);

	execve(exe[0], exe, a_envs.data());

	// TODO : clean memory / close pipes. maybe by an ugly exception
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
	int							cgi_header_size;

	// ! need to use WNOHANG and check every loop (when it will be implemented)
	// ? https://cboard.cprogramming.com/c-programming/138057-waitpid-non-blocking-fork.html
	waitpid(-1, &child, 0);

	// if (WIFEXITED(child))
	// 	std::cerr << "CGI returned : " << WEXITSTATUS(child) << std::endl;

	// ! avoid this loop by entering/leaving this function until child is exited
	while (1)
	{
		memset(buff, 0, CGI_BUFF_SIZE);
		err = read(client._webserv_pipe[0], buff, CGI_BUFF_SIZE - 1);
		if (err <= 0)
			break ;
		response += buff;
	}

	close(client._webserv_pipe[0]);

	// ? adding the first part of the header
	page = "HTTP/1.1 200 OK\r\n";
	page += "Server: Webserv 42\r\n";	// TODO : set a cool header
	page += "Content-Length: ";
	cgi_header_size = response.find("\r\n\r\n");
	if (cgi_header_size == std::string::npos)
		page += "0\r\n";
	else
		page += ft_to_string(response.length() - (cgi_header_size + 4)) + "\r\n";
	page += response;

	return (page);
}

bool				ResponseGenerator::cgi_send_body (Client & client, int cgi_pipe[2]) const
{
	int	err;

	if (client._request._method != "POST")
	{
		close(client._cgi_pipe[1]);
		client._body_sent = true;
		return (false);
	}

	std::cerr << "sending body" << std::endl;

	// TODO : when non blocking will be a thing, iterate piece by piece
	for (std::vector<std::string>::const_iterator cit = client._request.begin_body();
		cit != client._request.end_body(); ++cit)
	{
		std::cout << "sending : " << cit->c_str() << std::endl;
		err = write(client._cgi_pipe[1], cit->c_str(), cit->length());
		if (err < 0)
			return (true);
	}

	close(client._cgi_pipe[1]);

	// TODO : if (cit == client._request.end_body())
	std::cerr << "body sent" << std::endl;
	client._body_sent = true;

	return (false);
}

std::string			ResponseGenerator::cgi_handling (Client & client, std::string cgi_url, std::string path) const
{
	pid_t			child;
	std::string		response;

	if (pipe(client._cgi_pipe))
		return (get_error_file(500));
	if (pipe(client._webserv_pipe))
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		return (get_error_file(500));
	}

	// ? set non blocking the read part of the pipe
	if (fcntl(client._cgi_pipe[0], F_SETFL, O_NONBLOCK) < 0
		|| fcntl(client._webserv_pipe[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		return (get_error_file(500));
	}

	if (fcntl(client._webserv_pipe[0], F_SETFL, O_NONBLOCK) < 0
		|| fcntl(client._webserv_pipe[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		close(client._webserv_pipe[0]);
		close(client._webserv_pipe[1]);
		return (get_error_file(500));
	}

	child = fork();
	if (child < 0)
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		close(client._webserv_pipe[0]);
		close(client._webserv_pipe[1]);
		return (get_error_file(500));
	}
	else if (!child)
		this->start_cgi(client, cgi_url, path, client._cgi_pipe);

	close(client._webserv_pipe[1]);
	close(client._cgi_pipe[0]);

	if (client._body_sent == false)
	{
		if (cgi_send_body(client, client._cgi_pipe))
			return (get_error_file(500));
	}

	response = listen_cgi(client, cgi_url, client._cgi_pipe, child);

	return (response);
}

std::string			ResponseGenerator::get_redirection(const Route::redir_type & redir) const {
	return ("HTTP/1.1 " + ft_to_string(redir.first) + " " + _ss_error_messages.at(redir.first) + "\r\nLocation: " + redir.second + "\r\n\r\n");
}

std::string			ResponseGenerator::perform_method (Client & cl) const
{
	struct stat s;


	// if (is_upload_request(rq) && upload_to_server(rq))
	// 	return (get_error_file(204)); // no content to output
	// ? redirects if there is a redirection in appropriate route AND if what is typed in the url corresponds to location in conf
	if (cl._request._redir != Route::redir_type())
		return (get_redirection(cl._request._redir));
	if ( !(stat((cl._request._path).c_str(), &s)) )
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			if (cl._request._route._dir_listing) // check if directory listing is on
				return (directory_listing(cl._request._path));
			else
				return (get_error_file(403));
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			return (get_file_content(cl));
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
std::string			ResponseGenerator::perform_delete(const Request & rq) const {
	std::string header;
	std::string	file_content;
	std::string	root;
	struct stat	s_root;
	struct stat	s_file;

	root = rq._path;
	if (*(rq._path.end() - 1) == '/')
		root = rq._path.substr(0, rq._path.size() - 1);
	root = root.substr(0, root.rfind('/'));
	if (root == ".")
		return (get_error_file(401));
	if (!stat(root.c_str(), &s_root))
	{
		if ((s_root.st_mode & S_IWUSR) && (s_root.st_mode & S_IXUSR))
		{
			if (!stat(rq._path.c_str(), &s_file))
			{
				if (remove(rq._path.c_str()))
					return (get_error_file(500));
				file_content = "<html>\n";
				file_content += "\t<body>\n";
				file_content += "\t\t<h1>"+rq._path+" deleted.</h1>\n";
				file_content += "\t</body>\n";
				file_content += "</html>\n";
				return (set_header(0, ".html", file_content.size()) + file_content);	
			}
			else
				return (get_error_file(404));
		}
		else
			return (get_error_file(403));
	}
	else
		return (get_error_file(404));
}
/**
 * @brief generate the response for the client
 * 
 * @param rq 
 * @return true internal error, need to close the client connexion without sending response
 * @return false all good
 */

bool				ResponseGenerator::is_method(std::string method, Request const &rq) const {
	return (method == rq._method && (std::find(rq._route._methods.begin(), rq._route._methods.end(), method) !=  rq._route._methods.end()));
}

bool				ResponseGenerator::generate(Client& client) const
{
	// ! clear at the creation of the client. here it will erase the response each loop 
	client._response.clear();

	int	error_code;

	if ((error_code = client._request.request_error())) {
		client._response.append_buffer(get_error_file(error_code));
		client._response_ready = true;
		return (false);
	}

	parse_request_route(client);

	// ? check which method should be called
	if (is_method("GET", client._request) || is_method("POST", client._request))
		client._response.append_buffer(this->perform_method(client));
	else if (is_method("DELETE", client._request))
		client._response.append_buffer(this->perform_delete(client._request));
	else
		client._response.append_buffer(get_error_file(501));

	client._response_ready = true;	// this will not be set here in the future

	return (false);
}

bool				ResponseGenerator::is_directory(const std::string path) const{
	struct stat	s;

	if ( lstat(path.c_str(), &s) == 0 )
    	if (S_ISDIR(s.st_mode))
			return (true);
	return (false);
}

void 			ResponseGenerator::parse_request_route(Client &client) const{
	const char					sep = '/';
	int							found  = 0;
	Conf::route_list			routes((*_conf)._routes);
	std::string					file = std::string();
	std::string					input_path(client._request._path);
	std::string					location;
	
	// Loop to find the route and set it to output request route
	while (found <= client._request._path.size())
	{
		if ((found = client._request._path.find(sep, found)) == std::string::npos)
			found = client._request._path.size();
		for (Conf::route_list::iterator it = routes.begin(); it != routes.end(); it++)
		{
			location = client._request._path.substr(0,found);
			// if (*(location.end() - 1) != '/')
			// 	location+="/";
			if (it->_path == location+"/")
			{
				client._request._route = *it;
				if (found < client._request._path.size())
					file = client._request._path.substr(found + 1, client._request._path.size() - found);
				else
					file = "";
			}
		}
		found++;
	}
	// once route is found, path is equal to the location + anything after the route name 
	client._request._path = client._request._route._location+file;
	// If file is directory, check for default file
	if (is_directory(client._request._path))
	{
		if (*(client._request._path.end() - 1) != '/')
			client._request._path+="/";
		// Define redirection if there is a default file
		if (client._request._route._default_file != Route::file_type())
		{
			if (*(input_path.end() - 1) != '/')
				client._request._redir = std::make_pair(301, input_path+"/"+client._request._route._default_file);
			else
				client._request._redir = std::make_pair(301, input_path+client._request._route._default_file);
		}	
	}

	// If the inut_path is exactly the name of a route and this route has a redirection defined, add it

	if (client._request._route._redir != Route::redir_type() && input_path == client._request._route._path)
		client._request._redir = client._request._route._redir;
}