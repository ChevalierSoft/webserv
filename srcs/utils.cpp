/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 17:07:56 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/26 11:14:40 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <map>

/**
 * @brief Get the file name from a link.
 * 
 * @param path link or url to a file. It needs to be a file link.
 * @return std::string the file name.
 */
std::string					get_file_name (const std::string & path)
{
	return (path.substr(path.find_last_of("/\\") + 1));
}

/**
 * @brief Get the flie extention from a file name.
 * 
 * @param file_name 
 * @return std::string the extention of file_name
 */
std::string					get_file_extention (const std::string & file_name)
{
	std::string::size_type	position(file_name.find_last_of('.'));
	return (file_name.substr(position, file_name.size()));
}

/**
 * @brief Set the error map object
 * @details https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
 * @return std::map<int, std::string>
 */
std::map<int, std::string>	set_error_map ()
{
	std::map<int, std::string>	m;

	// ? Information responses
	m[100] = "Continue";
	m[101] = "Switching Protocols";
	m[102] = "Processing";
	m[103] = "Early Hints";

	// ? Successful responses
	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[203] = "Non-Authoritative Information";
	m[204] = "No Content";
	m[205] = "Reset Content";
	m[206] = "Partial Content";
	m[207] = "Multi-Status";
	m[208] = "Already Reported";
	m[226] = "IM Used";

	// ? Redirection messages
	m[300] = "Multiple Choice";
	m[301] = "Moved Permanently";
	m[302] = "Found";
	m[303] = "See Other";
	m[304] = "Not Modified";
	m[305] = "Use Proxy";
	m[306] = "unused";
	m[307] = "Temporary Redirect";
	m[308] = "Permanent Redirect";

	// ? Client error responses
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[402] = "Payment Required";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[406] = "Not Acceptable";
	m[407] = "Proxy Authentication Required";
	m[408] = "Request Timeout";
	m[409] = "Conflict";
	m[410] = "Gone";
	m[411] = "Length Required";
	m[412] = "Precondition Failed";
	m[413] = "Payload Too Large";
	m[414] = "URI Too Long";
	m[415] = "Unsupported Media Type";
	m[416] = "Range Not Satisfiable";
	m[417] = "Expectation Failed";
	m[418] = "I'm a teapot";
	m[421] = "Misdirected Request";
	m[422] = "Unprocessable Entity";
	m[423] = "Locked";
	m[424] = "Failed Dependency";
	m[425] = "Too Early";
	m[426] = "Upgrade Required";
	m[428] = "Precondition Required";
	m[429] = "Too Many Requests";
	m[431] = "Request Header Fields Too Large";
	m[451] = "Unavailable For Legal Reasons";

	// ? Server error responses
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[502] = "Bad Gateway";
	m[503] = "Service Unavailable";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP Version Not Supported";
	m[506] = "Variant Also Negotiates";
	m[507] = "Insufficient Storage";
	m[508] = "Loop Detected";
	m[510] = "Not Extended";
	m[511] = "Network Authentication Required";

	return (m);
}