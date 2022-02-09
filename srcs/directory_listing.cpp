/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/19 15:02:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 16:08:32 by lpellier         ###   ########.fr       */
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
 * @brief send to the client a html file containing the listing of the root dir
 * 
 * @param fd the client fd
 * @param root where is located the site
 * @param path where the listing should be done
 * //@param client_path the root_path to give to the client (avoiding to give them the real path)
 * @return true listing done
 * @return false an error occured
 */
std::string		directory_listing (std::string &path_raw, std::string &path)	// , const char *client_path)
{
	struct dirent	*entry;
	DIR				*dir;
	std::string		page = "";
	std::string		body = "";
	std::string		tmp;

	// ? DEBUG
	// std::cout << "root : " << root << std::endl;
	// std::cout << "path : " << path << std::endl;
	// std::cout << (root + path).c_str() << std::endl;

	dir = opendir((path).c_str());
	if (dir == NULL)
		return (send_403());
	if (*(path_raw.end() - 1) != '/')
			path_raw+="/";

	body += "<!DOCTYPE html>\r\n";
	body += "<meta charset=\"UTF-8\">\r\n";
	body += "<html>\r\n";
	body += "<style>\r\n";
	body += "	html   { background-color: #000; }";
	body += "	header { min-height:100%; height:100%; width: 100%; display: inline-block; color: #fff; text-align: center; position: relative; top:0px; overflow: hidden!important;}\r\n";
	body += "	body   { width: 100%;  color: #fff; margin: 0; overflow:auto; }\r\n";
	body += "	ul     { background-color: #222; padding 5%; }";
	body += "	li     { margin-left: 20%; padding: 8px }\r\n";
	body += "	a      { color: cyan; }\r\n";
	body += "	footer { padding: 10px; width: 100%; color: #fff; height: 50px; position:absolute; position:relative; bottom: 0; text-align: center; display: inline-block;}\r\n";
	body += "</style>\r\n";
	body += "<body>\r\n";
	body += "<header>\r\n	<h2>Index of ";
	body += path_raw;	// client_path;
	body += "</h2>\r\n</header>\r\n<ul>";

	while ((entry = readdir(dir)) != NULL)
	{
		if (!strcmp(entry->d_name, "."))
			;
		else if (!strcmp(entry->d_name, ".."))
		{
			body += "<li><a href=\"";			
			// body += std::filesystem::path_raw("foo/bar").remove_filename();	// ? c++17
			tmp = path_raw;
			tmp[tmp.rfind('/')] = '\0';
			tmp[tmp.rfind('/')] = '\0';
			if (tmp.empty() || tmp[0] == '\0')
				tmp = "/";
			body += tmp.c_str();
			body += "\">⬆️ Parent directory</a></li>";
		}
		else
		{
			body += "<li><a href=\"";
			body += path_raw;
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
			body += "</a></li>\r\n";
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