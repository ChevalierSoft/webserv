#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>

#define MAX_EVENTS	10
#define PORT		8080
#define BUFFER_SIZE	400

//Set socket connection to non blocking mode
void setnonblocking(int sockfd)
{
	int opts;
	opts = fcntl(sockfd, F_GETFL);
	if(opts < 0)
	{
		perror("fcntl(F_GETFL)\n");
		exit(1);
	}
	opts = (opts | O_NONBLOCK);
	if(fcntl(sockfd, F_SETFL, opts) < 0)
	{
		perror("fcntl(F_SETFL)\n");
		exit(1);
	}
}

int main()
{
	struct epoll_event	ev, events[MAX_EVENTS]; // EV is responsible for adding events, and events receives the returned events
	int					listen_sd, conn_sock, nfds, epfd, fd, i, nread, n, on = 1;
	socklen_t			addrlen;
	struct sockaddr_in	local, remote;
	char				buf[BUFFER_SIZE];			// BUFSIZ = 8292 (default buffer size)

	//Create listen socket
	if( (listen_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("sockfd\n");
		exit(1);
	}

	// setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));	// to allow sd to be reusable

	setnonblocking(listen_sd);	// Listen_sd set to non blocking [1]
	bzero(&local, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);;
	local.sin_port = htons(PORT);
	if (bind(listen_sd, (struct sockaddr *) &local, sizeof(local)) < 0)
	{
		perror("bind\n");
		exit(1);
	}
	listen(listen_sd, 20);

	epfd = epoll_create(MAX_EVENTS);
	if (epfd == -1)
	{
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = listen_sd;
	if (epoll_ctl (epfd, EPOLL_CTL_ADD, listen_sd, &ev) == - 1)
	{
		// listen to listen_sd
		perror("epoll_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}

	for (;;)
	{
		nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds == -1)
		{
			perror("epoll_pwait");
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < nfds; ++i)
		{
			fd = events[i].data.fd;
			if (fd == listen_sd)
			{
				while ((conn_sock = accept(listen_sd,(struct sockaddr *) &remote, &addrlen)) > 0)
				{
					setnonblocking(conn_sock);// Set et mode below, so set non blocking
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = conn_sock;
					if (epoll_ctl (epfd, EPOLL_CTL_ADD, conn_sock, &ev) == - 1) 
					{
						// read listening
						perror("epoll_ctl: add"); // connected socket 
						exit(EXIT_FAILURE);
					}
				}
				if (conn_sock == -1)
				{
					if (errno != EAGAIN && errno != ECONNABORTED
							&& errno != EPROTO && errno != EINTR)
						perror("accept");
				}
				continue;
			}
			if (events[i].events & EPOLLIN)
			{
				n = 0;
				while ((nread = read (fd, buf + n, BUFFER_SIZE - 1)) > 0)
				{
					std::cerr << buf;
					n += nread;
				}
				if (nread == -1 && errno != EAGAIN)
				{
					perror("read error");
				}
				ev.data.fd = fd;
				ev.events = events[i].events | EPOLLOUT; //MOD OUT
				if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
				{
					perror("epoll_ctl: mod");
				}
			}
			if (events[i].events & EPOLLOUT)
			{
				int		nwrite, data_size = strlen(buf);
				sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World\r\n", 13);
				n = data_size;
				while (n > 0)
				{
					nwrite = write(fd, buf + data_size - n, n);	// The data to be written under et has been written
					if (nwrite < n)
					{
						if (nwrite == -1 && errno != EAGAIN)
						{
							perror("write error");
						}
						break;
					}
					n -= nwrite;
				}
				close(fd);
			}
		}
	}
	return 0;
}