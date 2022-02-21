/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 06:25:14 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/21 18:56:16 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#define __DEB(s)	std::cerr << MAG << s << RST << std::endl;

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
	std::cout << YEL << "~SERVER" << RST << std::endl;
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

	// rc = fcntl(_listen_sd, F_SETFL, O_NONBLOCK);
	// if (rc == -1)
	// 	return 38;

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
	// rc = ioctl(_listen_sd, FIONBIO, (char *)&on);
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
	
	// std::cout << "ready to listen on port " << _conf._hosts.begin()->second << std::endl;
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
	int					new_sd;
	struct pollfd		tmp;
	struct sockaddr_in	addr;
	socklen_t			addr_size = sizeof(struct sockaddr_in);

	std::cout << "  Listening socket is readable\n";

	new_sd = accept(_listen_sd, (struct sockaddr *)&addr, &addr_size);

	// std::cout << "  New connection from : " << inet_ntoa(((addr)).sin_addr) << std::endl;
	// std::cout << "  on port : " << htons((addr).sin_port) << std::endl;

	if (new_sd < 0)
	{
		std::cerr << "  error: can't accept client: " << new_sd << std::endl;
		return (false);
	}

	std::cout << YEL << "  New incoming connection on fd : " << RED << new_sd << RST << std::endl;

	tmp.fd = new_sd;
	tmp.events = POLLIN;
	tmp.revents = 0;
	_fds.push_back(tmp);

	_clients[new_sd] = Client(&this->_conf, inet_ntoa((addr).sin_addr), ft_to_string(htons((addr).sin_port)));

	return (true);
}

/**
 * @brief remove a client of _clients and close it's linked fd
 * 
 * @param i Index from server_poll_loop's for loop.
 */
void			Server::remove_client (int i)
{
	int		position_child_read_fd;

	// std::cout << "  remove_client " << i << std::endl;
	if (i > 0)
	{
		close(_fds[i].fd);
		position_child_read_fd = _clients[_fds[i].fd].get_cgi_input_fd();
		_clients[_fds[i].fd].clean_cgi();

		if (position_child_read_fd != -1)
		{
			for (int j = 0; j < _fds.size(); ++j)
			{
				if (_fds[j].fd == position_child_read_fd)
				{
					std::cout << "removing child's fd [" << j << "] : " << _fds[j].fd << " from _fds." << std::endl;
					_fds[j].fd = -1;
					_fds[j].events = 0;
					_fds[j].revents = 0;
					break ;
				}
			}
		}
		_clients.erase(_fds[i].fd);
		_fds[i].fd = -1;
		_fds[i].events = 0;
		_fds[i].revents = 0;
	}
}

void			Server::add_cgi_listener(const int i)
{
	std::cout << "  add_cgi_listener " << i << " " << _fds[i].fd << std::endl;
	pollfd	tmp;

	tmp.fd = _clients[_fds[i].fd].get_cgi_input_fd();
	std::cout << CYN <<"  _clients[_fds[i].fd].get_cgi_input_fd() : " << _clients[_fds[i].fd].get_cgi_input_fd() <<RST<< std::endl;

	tmp.events = POLLIN;
	tmp.revents = 0;
	_fds.push_back(tmp);
	// _clients[_fds[i].fd].set_cgi_input_position(_fds.size() - 1);
}

#include "ft_print_memory.hpp"

void	*ft_print_memory(void *addr, size_t size);

