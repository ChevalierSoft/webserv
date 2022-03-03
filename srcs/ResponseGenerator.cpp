/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 11:28:08 by dait-atm          #+#    #+#             */
/*   Updated: 2022/03/03 16:33:56 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#ifndef __DEB
# define __DEB(s) std::cerr << CYN << s << RST << std::endl;
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
		_confs = copy._confs;
	}
	return (*this);
}

void				ResponseGenerator::set_conf (const Conf * c)
{
	_conf = c;
}

void				ResponseGenerator::set_confs (const std::vector<Conf> * confs)
{
	_confs = confs;
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

std::string			cool_header ()
{
	std::string		ret("");
	char			d_now[255];

	// ? server name
	ret = "server: 42|webserv\r\n";

	// ? date
	time_t		now = time(0);
	tm			tm;
	size_t		len;
	gmtime_r(&now, &tm);
	len = strftime(d_now, sizeof(d_now), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	ret	+= "date: " + std::string(d_now, len) + "\r\n";

	return (ret);
}

std::string			ResponseGenerator::set_header (int err, std::string ext, size_t size) const
{
	std::string		s_header;
	if (!err)
	{
		err = 200;
		s_header = "HTTP/1.1 " + ft_to_string(err) + " OK\r\n";
	}
	else
		s_header = "HTTP/1.1 " + ft_to_string(err) + " " + _ss_error_messages.find(err)->second + "\r\n";

	s_header += cool_header();
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

	// todo : could be nice to protect this by checking _ss_error_message.find == end()
	s_file_content = ft_to_string(err) + " " + _ss_error_messages.find(err)->second + "\r\n";
	s_full_content = set_header(err, ".html", s_file_content.size()) + s_file_content;

	return (s_full_content);
}

void				ResponseGenerator::get_error_file (Client & client, Conf::code_type err) const
{
	std::string							s_file_content = "";
	std::ifstream						i_file;
	std::string							tmp;
	Conf::error_list::const_iterator	it = _confs->at(client._request._conf_index)._error_pages.find(err);

	if (it == _confs->at(client._request._conf_index)._error_pages.end())
		client._response = generic_error(err);
	else
	{
		i_file.open(it->second.c_str());

		if (i_file.is_open())
		{
			while (i_file.good())
			{
				std::getline(i_file, tmp);
				s_file_content += (tmp + "\n");
			}
			client._response += (set_header(err, ".html", s_file_content.size()));
			client._response += (s_file_content);
		}
		else
			client._response = generic_error(err);
	}

	client._fast_forward = FF_READY;
	client._response_ready = true;
	return ;
}

/**
 * @brief get the requested file and fill a string with it.
 * 
 * @param root the site's root
 * @param path the requested file
 * @return std::string file content as string
 */
void				ResponseGenerator::listen_file (Client & client) const
{
	char			a_tmp[FILE_BUFF_SIZE];
	int				err;

	client._fast_forward = FF_GET_FILE;

	while (1)
	{
		err = read(client._webserv_pipe[0], a_tmp, FILE_BUFF_SIZE);
		if (err < 0)
			get_error_file(client, 500);
		else if (err == 0 || err < FILE_BUFF_SIZE)
		{
			client._response += std::string(a_tmp, err);
			break ;
		}
		client._response += std::string(a_tmp, err);
	}

	client._response = set_header(200, get_file_extention(get_file_name(client._request._path)), client._response.size()) + client._response;
	client._response_ready = true;
}

