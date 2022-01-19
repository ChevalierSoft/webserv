/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/18 15:31:48 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/19 14:03:29 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <cstring>

#include "ft_to_string.hpp"

/**
 * @brief generic response for error 403
 * 
 * @param fd client fd
 * @return false 
 */
bool	send_403 (int fd)
{
	std::cerr << "send 403 page" << std::endl;
	std::string	page;

	page = "HTTP/1.1 403 Forbiden\r\n";
	page += "webser:42\r\n\r\nContent-Length: 20\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
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

	body += "<!DOCTYPE html>\r\n";
	body += "<html>";
	body += "<style>\r\n";
	body += "	header { width: 100%; background-color: #000; display: inline-block; color: #fff; text-align: center; position: relative; top:0px; overflow: hidden!important;}\r\n";
	body += "	body   { width: 100%; background-color: #222; color: #fff; margin: 0; }\r\n";
	body += "	ul     { margin-top: 5%; }";
	body += "	li     { margin-left: 20%; }\r\n";
	body += "	a      { color: cyan; }\r\n";
	body += "	footer { width: 100%; background-color: #000; color: #fff; height: 50px; position:absolute; bottom: 0; text-align: center; display: inline-block;}\r\n";
	body += "</style>\r\n";
	body += "<body>\r\n";
	body += "<header>\r\n	<h2>Index of ";
	body += client_path;
	body += "</h2>\r\n</header>\r\n<ul>";

	while ((entry = readdir(dir)) != NULL)
	{
		if (!strcmp(entry->d_name, "."))
			;
		else if (!strcmp(entry->d_name, ".."))
		{
			body += "<p><li><a href=\"";
			body += path;				// ? when used localy
			// body += client_path;		// ? when used through webserv to hide the real location
			body += "/..\">⬆️ Parent directory</a></li></p>";
		}
		else
		{
			body += "<p><li><a href=\"";
			body += path;				// ? when used localy
			// body += client_path;		// ? when used through webserv to hide the real location
			body += entry->d_name;
			body += "\">";

			if (entry->d_type == DT_REG)
				body += "📄 ";
			else if (entry->d_type == DT_DIR)
				body += "📂 ";
			else if (entry->d_type == DT_BLK)
				body += "💿 ";
			else if (entry->d_type == DT_CHR)
				body += "⌨️ ";
			else if (entry->d_type == DT_FIFO)
				body += "🧪 ";
			else if (entry->d_type == DT_LNK)
				body += "↪️ ";
			else if (entry->d_type == DT_SOCK)
				body += "⚗️ ";
			else
				body += "❔ ";
			body += entry->d_name;

			body += "</a></li></p>\r\n";
		}	
	}

	body += "</ul><footer>Webserv 42 oui</footer>\r\n</body>\r\n</html>\r\n";	// ? footer + </html>

	header = "HTTP/1.1 200 OK\r\n";
	header += "webser:42\r\nContent-Length: ";
	header += ft_to_string(body.size());
	header += "\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	header += body;

	write (fd, body.c_str(), body.size());			// ? when opening the html localy
	// write (fd, header.c_str(), header.size());	// ? when opening the html through webserv

	closedir(dir);

	return (true);
}

int main (void)
{
	int		fd;
	bool	err;

	fd = open("index.html", O_CREAT | O_WRONLY, 0666);
	err = directory_listing(fd, "../../", "/");
	close(fd);
	return (err);
}
