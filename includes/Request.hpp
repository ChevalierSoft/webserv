#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include "color.h"
#include <cstdlib>
#include "Route.hpp"

# define BUFFER_SIZE	64

void	*ft_print_memory(void *addr, size_t size);

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
	std::string							_http_version; // from header first line
	std::string							_get_query;
	bool								_in_header;
	int									_error;

	Route	_route;
	Route::redir_type	_redir;
	
	typedef std::pair<std::string, std::string>					value_type;
	typedef std::map<std::string, std::string>::const_iterator	it_header;
	typedef std::vector<std::string>::const_iterator							it_value;
	
	Request(void) {
		clear();
		_method = std::string();
		_path = std::string();
		_http_version = std::string();
		_get_query = std::string();
		_line_index = 0;
		_body_index = 0;
		_in_header = true;
		_error = NO_ERROR;
		_content_length = -1;
	}

	virtual ~Request(void) {
		clear();
	}
	
	Request(const Request & src) {
		clear();
		_header = src._header;
		_body = src._body;
		_buffer = src._buffer;
		_line_index = src._line_index;
		_method = src._method;
		_path = src._path;
		_http_version = src._http_version;
		_in_header = src._in_header;
		_error = src._error;
		_body_index = src._body_index;
		_content_length = src._content_length;
		_get_query = src._get_query;
		_route = src._route;
		_redir = src._redir;
	}
	
	Request &	operator=(const Request & src) {
		clear();
		_header = src._header;
		_body = src._body;
		_buffer = src._buffer;
		_line_index = src._line_index;
		_method = src._method;
		_path = src._path;
		_http_version = src._http_version;
		_in_header = src._in_header;
		_error = src._error;
		_body_index = src._body_index;
		_content_length = src._content_length;
		_get_query = src._get_query;
		_route = src._route;
		_redir = src._redir;
		return *this;
	}

	/**
	 * @brief simply appends new 64 bytes packet to internal buffer
	 * 
	 * @details might remove this function
	 * 
	 * @param src 
	 */
	void		append_buffer(const std::string & src) {
		_buffer.append(src);
	}


	std::vector<std::string>	split_values(std::string::const_iterator _begin, std::string::const_iterator _end) {
		std::vector<std::string>	ret;

		while (_begin != _end) {
			std::string::const_iterator	new_str_begin = _begin;
			while (new_str_begin != _end && std::isspace(*new_str_begin))
				new_str_begin++;

			std::string::const_iterator	new_str_end = new_str_begin;
			while (new_str_end != _end && (*new_str_end) != ',')
				new_str_end++;
			if (new_str_begin != new_str_end)
				ret.push_back(std::string(new_str_begin, new_str_end));
			if ((*new_str_end) == ',')
				new_str_end++;
			_begin = new_str_end;
		}
		return ret;
	}
	/**
	 * @brief splits string into key and value by ": "
	 * 
	 * @return a pair consisting of a key and a value
	 */
	value_type	split_buffer(std::string str) {
		value_type	ret;
		int found;

		if ((found = str.find(":")) != str.npos)
			ret = value_type(std::string(str.begin(), str.begin() + found), std::string(str.begin() + found + 2, str.end()));
		return ret;
	}

	bool		valid_header(std::string str) {
		std::string test = str;
		size_t		found_info;

		while (test.size() > 0 && std::isspace(test.at(0))) // skipping whitespaces
			test.erase(0, 1);
		while (test.size() > 0 && !std::isspace(test.at(0)) && test.at(0) != ':') // skipping key
			test.erase(0, 1);
		if ((found_info = test.find(":")) != 0) // if ':' is not attached to key, error
			return false;
		test.erase(0, 1);
		while (test.size() > 0 && std::isspace(test.at(0))) // skipping whitespaces
			test.erase(0, 1);
		if (test.size() == 0) // if no value, error
			return false;
		while (test.size() > 0 && !std::isspace(test.at(0))) // skipping value
			test.erase(0, 1);
		while (!test.empty()) { // checking for multiple values preceded by a ','
			if (test.size() > 0 && test.at(0) == ',')
				test.erase(0, 1);
			while (test.size() > 0 && std::isspace(test.at(0))) // skipping whitespaces
				test.erase(0, 1);
			if (test.size() == 0) // if no value, error
				return false;
			while (test.size() > 0 && !std::isspace(test.at(0))) // skipping value
				test.erase(0, 1);
		}
		return true;
	}
	
	int	cl_error(std::string cl) {
		std::string::iterator it = cl.begin();
		while (it != cl.end()) {
			if (!std::isdigit(*it))
				return (1);
			it++;
		}
		return (0);
	}
	/**
	 * @brief updates header map with buffer contentt
	 * 
	 * @details checks for "\r\n\r\n" (signifies separation between header and body and end of file)
	 * @details checks for "\r\n" (new line)
	 * @details adds recorded keys and values to header map
	 * 
	 * @return 2 if there is no body
	 * @return 1 to indicate that separation between header and body has been found
	 * @return 0, otherwise 
	 * @return -1 for errors
	 */
	int		update_header() {
		size_t		found_newline;
		std::string	new_str;

		found_newline = _buffer.find("\r\n");
		if (_line_index == 0 && found_newline != _buffer.npos && found_newline > 0)
			return (get_first_line(found_newline));
		else if (found_newline != _buffer.npos && found_newline > 0) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_newline);
			if (!valid_header(new_str)) {
				_error = WRONG_LINE_HEADER;
				return (2);
			}
			_header.insert(split_buffer(new_str));
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			_line_index++;
			return (0);
		}
		else if (found_newline == 0) {
			_buffer.erase(_buffer.begin(), _buffer.begin() + 2);
			_line_index++;
			it_header cl_key = _header.find("Content-Length");
			if (cl_key != _header.end() && !cl_error((*cl_key).second))
				_content_length = std::atoi(((*cl_key).second).c_str());
			_in_header = false;
		}
		return (1);
	}
	

	void	clear() {
		_header.clear();
		_body.clear();
		_buffer.clear();
		_method.clear();
		_path.clear();
		_get_query.clear();
		_http_version.clear();
		_line_index = 0;
		_body_index = 0;
		_in_header = true;
		_error = NO_ERROR;
		_content_length = -1;
	}

	/**
	 * @brief updates body vector with buffer content
	 * 
	 * @return 2 if "\r\n\r\n" (end of file) is found
	 * @return 1 if "\r\n" (newline) is found
	 * @return 0 otherwise
	 */
	int		update_body() {
		int			found_newline;
		std::string	new_str;

		found_newline = _buffer.find("\r\n");
		if (_content_length != -1) {
			if (_buffer.size() >= _content_length) {
				new_str = std::string(_buffer.begin(), _buffer.begin() + _content_length);
				_body.push_back(new_str);
				_buffer.erase(new_str.size());
				return (2);
			}
			return (0);
		}
		else if (found_newline != _buffer.npos && found_newline > 0) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_newline);
			_body.push_back(new_str);
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			_body_index += found_newline + 2;
			_line_index++;
			return (1);
		}
		else if (found_newline == 0)
			return (2);
		return (0);
	}

	int			get_first_line(size_t found_newline) {
		while (_buffer.size() > 0 && std::isspace(_buffer.at(0)))
			_buffer.erase(0, 1);
		if (_buffer.find("GET") == 0)
			_method = "GET";
		else if (_buffer.find("POST") == 0)
			_method = "POST";
		else if (_buffer.find("DELETE") == 0)
			_method = "DELETE";
		else {
			_error = UNDEFINED_METHOD;
			return (2);
		}
		_buffer.erase(0, _method.size());
		while (_buffer.size() > 0 && std::isspace(_buffer.at(0)))
			_buffer.erase(0, 1);
		if (_buffer.find("/") == 0) {
			std::string::iterator end_path = _buffer.begin();
			while (end_path != _buffer.end() && !(std::isspace(*end_path)) && (*end_path) != '?')
				end_path++;
			_path = std::string(_buffer.begin(), end_path);
			_buffer.erase(_buffer.begin(), end_path);
			if (_buffer.find("?") == 0) {
				_buffer.erase(_buffer.begin(), _buffer.begin() + 1);
				end_path = _buffer.begin();
				while (end_path != _buffer.end() && !(std::isspace(*end_path))) {
					std::cout << "\'" << *end_path << "\'" << " ";
					end_path++;
				}
				_get_query = std::string(_buffer.begin(), end_path);
				_buffer.erase(_buffer.begin(), end_path);
			}
		}
		else {
			_error = UNDEFINED_PATH;
			return (2);
		}
		while (_buffer.size() > 0 && std::isspace(_buffer.at(0)))
			_buffer.erase(0, 1);
		if (_buffer.find("HTTP/1.1") == 0) {
			_http_version = "HTTP/1.1";
			_buffer.erase(0, 8);
		}
		else {
			_error = UNDEFINED_HTTP_VERSION;
			return (2);
		}
		if (_buffer.find("\r\n") == 0)
			_buffer.erase(0, 2);
		_line_index++;
		return (0);
	}

	/**
	 * @brief calls header map begin
	 * 
	 * @return it_header begin 
	 */
	it_header	begin_header() {
		return (_header.begin());
	}

	std::vector<std::string>::iterator	begin_body() {
		return (_body.begin());
	}

	std::vector<std::string>::iterator	end_body() {
		return (_body.end());
	}

	/**
	 * @brief calls header map end
	 * 
	 * @return it_header end 
	 */
	it_header	end_header() {
		return (_header.end());
	}

	std::string	& get_buffer() {
		return _buffer;
	}

	std::string	find_header(const std::string & key) {
		it_header it = _header.find(key);
		if (it == _header.end())
			return (std::string());
		return (*it).second;
	}

	void		d_output() const {
		it_header _it_header;
		_it_header = _header.begin();
		std::cout << GRN << "HEADER" << RST << std::endl;
		std::cout << RED << "Method: " << _method << RST << std::endl;
		std::cout << RED << "path: " << _path << RST << std::endl;
		std::cout << RED << "http-version: " << _http_version << RST << std::endl;
		std::cout << RED << "get_query: " << _get_query << RST << std::endl;
		for (; _it_header != _header.end(); _it_header++)
			std::cout << RED << (*(_it_header)).first << ": " << (*(_it_header)).second << std::endl;
		it_value it_test = _body.begin();
		std::cout << GRN << "BODY" << RST << std::endl;
		for (; it_test != _body.end(); it_test++)
			std::cout << RED << *it_test << RST << std::endl;
	}


	int	request_error() {
		if (_error == UNDEFINED_HTTP_VERSION)
			return (505);
		else if (_error == UNDEFINED_METHOD)
			return (405);
		else if (_error == UNDEFINED_PATH || _error == WRONG_LINE_HEADER || _error == WRONG_VALUE_HEADER)
			return (400);
		else if (_method == "POST" && _content_length < 0)
			return (411);
		return (0);
	}

	bool	is_upload() {
		std::string	ct = find_header("Content-Type");
		size_t		found_ct = ct.find("multipart/form-data");
		size_t		found_bound = ct.find("boundary=");
		if (_method == "POST" && found_ct != -1 && found_bound != -1)
			return (true);
		return (false);
	}

	bool	upload_to_server(const Conf * conf) {
		std::string	filename("default_name");
		std::string	file_content = *(_body.begin());
		std::string	boundary(find_header("Content-Type"));
		size_t		found_bound = boundary.find("boundary=");
		size_t		found_info;
		size_t		content_length = _content_length;
		
		// ? Find boundary for the file contents
		boundary.erase(0, found_bound + 9);
		boundary.insert(0, "--");
		// ? Skip first boundary
		if (file_content.find(boundary) == 0)
			file_content.erase(0, boundary.size() + 2);
		else
			return (false);
		// ? Skip mime types and get filename
		if (file_content.find("Content-Disposition") != -1 && (found_info = file_content.find("filename=")) != -1) {
			filename = std::string(file_content, found_info + 10);
			file_content.erase(0, found_info + 10);
			found_info = filename.find("\"");
			filename.erase(found_info, filename.size() - found_info);
			file_content.erase(0, filename.size() + 3);
		}
		if (file_content.find("Content-Type") != -1 && (found_info = file_content.find("\r\n")) != -1) {
			file_content.erase(0, found_info + 4);
		}
		// ? Get file contents until final boundary with extra "--"
		if ((found_info = file_content.find(boundary)) != -1) {
			file_content.erase(found_info, boundary.size() + 4);
		}
		else
			return false;
		file_content.erase(file_content.size() - 2, 2);

		std::ofstream new_file((conf->_upload_path + "/" + filename).c_str());
		new_file << file_content;
		new_file.close();
		return (true);
	}
};
