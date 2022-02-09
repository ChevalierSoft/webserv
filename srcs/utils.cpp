/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 17:07:56 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/09 17:08:49 by lpellier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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

	// "Accept-Ranges: none" in header will let us refuse any range request
	// ? Information responses
	m[100] = "Continue"; // USELESS // // ? Initial part of request received, the client should either send the rest or ignore is the request is finished
	m[101] = "Switching Protocols"; // USELESS // // ? If clients sends the "Upgrade" header field request, server responds with 101 to switch to specified protocol
	m[102] = "Processing"; // USELESS // // ? Request is received and accepted and response is being generated
	m[103] = "Early Hints"; // USELESS // // ? While server is preparing a response, allows the client to preload resources (primarily used with the Link Header)

	// ? Successful responses
	m[200] = "OK"; // DONE // // ? Perfect status code for a functioning page
	m[201] = "Created"; // TODO // ? Successful request that resulted in the create of one or multiple resources
	m[202] = "Accepted"; // TODO // ? Request accepted for processing but response is still being generated
	m[203] = "Non-Authoritative Information"; // USELESS // // ? Request successful but meta-information that's been received is different from the one on the origin server
	m[204] = "No Content"; // TODO // ? Successful request but no content available for the request
	m[205] = "Reset Content"; // USELESS // // ? User should reset the document that sent this request
	m[206] = "Partial Content"; // ONLY IF USING RANGE HEADERS // ? If only a part of the resource is requested via a Range header
	m[207] = "Multi-Status"; // USELESS // // ? If multiple resources are involved and multiple status code are appropriate
	m[208] = "Already Reported"; // USELESS // // ? Used inside the response element "DAV: propstat" but idk what the fuck is that
	m[226] = "IM Used"; // USELESS // // ? Successful GET request and the response is a representation of the result of one or multiple instance-manipulations applied to the current instance

	// ? Redirection messages
	// ? Since redirections are set by the user and handled by client, no need to do much
	m[300] = "Multiple Choice"; // ? Multiple possible responses and the user should choose one
	m[301] = "Moved Permanently"; // ? Target resource has been assigned a new permanent URL
	m[302] = "Found"; // ? URI has been changed temporarily
	m[303] = "See Other"; // ? Directs the client to get the requested resource at another URI with a GET request
	m[304] = "Not Modified"; // ? Reponse has not been modified -> helps for caching
	m[305] = "Use Proxy"; // DEPRECATED // // ? Instructs a client that is should connect to a proxy and repeat the request there
	m[306] = "Switch Proxy"; // DEPRECATED // // ? Informs the client that the subsequent requests should use the specified proxy 
	m[307] = "Temporary Redirect"; // ? Directs the client to requested resource - the request method mustn't be changed
	m[308] = "Permanent Redirect"; // ? Requested resource has beend assigned a new permanent URI

	// ? Client error responses
	m[400] = "Bad Request"; // DONE // // ? Invalid syntax in request
	m[401] = "Unauthorized"; // USELESS // // ? Server requires user authentication
	m[402] = "Payment Required"; // USELESS // // ? Very rarely used for digital payment systems
	m[403] = "Forbidden"; // DONE // // ? Client does not have rights to access the content 
	m[404] = "Not Found"; // DONE // // ? Resource not found or hidden from unauthorized client
	m[405] = "Method Not Allowed"; // DONE // // ? Requested method has been disabled by server
	m[406] = "Not Acceptable"; // USELESS // // ? No content found following criteria given by user agent
	m[407] = "Proxy Authentication Required"; // USELESS // // ? Client must first be authenticated by a proxy
	m[408] = "Request Timeout"; // TODO // ? Didn't receive complete request in the time that it prepared to wait
	m[409] = "Conflict"; // USELESS // // ? Request unfulfilled because of conflict with state of target resource
	m[410] = "Gone"; // USELESS // // ? Target resource has been deleted and the condition seems to be permatnent (permanent 404)
	m[411] = "Length Required"; // DONE // // ? Request rejected because it has no "Content-Length" header field
	m[412] = "Precondition Failed"; // USELESS // // ? Server does not meet one or multiple preconditions that were indicated in the request header field
	m[413] = "Payload Too Large";  // TODO // ? Request payload is larger than the server is able or willing to process
	m[414] = "URI Too Long"; // TODO // ? Request refused because target resource URI is too long
	m[415] = "Unsupported Media Type"; // USELESS // // ? Server does not support media format of requested data
	m[416] = "Range Not Satisfiable"; // ONLY IF USING RANGE HEADERS // ? Range specified in Range header field of request can't be fulfilled
	m[417] = "Expectation Failed"; // TODO // ? "Expectation" indicated by the "Expect" request header field could not be met by server
	m[418] = "I'm a Teapot"; // USELESS // // ? april fools lulz
	m[421] = "Misdirected Request"; // USELESS // // ? Client request was directed at a server not configured to produce a response
	m[422] = "Unprocessable Entity"; // USELESS // // ? Server unable to follow request due to semantic errors
	m[423] = "Locked"; // USELESS // // ? Resource being accessed is locked
	m[424] = "Failed Dependency"; // USELESS // // ? Request failed due to the failure of a previous request
	m[425] = "Too Early"; // USELESS // // ? Server not willing to risk processing a request that might be replayed
	m[426] = "Upgrade Required"; // USELESS // // ? Server unwilling to process request until client upgrades to a different protocol
	m[428] = "Precondition Required"; // USELESS // // ? Origin server requires request to be conditional
	m[429] = "Too Many Requests"; // TODO // ? User has sent too many requests
	m[431] = "Request Header Fields Too Large"; // USELESS // // ? Server unwilling to process the request because its header fields are too large
	m[451] = "Unavailable For Legal Reasons"; // USELESS // // ? User requested an illegal resource -> Server calls the cops

	// ? Server error responses
	m[500] = "Internal Server Error"; // // // ? Server encountered a situation that it's unable to handle
	m[501] = "Not Implemented"; // TODO // ? Unsupported by the server
	m[502] = "Bad Gateway"; // USELESS // // ? Server received invalid response while working as a gateway to handle the response
	m[503] = "Service Unavailable"; // USELESS // // ? Server currently not ready to handle request commonly due to maintenance
	m[504] = "Gateway Timeout"; // USELESS // // ? Server acting as a gateway could not get a response time
	m[505] = "HTTP Version Not Supported"; // DONE // // ? Version of HTTP used in the request is not supported by the server
	m[506] = "Variant Also Negotiates"; // USELESS // // ? Look up doc, I don't understand any of it
	m[507] = "Insufficient Storage"; // USELESS // // ? Method couldn't be performed on the resource because server unable to store representation
	m[508] = "Loop Detected"; // USELESS // // ? Detected infinite loop
	m[510] = "Not Extended"; // USELESS // // ? Further extentions to the request are required for the server to fulfill it
	m[511] = "Network Authentication Required"; // USELESS // // ? Indicates that the client needs to authenticate to gain network access

	return (m);
}