/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 06:25:14 by dait-atm          #+#    #+#             */
/*   Updated: 2022/03/03 17:32:33 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#define __DEB(s)	std::cerr << MAG << s << std::endl;

/**
 * @brief Default Constructor for a new Server:: Server object.
 */
Server::Server () : _response_generator() {}

/**
 * @brief Construct a new Server:: Server object and initialise it.
 * 
 * @param p Port on which the socket will listen.
 */
Server::Server (const Conf& c)
{
	init(c);
}

/**
 * @brief Destroy the Server:: Server object.
 */
Server::~Server ()
{
	_clients.clear();
	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
	{
		_clients[it->fd].clean_cgi();
		close(it->fd);
	}
	_fds.clear();
}

/**
 * @brief Construct a new Server:: Server object.
 * 
 * @param other Another instance of Server object to copy from.
 */
Server::Server (const Server & other)
{
	*this = other;
}

/**
 * @brief Overload of the operator= for Server::.
 * 
 * @param rhs Another instance of Server object to deep copy from.
 * @return Server& A reference to this object.
 */
Server&			Server::operator= (const Server &rhs)
{
	dup2(rhs._listen_sd, _listen_sd);
	_conf = rhs._conf;
	_fds = rhs._fds;
	_clients = rhs._clients;
	return (*this);
}

/**
 * @brief Bind the listening socket
 * 
 * @return true sucess.
 * @return false bind() failed.
 */
bool			Server::socket_bind ()
{
	struct sockaddr_in	addr;
	int					rc;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(_conf._hosts.begin()->first.c_str());
	addr.sin_port = htons(_conf._hosts.begin()->second);
	rc = bind(_listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(_listen_sd);
		return (false);
	}
	return (true);
}

/**
 * @brief Initialise the server.
 * 
 * @param c Conf object with instructions the Server will need to follow. 
 * @return int On success returns 0.
 *         A positive number is returned in case of error.
 */
int				Server::init (const Conf& c)
{
	int					rc;
	int					on = 1;

	this->_conf = c;
	this->_response_generator.set_conf(&_conf);
	this->_response_generator.set_confs(&_confs);

	// ? AF_INET6 stream socket to receive incoming connections
	_listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_sd < 0)
	{
		perror("socket() failed");
		return (1);
	}

	rc = fcntl(_listen_sd, F_SETFL, O_NONBLOCK);
	if (rc == -1)
		return 38;

	// ? Allow socket descriptor to be reuseable
	rc = setsockopt(_listen_sd, SOL_SOCKET, SO_REUSEADDR,
					(char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(_listen_sd);
		return (2);
	}
	// ? Set _listen_sd (and incoming accepted sockets from it) to be nonblocking
	rc = fcntl(_listen_sd, F_SETFL, O_NONBLOCK);
	if (rc < 0)
	{
		perror("fcntl() failed");
		close(_listen_sd);
		return (3);
	}
	
	// ? Bind the socket
	if (this->socket_bind() == false)
		return (4);

	return (0);
}

/**
 * @brief Add a new client 
 * 
 * @return true On success.
 * @return false client's connection can not be handled.
 */
bool			Server::add_new_client ()
{
	int								new_sd;
	struct pollfd					tmp;
	struct sockaddr_in				addr;
	socklen_t						addr_size = sizeof(struct sockaddr_in);
	std::vector<pollfd>::iterator	it_used_before;

	new_sd = accept(_listen_sd, (struct sockaddr *)&addr, &addr_size);

	if (new_sd < 0)
		return (false);

	// ? in case the file descriptor was already in use waiting to send the response,
	// ? and got invalid during that process. so we remove the previous iteration and add the new one
	for (std::vector<int>::size_type k = 1; k < _fds.size(); ++k)
	{
		if (_fds[k].fd == new_sd)
		{
			remove_client(k);
			break ;
		}
	}

	tmp.fd = new_sd;
	tmp.events = POLLIN;
	tmp.revents = 0;
	_fds.push_back(tmp);

	_clients[new_sd] = Client(inet_ntoa((addr).sin_addr), ft_to_string(htons((addr).sin_port)));

	return (true);
}

/**
 * @brief remove a client of _clients and close it's linked fd
 * 
 * @param i Index from server_poll_loop's for loop.
 * 
 * @return the number of file descriptor removed before i + 1
 */
