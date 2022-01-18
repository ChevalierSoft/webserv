/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/18 15:31:48 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/18 15:54:58 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool	send_403 (int fd)
{
	std::cout << "send 403 page" << std::endl;
	std::string	page;

	page = "HTTP/1.1 403 OK\r\n";
	page += "webser:42\r\n\r\nContent-Length: 120\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	page += "<html><h1>403</h1>\r\n";
	write(fd, page.c_str(), page.size());
	return (false);
}

/**
 * @brief send to the client a html file containing the listing of the path dir
 * 
 * @param fd the client fd
 * @param path where the listing should be done
 * @param client_path the path to give to the client (avoiding to give them the real path)
 * @return true listing done
 * @return false an error occured
 */
bool	directory_listing (int fd, const char *path, const char *client_path)
{
	struct dirent	*entry;
	DIR				*dir = opendir(path);
	std::string		header = "";
	std::string		body = "";

	if (dir == NULL)
		return (send_403(fd));


	while ((entry = readdir(dir)) != NULL)
	{
		body += "<p><li><a href=\"";
		body += entry->d_name;
		body += "\">";
		body += entry->d_name;
		body += "</a></p>";
	}

	body += "<p>Webserv 42 oui</p>";

	header = "HTTP/1.1 200 OK\r\n";
	header += "webser:42\r\n\r\nContent-Length: ";
	header += body.size();
	header += "\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	header += body;

	write (fd, header.c_str(), header.size());

	closedir(dir);

	return (true);
}

int main (void)
{
	int fd;

	fd = open("index.html", O_CREAT | O_WRONLY, 0666);
	return (directory_listing(fd, "../../", "/"));
}
