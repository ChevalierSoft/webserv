#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
// #include "ft_print_memory.hpp"

void	*ft_print_memory(void *addr, size_t size);

// TODO handle errors from requests
// TODO is a line correct ? (first line is method, path, and http version ; rest of header is "key: value")
// ? Message syntax
// ? http-version : two digits separated by a period '.'
// 

class Message {

protected:
	std::map<std::string, std::string>	_header; // header contents stored in a map of key-values
	std::vector<std::string>			_body; // body contents stored in a vector of strings

	std::string							_buffer; // internal buffer to keep track of packets
	size_t								_line_index; // index that lets me know current line in request
	bool								_empty_line;

public:
	std::string							_method; // from header first line
	std::string							_path; // from header first line
	std::string							_http_version; // from header first line
	
	typedef std::pair<std::string, std::string>					value_type;
	typedef std::map<std::string, std::string>::const_iterator	it_chunk;
	
	Message(void) : _line_index(0), _empty_line(false) {}

	virtual ~Message(void) {}
	
	Message(const Message & src) {}
	
	Message &	operator=(const Message & src) {
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
		size_t		found_body;
		size_t		found_newline;
		std::string	new_str;

		found_body = _buffer.find("\r\n\r\n");
		found_newline = _buffer.find("\r\n");
		// std::cout << GRN << "header" << RST << std::endl;
		// ft_print_memory((void *)(_buffer.c_str()), _buffer.size());
		// std::cout << std::endl;
		if (_line_index == 0 && found_newline != _buffer.npos && found_newline > 0)
			return (get_first_line(found_newline));
		else if (found_newline != _buffer.npos && found_newline > 0) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_newline);
			_header.insert(split_buffer(new_str));
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			_line_index++;
			return (0);
		}
		else if (found_newline != _buffer.npos) {
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			_line_index++;
			if (_buffer.size() == 0 && _empty_line == true)
				return (2);
			else if (_buffer.size() == 0)
				_empty_line = true;
			return (1);
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
		_empty_line = false;
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
		int			found_eof;
		std::string	new_str;

		found_newline = _buffer.find("\r\n");
		found_eof = _buffer.find("\r\n\r\n");
		// std::cout << GRN << "body" << RST << std::endl;
		// ft_print_memory((void *)(_buffer.c_str()), _buffer.size());
		// std::cout << std::endl;
		// std::cout << RED << (found_eof == _buffer.npos ? "eof not found" : "found") << RST << std::endl;
		if (found_eof != _buffer.npos && found_eof <= found_newline) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_eof);
			_body.push_back(new_str);
			_line_index = 0;
			_buffer.clear();
			return (2);
		}
		else if (found_newline != _buffer.npos && found_newline > 0) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_newline);
			_body.push_back(new_str);
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline);
			_line_index++;
			return (1);
		}
		return (0);
	}

	int			get_first_line(size_t found_newline) {
		size_t		found_info;

		if ((found_info = _buffer.find("GET") != _buffer.npos))
			_method = "GET";
		else if ((found_info = _buffer.find("POST")) != _buffer.npos)
			_method = "POST";
		else if ((found_info = _buffer.find("DELETE")) != _buffer.npos)
			_method = "DELETE";
		else
			return (-1);
		// need to get path by splitting line by spaces
		std::string::iterator it_begin_path = _buffer.begin() + found_info;
		while (*it_begin_path && *it_begin_path != ' ')
			it_begin_path++;
		it_begin_path++;
		std::string::iterator it_end_path = it_begin_path;
		while (*it_end_path && *it_end_path != ' ')
			it_end_path++;
		_path = std::string(it_begin_path, it_end_path);
		_http_version = std::string(it_end_path + 1, _buffer.begin() + found_newline);
		_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
		// std::cout << "method : " << GRN << _method << RST << std::endl;
		// std::cout << "path : " << GRN << _path << RST << std::endl;
		// std::cout << "http_version : " << GRN << _http_version << RST << std::endl;
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