int				Server::remove_client (size_t i)
{
	int							child_read_fd;
	int							ret = 0;
	std::vector<int>::size_type	j = 0;
	bool						_listener_fd_found = false;

	if (i > 0)
	{
		child_read_fd = _clients[_fds[i].fd].get_cgi_input_fd();
		_clients[_fds[i].fd].clean_cgi();
		if (child_read_fd != -1)
		{
			for (; j < _fds.size(); ++j)
			{
				if (_fds[j].fd == child_read_fd)
				{
					_listener_fd_found = true;
					close(_fds[j].fd);
					_fds[j].fd = -1;
					_fds[j].events = 0;
					_fds[j].revents = 0;
					_listeners.erase(_fds[j].fd);
					_fds.erase(_fds.begin() + j);
					if (j < i)
						++ret;
					break ;
				}
			}
		}
		close(_fds[i].fd);
		_clients.erase(_fds[i].fd);
		_fds[i].fd = -1;
		_fds[i].events = POLLIN;
		_fds[i].revents = 0;
		++ret;
	}
	return (ret);
}

void			Server::add_cgi_listener(const size_t i)
{
	pollfd	tmp;

	tmp.fd = _clients[_fds[i].fd].get_cgi_input_fd();

	tmp.events = POLLIN;
	tmp.revents = 0;
	_fds.push_back(tmp);
}

bool			Server::record_client_input (const size_t &i)
{
	char	buffer[REQUEST_BUFFER_SIZE];
	int		rc;


	if (_clients[_fds[i].fd].is_request_parsed() == true)
	{
		remove_client(i);
		return (true);
	}

	memset(buffer, 0, REQUEST_BUFFER_SIZE);
	rc = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

	if (rc == -1 || rc == 0)	// ? error while reading or client closed the connection
	{
		remove_client(i);
		return (true);
	}

	buffer[rc] = '\0';									// ? closing the char array
	_clients[_fds[i].fd].update();
	_clients[_fds[i].fd].add_input_buffer(buffer, rc);

	_clients[_fds[i].fd].parse_response();

	if (_clients[_fds[i].fd].is_request_parsed() == true)
	{
		_response_generator.generate(_clients[_fds[i].fd]);

		switch (_clients[_fds[i].fd].get_performing_state())
		{
		case FF_FILE_WAITING_TO_BE_IN__FDS :
			add_cgi_listener(i);
			_listeners[_clients[_fds[i].fd].get_cgi_input_fd()] = 0;
			break;
		case FF_CGI_WAITING_TO_BE_IN__FDS :
			add_cgi_listener(i);
			_listeners[_clients[_fds[i].fd].get_cgi_input_fd()] = 1;
			break;
		default:
			_listeners[_clients[_fds[i].fd].get_cgi_input_fd()] = 2;
			set_client_event_to_flag(_fds[i].fd, POLLOUT);
			break;
		}
	}

	return (false);
}

/**
 * @brief Kick out a client if there _life_time is too long.
 */
bool			Server::check_timed_out_client (const size_t i)
{
	if (is_client_fd(_fds[i].fd) && _clients[_fds[i].fd].is_timed_out() == true)
		return (remove_client(i));
	return (0);
}


bool			Server::is_client_fd (const size_t i) const
{
	if (_clients.find(i) != _clients.end())
		return (true);
	return (false);
}

int				Server::pipe_to_client (int fd)
{
	for (std::map<int, Client>::const_iterator cit = _clients.begin(); cit != _clients.end(); ++cit)
	{
		if (fd == cit->second.get_cgi_input_fd())
			return (cit->first);
	}
	return (-1);
}

int				Server::get_client_position (int client_key) const
{
	for (std::vector<int>::size_type k = 0; k < _fds.size(); ++k)
	{
		if (_fds[k].fd == client_key)
			return (k);
	}
	
	exit(92);

	return (-1);
}

void			Server::set_client_event_to_flag (int client_id, int flag)
{
	std::vector<int>::size_type	target_fd = 0;

	for (; target_fd < _fds.size(); ++target_fd)
	{
		if (_fds[target_fd].fd == client_id)
			break ;
	}
	if (target_fd == _fds.size())
		return ;
	_fds[target_fd].events = flag;
}

/**
 * @brief This function handles the poll loop
 *        and call functions according to the events.
 * 
 * Loop over _fds each time a envent is registered.
 * Kick timed out connections.
 * Accept incomimg clients.
 * If poll is timed out Kick timed out connections and return;
 * 
 * @return true On success.
 * @return false An error occured while using poll. 
 */
