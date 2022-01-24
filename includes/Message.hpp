#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
// #include "ft_print_memory.hpp"

# define BUFFER_SIZE	64

void	*ft_print_memory(void *addr, size_t size);

// TODO handle errors from requests
// TODO is a line correct ? (first line is method, path, and http version ; rest of header is "key: value")
// ? Message syntax
// ? http-version : two digits separated by a period '.'

enum errors {
	NO_ERROR,
	UNDEFINED_METHOD,
	UNDEFINED_PATH,
	UNDEFINED_HTTP_VERSION,
	WRONG_LINE_HEADER,
	WRONG_VALUE_HEADER
};

class Message {

protected:
	std::map<std::string, std::string>	_header; // header contents stored in a map of key-values
	std::vector<std::string>			_body; // body contents stored in a vector of strings

	std::string							_buffer; // internal buffer to keep track of packets
	size_t								_line_index; // index that lets me know current line in request

public:
	std::string							_method; // from header first line
	std::string							_path; // from header first line
	std::string							_http_version; // from header first line
	bool								_in_header;
	int									_error;
	
	typedef std::pair<std::string, std::string>					value_type;
	typedef std::map<std::string, std::string>::const_iterator	it_chunk;
	
	Message(void) {
		clear();
		_line_index = 0;
		_in_header = true;
		_error = NO_ERROR;
	}

	virtual ~Message(void) {
		clear();
	}
	
	Message(const Message & src) {
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
	}
	
	Message &	operator=(const Message & src) {
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

	/**
	 * @brief splits string into key and value by ": "
	 * 
	 * @return a pair consisting of a key and a value
	 */
	value_type	split_buffer(std::string str) {
		value_type	ret;
		int found;

		if ((found = str.find(":")) != str.npos) {
			ret = value_type(std::string(str.begin(), str.begin() + found), std::string(str.begin() + found + 2, str.end()));
		}
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
		// while (test.size() > 0 && !std::isspace(test.at(0))) // skipping value
		// 	test.erase(0, 1);
		// while (test.size() > 0 && std::isspace(test.at(0))) // skipping whitespaces
		// 	test.erase(0, 1);
		// if (test.size() != 0) // if there is more text after value, error
		// 	return false;
		return true;
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
		else if (found_newline != _buffer.npos) {
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			_line_index++;
			if (_buffer.empty())
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
		_http_version.clear();
		_line_index = 0;
		_in_header = true;
		_error = NO_ERROR;
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
		if (found_newline != _buffer.npos && found_newline > 0) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_newline);
			_body.push_back(new_str);
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			_line_index++;
			return (1);
		}
		else if (found_newline != _buffer.npos)
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
			while (end_path != _buffer.end() && !(std::isspace(*end_path)))
				end_path++;
			_path = std::string(_buffer.begin(), end_path);
			_buffer.erase(_buffer.begin(), end_path);
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
	 * @return it_chunk begin 
	 */
	it_chunk	begin_header() {
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
	 * @return it_chunk end 
	 */
	it_chunk	end_header() {
		return (_header.end());
	}

	std::string	& get_buffer() {
		return _buffer;
	}
};
