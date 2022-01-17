#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <cstring>

class Message {

protected:
	std::map<std::string, std::string>	_header;
	std::vector<std::string>			_body;
	std::string							_buffer;

public:
	typedef std::pair<std::string, std::string>					value_type;
	typedef std::map<std::string, std::string>::const_iterator	it_chunk;
	
	Message(void) {}

	virtual ~Message(void) {}
	
	Message(const Message & src) {}
	
	Message &	operator=(const Message & src) {
		return *this;
	}

	/**
	 * @brief appends 64 byte string to buffer
	 * 
	 * @param src 
	 * @return 1 if contains /r/n to indicate end of line
	 * @return 0 otherwise
	 */
	void		append_buffer(const std::string & src) {
		_buffer.append(src);
	}

	/**
	 * @brief splits buffer into key and value
	 * 
	 * @return a pair consisting of a key and a value
	 */
	value_type	split_buffer(std::string str) {
		value_type	ret;
		int found;

		if ((found = str.find(":")) != str.npos) {
			ret = value_type(std::string(str.begin(), str.begin() + found), std::string(str.begin() + found + 2, str.end()));
			std::cout << "header : " << GRN << ret.first << ": " << ret.second << RST << std::endl;
		}
		return ret;
	}
	
	/**
	 * @brief adds new pair to header
	 * 
	 * @param _pair to be added to header 
	 * @return 1 if "\r\n\r\n" is found (meaning that we'll be updating body from now on)
	 * @return 0 otherwise and keep updating header
	 */
	int		update_header() {
		int			found_body;
		int			found_newline;
		std::string	new_str;

		found_body = _buffer.find("\r\n\r\n");
		found_newline = _buffer.find("\r\n");
		if (found_body != _buffer.npos && found_body < found_newline) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_body);
			_header.insert(split_buffer(new_str));
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_body + 4);
			return (1);
		}
		else if (found_newline != _buffer.npos) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found_newline);
			_header.insert(split_buffer(new_str));
			_buffer.erase(_buffer.begin(), _buffer.begin() + found_newline + 2);
			return (0);
		}
		return (1);
		// else
		// 	return (1);
	}

	/**
	 * @brief adds new pair to body
	 * 
	 * @param _pair to be added to body 
	 */
	int		update_body() {
		int			found;
		std::string	new_str;

		if ((found = _buffer.find("\r\n")) != _buffer.npos) {
			new_str = std::string(_buffer.begin(), _buffer.begin() + found);
			std::cout << "body : " << new_str << std::endl;
			_body.push_back(new_str);
			_buffer.erase(_buffer.begin(), _buffer.begin() + found + 2);
			return (1);
		}
		else {
			// new_str = std::string(_buffer.begin(), _buffer.begin() + found);
			// std::cout << "body : ";
			// _body.insert(split_buffer(new_str));
			// _buffer.clear();
			return (0);
		}
	}

	/**
	 * @brief calls header map begin
	 * 
	 * @return it_chunk begin 
	 */
	it_chunk	begin_header() {
		return (_header.begin());
	}

	/**
	 * @brief calls header map end
	 * 
	 * @return it_chunk end 
	 */
	it_chunk	end_header() {
		return (_header.end());
	}
};
