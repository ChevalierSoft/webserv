/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 16:44:09 by lpellier          #+#    #+#             */
/*   Updated: 2022/02/09 17:08:20 by lpellier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

# define BUFFER_SIZE	64
# define LINE_LIMIT		998

enum errors {
	NO_ERROR,
	UNDEFINED_METHOD,
	UNDEFINED_PATH,
	UNDEFINED_HTTP_VERSION,
	WRONG_LINE_HEADER,
	WRONG_VALUE_HEADER
};

class Request {

protected:
	std::map<std::string, std::string>	_header; // header contents stored in a map of key-values
	std::vector<std::string>			_body; // body contents stored in a vector of strings

	std::string							_buffer; // internal buffer to keep track of packets
	size_t								_line_index; // index that lets me know current line in request
	size_t								_body_index; // index that lets me know if body is fully read (compare to Content-Length)
	int									_content_length;

public:
	std::string							_method; // from header first line
	std::string							_path; // from header first line
	std::string							_path_raw;
	std::string							_http_version; // from header first line
	std::string							_get_query;
	bool								_in_header;
	int									_error;
	int									_conf_index;

	Route	_route;
	Route::redir_type	_redir;
	
	typedef std::pair<std::string, std::string>					value_type;
	typedef std::map<std::string, std::string>::const_iterator	it_header;
	typedef std::vector<std::string>::const_iterator							it_value;
	
	Request(void);
	~Request(void);
	Request(const Request & src);
	Request &	operator=(const Request & src);

	void						append_buffer(const std::string & src);

	std::vector<std::string>	split_values(std::string::const_iterator _begin, std::string::const_iterator _end);

	value_type					split_buffer(std::string str);

	bool						valid_header(std::string str);

	int							cl_error(std::string cl);

	int							update_header();

	void						clear();

	int							update_body();

	int							get_first_line(size_t found_newline);

	it_header					begin_header();
	it_header					end_header();

	it_value					begin_body();
	it_value					end_body();

	std::string	& 				get_buffer();

	std::string					find_header(const std::string & key);

	void						d_output() const ;

	int							request_error(const Conf & conf);

	long						body_size() const;

	int							is_upload(const Conf & conf);

	bool						not_printable(std::string str);

	bool						upload_to_server(const Conf & conf);
};
