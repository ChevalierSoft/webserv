#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_EVENTS 10
#define EXI

int main()
{
	struct epoll_event	ev, events[MAX_EVENTS];
	int					listen_sock, conn_sock, nfds, epollfd;
	struct sockaddr		addr;
	socklen_t			addrlen = sizeof(addr);

	/* Code to set up listening socket, 'listen_sock',
		(socket(), bind(), listen()) omitted */

	epollfd = epoll_create1(0);
	if (epollfd == -1) {
		perror("epoll_create1");
		return (1);
	}

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
	{
		perror("epoll_ctl: listen_sock");
		return (1);
	}

	for (;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			return (1);
		}

		for (int n = 0; n < nfds; ++n) {
			if (events[n].data.fd == listen_sock) {
				conn_sock = accept(listen_sock,
									(struct sockaddr *) &addr, &addrlen);
				if (conn_sock == -1) {
					perror("accept");
					return (1);
				}
				setnonblocking(conn_sock);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
							&ev) == -1) {
					perror("epoll_ctl: conn_sock");
					return (1);
				}
			} else {
				do_use_fd(events[n].data.fd);
			}
		}
	}
	return (0);
}