bool			Server::server_poll_loop ()
{
	int					rc;

	rc = poll(&_fds.front(), _fds.size(), TIMEOUT);
	
	if (rc < 0)
	{
		perror("  poll() failed.");
		close(_listen_sd);
		return (false) ;
	}

	// ? Check to see if TIMEOUT is reached in poll
	if (rc == 0)
	{
		for (std::vector<int>::size_type k = 0; k < _fds.size(); ++k)
		{
			if (is_client_fd(_fds[k].fd))
			{
				k -= check_timed_out_client(k);
			}
			else if (_fds[k].fd == -1)
			{
				_fds.erase(_fds.begin() + k);
				--k;
			}
		}
		return (true);
	}

	// ? Loop through to find the descriptors that returned
	// ? POLLIN and determine whether it's the listening
	// ? or the active connection.
	for (std::vector<int>::size_type i = 0; i < _fds.size(); ++i)
	{
		if (_fds[i].fd == -1)
		{
			_fds.erase(_fds.begin() + i);
			--i;
			continue ;
		}

		// ? if there is no event on this index the loop continues
		if (_fds[i].revents == 0)
		{
			if (is_client_fd(_fds[i].fd))	// ? not the listening socket
				i -= check_timed_out_client(i);
			continue;
		}

		// ? check if it's a new client
		if (_fds[i].fd == _listen_sd)
		{
			if (_fds[i].revents & POLLIN)
				add_new_client();
			else
				return (false);
		}
		// ? else the event was triggered by a pollfd that is already in _fds
		else
		{
			if (_fds[i].revents & (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI))
			{
				if (is_client_fd(_fds[i].fd))
				{
					record_client_input(i);	// ? also adds a listener
					continue ;
				}
				else	// ? it's a listener
				{
					int client_id = pipe_to_client(_fds[i].fd);

					if (client_id == -1)
						continue ;

					// ? get full response
					if (_listeners[_fds[i].fd] == 0)
						this->_response_generator.listen_file(_clients[client_id]);
					else if (_listeners[_fds[i].fd] == 1)
						this->_response_generator.listen_cgi(_clients[client_id]);
					else
						;

					set_client_event_to_flag(client_id, POLLOUT);
				}
			}
			else if (_fds[i].revents & POLLOUT)
			{
				if (is_client_fd(_fds[i].fd))
				{
					if (_clients[_fds[i].fd].is_response_ready())
					{
						if (_clients[_fds[i].fd].send_response(_fds[i].fd) == false
							|| _clients[_fds[i].fd].get_performing_state() == FF_REDIRECT)
							i -= remove_client(i);
						else
						{
							int	input_fd = _clients[_fds[i].fd].get_cgi_input_fd();

							if (input_fd != -1)
							{
								std::vector<int>::size_type target_fd = 0;
								for (; target_fd < _fds.size(); ++target_fd)
								{
									if (_fds[target_fd].fd == input_fd)
										break ;
								}
								if (target_fd == _fds.size())
								{
									i -= remove_client(i);
									continue ;
								}
								_clients[_fds[i].fd].clean_cgi();
								_fds.erase(_fds.begin() + target_fd);
							}
							_listeners.erase(_fds[i].fd);
							_clients[_fds[i].fd] = Client();
							_fds[i].events = POLLIN;
							_fds[i].revents = 0;
						}
					}
				}
				else
				{
					int client_id = pipe_to_client(_fds[i].fd);
					if (client_id == -1)
					{
						close(_fds[i].fd);
						_fds.erase(_fds.begin() + i);
						continue ;
					}
					_response_generator.get_error_file(_clients[client_id], 500);
					_clients[client_id].send_response(client_id);
					remove_client(client_id);
				}
			}
			else
			{
				if (is_client_fd(_fds[i].fd))
					i -= remove_client(i);
				else
				{
					_listeners.erase(_fds[i].fd);
					close(_fds[i].fd);
					_fds.erase(_fds.begin() + i);
					--i;
				}
			}
		}
	}

	return (true);
}

void			sig_handler (int sig)
{
	(void)sig;
	run = false;
	return ;
}

/**
 * @brief This function starts the Server
 * 
 * @return int returns 0 on success, cant' fail.
 */
int				Server::start ()
{
	int				err = 0;
	struct pollfd	tmp;

	signal(SIGINT, &sig_handler);
	signal(SIGQUIT, &sig_handler);

	// ? Set the listen back log (how many events at the same time)
	err = listen(_listen_sd, BACK_LOG);
	if (err < 0)
	{
		perror("listen() failed");
		close(_listen_sd);
		return (5);
	}

	// ? Set the initial listening socket
	tmp.fd = _listen_sd;
	tmp.events = POLLIN;
	tmp.revents = 0;
	_fds.push_back(tmp);

	try
	{
		while (run && server_poll_loop() == true)
			;
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}

	_clients.clear();

	return (0);
}
