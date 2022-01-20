/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/19 15:02:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/20 08:08:42 by dait-atm         ###   ########.fr       */
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
#include <stdio.h>
#include "color.h"
#include "ft_to_string.hpp"

/**
 * @brief generic response for error 403
 * 
 * @return th 
 */
std::string		send_403 ()
{
	std::cerr << "send 403 page" << std::endl;
	std::string	page;

	page = "HTTP/1.1 403 Forbidden\r\n";
	page += "webserv:42\r\nContent-Length: 30\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	page += "<html><h1>403 Forbidden</h1>\r\n";

	return (page);
}

/**
 * @brief send to the client a html file containing the listing of the path dir
 * 
 * @param fd the client fd
 * @param root where is located the site
 * @param root_path where the listing should be done
 * //@param client_path the root_path to give to the client (avoiding to give them the real path)
 * @return true listing done
 * @return false an error occured
 */
std::string		directory_listing (std::string root, std::string root_path)	// , const char *client_path)
{
	struct dirent	*entry = 0;
	DIR				*dir = 0;
	std::string		page = "";
	std::string		body = "";

	// std::cout << "root : " << root << std::endl;
	// std::cout << "path : " << root_path << std::endl;
	// std::cout << (root + root_path).c_str() << std::endl;

	dir = opendir((root + root_path).c_str());

	if (dir == NULL)
		return (send_403());

	body += "<!DOCTYPE html>\r\n";
	body += "<meta charset=\"UTF-8\">\r\n";
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
	body += root_path;	// client_path;
	body += "</h2>\r\n</header>\r\n<ul>";

	while ((entry = readdir(dir)) != NULL)
	{
		if (!strcmp(entry->d_name, "."))
			;
		else if (!strcmp(entry->d_name, ".."))
		{
			body += "<p><li><a href=\"";
			body += root_path;				// ? client_path should be used 
			body += "/..\">⬆️ Parent directory</a></li></p>";
		}
		else
		{
			body += "<p><li><a href=\"";
			body += root_path;				// ? should use client friendly path
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
	closedir(dir);
	dir = 0;

	body += "</ul><footer>Webserv 42 oui</footer>\r\n</body>\r\n</html>\r\n";	// ? footer + </html>

	page = "HTTP/1.1 200 OK\r\n";
	page += "webser:42\r\nContent-Length: ";
	page += ft_to_string(body.size());
	page += "\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	page += body;

	return (page);
}