bool			Server::record_client_input (const int &i)
{
	char	buffer[REQUEST_BUFFER_SIZE];
	bool	close_conn = 0;
	int		rc;

	usleep(100000);

	std::cout << "record_client_input " << i << " " << _fds[i].fd << std::endl;

	if (_clients[_fds[i].fd].is_request_parsed() == true)
		return (true);

	while (1)
	{
		memset(buffer, 0, REQUEST_BUFFER_SIZE);
		rc = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0); // MSG_DONTWAIT | MSG_ERRQUEUE);	// ? errors number can be checked with the flag MSG_ERRQUEUE (man recv)

		std::cout << "rc : " << rc << std::endl;

		if (rc == -1)	// ? error while reading or client closed the connection
		{
			std::cerr <<MAG<< "client closed the connection" <<RST<< std::endl;
			_fds[i].fd = -1;
			_fds[i].events = 0;
			_fds[i].revents = 0;
			return (true);
		}
		else if (rc == 0)
			break ;	// ! tout est lu ?
		

		buffer[rc] = '\0';									// ? closing the char array
		_clients[_fds[i].fd].update();
		_clients[_fds[i].fd].add_input_buffer(buffer, rc);

		ft_print_memory((void *)buffer, rc);

		// ? update client's _life_time


		// if (_clients[_fds[i].fd].is_request_parsed() == false) // ? no need here
			_clients[_fds[i].fd].parse_response();
		// ? If needed, this is where we could send 100 Continue

		if (_clients[_fds[i].fd].is_request_parsed() == true)
			break ;

	}

	// if (_clients[_fds[i].fd].is_request_parsed() == true)
	{
		__DEB("  is_request_parsed : true")
		close_conn = this->_response_generator.generate(_clients[_fds[i].fd]);

		if (_clients[_fds[i].fd].get_performing_state() == FF_WAITING_TO_BE_IN__FDS)
		{
			add_cgi_listener(i);
				std::cout << CYN <<"  _clients[_fds[i].fd].get_cgi_input_fd() : " << _clients[_fds[i].fd].get_cgi_input_fd() <<RST<< std::endl;
		}
	}
	return (false);
}

/**
 * @brief Kick out a client if there _life_time is too long.
 */
bool			Server::check_timed_out_client (const int i)
{	
	// std::cout << "check_timed_out_client [" << i << "] : " << _fds[i].fd << std::endl;

	if (is_client_fd(_fds[i].fd) && _clients[_fds[i].fd].is_timed_out() == true)
	{
		remove_client(i);
		return (1);
	}
	return (0);
}