void				ResponseGenerator::set_cgi_env (Client & client, std::string path, std::vector<std::string> & s_envs, std::vector<char *> & a_envs) const
{
	char	cwd[PATH_MAX];
	
	getcwd(cwd, sizeof(cwd));

	s_envs.push_back("QUERY_STRING=" + client._request._get_query);

	if (client._request._method == "POST")
	{
		s_envs.push_back("CONTENT_LENGTH=" + ft_to_string(client._request.begin_body()->size()));
		s_envs.push_back("CONTENT_TYPE=" + client._request.find_header("Content-Type"));
	}
	else
		s_envs.push_back("CONTENT_LENGTH=0");

	s_envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
	s_envs.push_back("REQUEST_METHOD=" + client._request._method);
	s_envs.push_back("SERVER_NAME=" + std::string(cwd) + client._request._path);
	s_envs.push_back("SCRIPT_FILENAME="+ std::string(cwd) + "/" + path);
	s_envs.push_back("SCRIPT_NAME=" + client._request._path);
	s_envs.push_back("SCRIPT_FILE_NAME=" + client._request._path);
	s_envs.push_back("REDIRECT_STATUS=200");
	s_envs.push_back("REQUEST_URI=" + client._request._path + "?" + client._request._get_query);
	s_envs.push_back("REMOTE_ADDR=" + client._ip);
	s_envs.push_back("REMOTE_PORT=" + client.port);
	s_envs.push_back("SERVER_ADDR=" + this->_confs->at(client._request._conf_index)._hosts.begin()->first);
	s_envs.push_back("SERVER_PORT=" + ft_to_string(this->_confs->at(client._request._conf_index)._hosts.begin()->second));
	s_envs.push_back("REQUEST_SCHEME=http");
	s_envs.push_back("SERVER_SIGNATURE=42|webserv");
	s_envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
	s_envs.push_back("SHLVL=2");
	s_envs.push_back("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");	// TODO : use the one from main
	s_envs.push_back("REQUEST_SCHEME=http");
	s_envs.push_back("PATH_TRANSLATED=" + client._request._path);
	s_envs.push_back("PATH_INFO=" + client._request._path);
	s_envs.push_back("AUTH_TYPE=BASIC");	// ? not needed

	// ? this adds request's headers to env
	for (Request::it_header it = client._request.begin_header();
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

void				ResponseGenerator::start_cgi (Client & client, std::string cgi_url, std::string path) const
{
	char						*exe[3];
	std::vector<char *>			a_envs;

	set_cgi_env(client, path, client.s_envs, a_envs);

	exe[0] = &cgi_url[0];
	exe[1] = &path[0];
	exe[2] = NULL;

	close(client._cgi_pipe[1]);
	close(client._webserv_pipe[0]);

	dup2(client._cgi_pipe[0], 0);
	dup2(client._webserv_pipe[1], 1);

	execve(exe[0], exe, a_envs.data());

	exit(66);
}

void				ResponseGenerator::listen_cgi (Client & client) const
{
	int							err = 0;
	std::string					page_header;
	char						buff[CGI_BUFF_SIZE];
	int							cgi_header_size;
	bool						death_check = true;

	client._fast_forward = FF_GET_CGI;

	while (1)
	{
		if (death_check && waitpid(-1, &client._child, WNOHANG))
		{
			client._child = -1;
			close(client._webserv_pipe[1]);
			close(client._cgi_pipe[0]);
			close(client._cgi_pipe[1]);
			death_check = false;
		}
		if (WIFEXITED(client._child) && WEXITSTATUS(client._child) != 0)
		{
			get_error_file(client, 500);
			return ;
		}
		memset(buff, 0, CGI_BUFF_SIZE);
		err = read(client._webserv_pipe[0], buff, CGI_BUFF_SIZE);
		
		if (err < 0)		// ? error while reading
		{
			get_error_file(client, 500);
			return ;
		}
		else if (err == 0 || err < CGI_BUFF_SIZE)	// ? everything on the pipe was read
		{
			client._response += std::string(buff, err);
			break ;
		}

		client._response += std::string(buff, err);
	}

	client._response_ready = true;

	// ? adding the first part of the header
	page_header = "HTTP/1.1 200 OK\r\n";
	page_header += cool_header();
	page_header += "Content-Length: ";
	cgi_header_size = client._response.find("\r\n\r\n");
	if (cgi_header_size == -1)
		page_header += "0\r\n";
	else
		page_header += ft_to_string(client._response.length() - (cgi_header_size + 4)) + "\r\n";
	client._response = page_header + client._response;
	return ;

}

bool				ResponseGenerator::cgi_send_body (Client & client) const
{
	int	err;

	if (client._request._method != "POST")
		return (false);

	for (std::vector<std::string>::const_iterator cit = client._request.begin_body();
		cit != client._request.end_body(); ++cit)
	{
		err = write(client._cgi_pipe[1], cit->c_str(), cit->length());
		if (err < 0)
			return (true);
	}

	close(client._cgi_pipe[1]);
	client._cgi_pipe[1] = -1;

	return (false);
}

void				ResponseGenerator::cgi_handling (Client & client) const
{
	std::string		response;

	if (pipe(client._cgi_pipe))
	{
		__DEB("                    FIRST PIPE FAILED")
		get_error_file(client, 500);
		return ;
	}
	if (pipe(client._webserv_pipe))
	{
		__DEB("                    SECOND PIPE FAILED")
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		get_error_file(client, 500);
		return ;
	}

	// ? set non blocking the read part of the pipe
	if (fcntl(client._cgi_pipe[0], F_SETFL, O_NONBLOCK) < 0
		|| fcntl(client._webserv_pipe[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		get_error_file(client, 500);
		return ;
	}

	if (fcntl(client._webserv_pipe[0], F_SETFL, O_NONBLOCK) < 0
		|| fcntl(client._webserv_pipe[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		close(client._webserv_pipe[0]);
		close(client._webserv_pipe[1]);
		get_error_file(client, 500);
		return ;
	}

	client._child = fork();
	if (client._child < 0)
	{
		close(client._cgi_pipe[0]);
		close(client._cgi_pipe[1]);
		close(client._webserv_pipe[0]);
		close(client._webserv_pipe[1]);
		get_error_file(client, 500);
		return ;
	}
	else if (!client._child)
		this->start_cgi(client, client._cgi->second, client._request._path);

	if (cgi_send_body(client))
	{
		get_error_file(client, 500);
		return ;
	}

	client._fast_forward = FF_CGI_WAITING_TO_BE_IN__FDS;

	return ;
}

void				ResponseGenerator::file_handling (Client & client) const
{
	client._webserv_pipe[0] = open((client._request._path).c_str(), O_RDONLY);
	if (client._webserv_pipe[0] == -1)
		get_error_file(client, 500);
	else
		client._fast_forward = FF_FILE_WAITING_TO_BE_IN__FDS;
}


std::string			ResponseGenerator::get_redirection(const Route::redir_type & redir) const {
	return ("HTTP/1.1 " + ft_to_string(redir.first) + " " + _ss_error_messages.at(redir.first) + "\r\nLocation: " + redir.second + "\r\n\r\n");
}

void				ResponseGenerator::perform_method (Client & client) const
{
	struct stat	s;
	int			upload_error = 3;


	if (!(upload_error = client._request.is_upload(_confs->at(client._request._conf_index))) && client._request.upload_to_server(_confs->at(client._request._conf_index)))
		return (get_error_file(client, 204)); // no content to output
	else if (upload_error == 1) // if upload folder exists but doesnt have the rights to create a file
		return (get_error_file(client, 404));
	else if (upload_error == 2) // if upload folder doesnt exist
		return (get_error_file(client, 403));

	// ? redirects if there is a redirection in appropriate route AND if what is typed in the url corresponds to location in conf
	if (client._request._redir != Route::redir_type())
	{
		client._response += (get_redirection(client._request._redir));
		client._fast_forward = FF_REDIRECT;
		client._response_ready = true;
		return ;
	}

	if (! stat((client._request._path).c_str(), &s))
	{
		if (s.st_mode & S_IFDIR)	// ? the requested path is a directory
		{
			if (client._request._route._dir_listing) // ? check if directory listing is on
			{
				client._response += (directory_listing(client._request._path_raw, client._request._path));
				client._response_ready = true;
			}
			else
				get_error_file(client, 403);
		}
		else if (s.st_mode & S_IFREG)	// ? the requested path is a file
		{
			client._cgi = client._request._route._cgis.find(get_file_extention((client._request._path)));
			if (client._cgi != client._request._route._cgis.end())
				cgi_handling(client);
			else
			{
				client._input_file.clear();
				client._input_file.open((client._request._path).c_str());
				if (! client._input_file.good())
					get_error_file(client, 403);
				else
					file_handling(client);
				client._input_file.close();
			}
		}
		else
		{
			// ? error: it's there, but it's not a directory or a file.
			// ? not sure if symlinks must work.
			get_error_file(client, 418);
		}
	}
	else
	{
		// ? error: wrong path || path too long || out of memory || bad address || ...
		get_error_file(client, 404);
	}
}

void				ResponseGenerator::perform_delete(Client & client) const
{
	std::string	header;
	std::string	file_content;
	std::string	root;
	struct stat	s_root;
	struct stat	s_file;

	root = client._request._path;
	if (*(client._request._path.end() - 1) == '/')
		root = client._request._path.substr(0, client._request._path.size() - 1);
	root = root.substr(0, root.rfind('/'));
	if (root == ".")
		get_error_file(client, 401);
	else if (!stat(root.c_str(), &s_root))
	{
		if ((s_root.st_mode & S_IWOTH) && (s_root.st_mode & S_IXOTH))
		{
			if (!stat(client._request._path.c_str(), &s_file))
			{
				if (remove(client._request._path.c_str()))
					get_error_file(client, 500);
				else
				{
					file_content = "<html>\n";
					file_content += "\t<body>\n";
					file_content += "\t\t<h1>"+client._request._path+" deleted.</h1>\n";
					file_content += "\t</body>\n";
					file_content += "</html>\n";
					client._response += (set_header(0, ".html", file_content.size()) + file_content);
					client._response_ready = true;
					return ;
				}
			}
			else
				get_error_file(client, 404);
		}
		else
			get_error_file(client, 403);
	}
	else
		get_error_file(client, 404);
}

int					ResponseGenerator::is_method(std::string method, Request const &rq) const {
	if (method == rq._method)
	{
		if (std::find(rq._route._methods.begin(), rq._route._methods.end(), method) !=  rq._route._methods.end())
			return (1);
		else
			return (2);
	}
	else 
		return (0);
}

bool				ResponseGenerator::generate (Client& client) const
{
	int	error_code;

	set_conf_index(client); //? Setting conf index here

	error_code = client._request.request_error(_confs->at(client._request._conf_index));
	if (error_code)
	{
		get_error_file(client, error_code);
		return (false);
	}
	if (client._request_parsed == false)
	{
		parse_request_route(client);
		client._request_parsed = true;
	}
	// ? check which method should be called
	if (is_method("GET", client._request) == 1 || is_method("POST", client._request) == 1)
		this->perform_method(client);
	else if (is_method("DELETE", client._request) == 1)
		this->perform_delete(client);
	else if (is_method("GET", client._request) == 2 || is_method("POST", client._request) == 2|| is_method("DELETE", client._request) == 2)
		get_error_file(client, 405);
	else
		get_error_file(client, 501);

	return (false);
}

bool				ResponseGenerator::is_directory(const std::string path) const{
	struct stat	s;

	if ( lstat(path.c_str(), &s) == 0 )
    	if (S_ISDIR(s.st_mode))
			return (true);
	return (false);
}

void				ResponseGenerator::set_conf_index(Client &client) const {
	std::string host = client._request.find_header("Host");
	host = host.substr(0, host.rfind(':'));
	client._request._conf_index = 0;
	std::vector<Conf>::const_iterator it = _confs->begin();
	while (it != _confs->end() && it->_name != host)
		it++;
	if (it != _confs->end())
		client._request._conf_index = it - _confs->begin();
}

void 				ResponseGenerator::parse_request_route(Client &client) const{
	const char					sep = '/';
	size_t						found  = 0;
	std::string					file = std::string();
	std::string					location;
	Conf::route_list			routes(_confs->at(client._request._conf_index)._routes);
	
	client._request._path_raw = client._request._path;
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
			if (*(client._request._path_raw.end() - 1) != '/')
				client._request._redir = std::make_pair(301, client._request._path_raw+"/"+client._request._route._default_file);
			else
				client._request._redir = std::make_pair(301, client._request._path_raw+client._request._route._default_file);
		}	
	}

	// If the inut_path is exactly the name of a route and this route has a redirection defined, add it

	if (client._request._route._redir != Route::redir_type() && client._request._path_raw == client._request._route._path)
		client._request._redir = client._request._route._redir;
}