bool			Server::is_client_fd (const int i) const
{
	// std::cout << CYN << "_clients.size : " << _clients.size() << RST << std::endl;

	if (_clients.find(i) != _clients.end())
	{
		// std::cout << MAG << "is_client_fd " << i << " is true" << RST << std::endl;
		return (true);
	}
	// std::cout << MAG << "is_client_fd " << i << " is false" << RST << std::endl;
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
	bool				need_cleaning = 0;

	std::cout << "Waiting on poll()...\n";
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
		std::cout << "  poll() timed out." << std::endl;
		// aff_fds();
		// aff_clients();
		for (int i = 0; i < _fds.size(); ++i)
		{
			if (is_client_fd(_fds[i].fd))
			{
				__DEB("here")
				i -= check_timed_out_client(i);
			}
			else if (_fds[i].fd == -1)
			{
				_fds.erase(_fds.begin() + i);
				--i;
			}
		}
		return (true);
	}

	// ? Loop through to find the descriptors that returned
	// ? POLLIN and determine whether it's the listening
	// ? or the active connection.
	for (int i = 0; i < _fds.size(); ++i)
	{
		if (_fds[i].fd == -1)
		{
			// std::cout << "found _fds[i].fd == -1" << std::endl; 
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

		if (_fds[i].revents != POLLIN && _fds[i].revents != POLLOUT)
		{
			if (i == 0)
				return (false);
			else if (is_client_fd(_fds[i].fd))
			{
				remove_client(i);
				--i;
			}
			else
			{
				_fds.erase(_fds.begin() + i);
				--i;
			}
			continue ;
		}

		std::cout << "poll triggered by : [" << i << "] : " << _fds[i].fd << std::endl;

		// ? check if it's a new client
		if (_fds[i].fd == _listen_sd)
		{
			if (_fds[i].revents != POLLIN)
				return (false);
			else
				add_new_client();
		}
		// ? else the event was triggered by a pollfd that is already in _fds
		else
		{
			bool	close_conn = false;

			if (_fds[i].revents == POLLIN)
			{
				if (is_client_fd(_fds[i].fd))
				{
					record_client_input(i);
				}
				else
				{
					int client_id = pipe_to_client(_fds[i].fd);

					if (client_id == -1)
					{
						__DEB("Error :  pipe_to_client returned -1")
						close (_fds[i].fd);
						// _fds[i].fd = -1;
						_fds.erase(_fds.begin() + i);
						--i;
						continue ;
					}
					if (_response_generator.listen_cgi(_clients[client_id]))
					{
						// ? response ready
						int target_fd = 0;
						for (; target_fd < _fds.size(); ++target_fd)
						{
							if (_fds[target_fd].fd == client_id)
								break ;
						}
						if (i == _fds.size())
						{
							std::cout << RED << "Error : client_id " << client_id << " not found in _fds" << std::endl;
							exit(404);
						}
						std::cout << RED << "client_id : " << client_id << " -> target_fd : " << target_fd << RST << std::endl;

						_fds[target_fd].events = POLLOUT;
						_fds[target_fd].revents = POLLOUT;
					}
				}
			}
			// if (is_client_fd(_fds[i].fd) && _clients[_fds[i].fd].is_response_ready())
			else if (_fds[i].revents == POLLOUT)
			{
				if (is_client_fd(_fds[i].fd))
				{
					__DEB("  is_response_ready")

					std::cout << CYN << "  [" << i << "] : " << _fds[i].fd << RST<< std::endl;
					std::cout << CYN <<"  _clients[_fds[i].fd].get_cgi_input_fd() : " << _clients[_fds[i].fd].get_cgi_input_fd() <<RST<< std::endl;
					// std::cout << CYN <<"_clients[i]._webserv_pipe[0] : " << _clients[i]._webserv_pipe[0] <<RST<< std::endl;
					// aff_fds();

					close_conn = _clients[_fds[i].fd].send_response(_fds[i].fd);

					int target_fd = 0;
					for (; target_fd < _fds.size(); ++target_fd)
					{
						if (_fds[target_fd].fd == _clients[_fds[i].fd].get_cgi_input_fd())
							break ;
					}
					if (target_fd == _fds.size())
					{
						std::cout << YEL << "Error : target == _fds.size(). fd deleted before " << std::endl;
						exit(188);
					}
					// std::cout << CYN << "target_fd : " << target_fd << RST<< std::endl;

					// ! need to find a way to keep the client connexion alive without infinite loop
					remove_client(i);

					// _clients[_fds[i].fd].clean_cgi();
					// _clients[_fds[i].fd] = Client();
					// _fds[i].events = 0;
					// _fds[i].revents = 0;
					// _fds.erase(_fds.begin() + target_fd);

					--i;

				}
				else
				{
					std::cout << YEL << "non client getting POLLOUT" <<RST<< std::endl;
					_fds.erase(_fds.begin() + i);
				}
			}
			__DEB("end of loop")
		}
	}

	return (true);
}

void			sig_handler(int sig)
{
	run = false;
	return ;
}

/**
 * @brief This function starts the Server
 * 
 * @return int returns 0 on success, else a error code is returned.
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

	// ? Set up the initial listening socket
	tmp.fd = _listen_sd;
	tmp.events = POLLIN;
	tmp.revents = 4;
	_fds.push_back(tmp);

	try
	{
		while (run && server_poll_loop() == true)
			;
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	_clients.clear();

	return (0);
}

void			Server::aff_fds() const
{
	std::cout << "nb fds : " << _fds.size() << std::endl;
	for (int i = 0; i < _fds.size(); ++i)
		std::cout << i << " : " << _fds[i].fd << std::endl;
}

void			Server::aff_clients() const
{
	std::cout << "nb clients : " << _clients.size() << std::endl;
	for (auto cit = _clients.begin(); cit != _clients.end(); ++cit)
		std::cout << cit->first << " : " << cit->second.get_cgi_input_fd() << std::endl;
}
