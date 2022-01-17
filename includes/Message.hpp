#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <cstring>

class Message {

protected:
	std::map<std::string, std::string>	_header;
	std::map<std::string, std::string>	_body;
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
	// value_type	split_buffer(void) {
	// 	value_type	ret;
	// 	std::string	buffer;
	// 	int			nb_elements;
	// 	size_t		found = 0;
		
	// 	nb_elements = _buffer.size();

	// 	while (_buffer_index < nb_elements) {
	// 		if ((found = buffer.find("\r\n")) != buffer.npos) {
	// 			std::cout << "newline found" << std::endl;
	// 			break ;
	// 		}
	// 		buffer.append(_buffer.at(_buffer_index));
	// 		_buffer_index++;
	// 	}
	// 	std::string::iterator test_it = buffer.begin();
	// 	for (int i = 0; i < found; i++)
	// 		test_it++;
	// 	buffer.erase(test_it, buffer.end());
	// 	std::cout << GRN << "line is : " << buffer << RST << std::endl; 
	// 	return ret;
	// }
	
	/**
	 * @brief adds new pair to header
	 * 
	 * @param _pair to be added to header 
	 */
	void		update_header() {
		int		found;

		while ((found = _buffer.find("\r\n") != _buffer.npos)) {
			std::string new_str(_buffer.begin(), _buffer.begin() + found);
			_header.insert(value_type(new_str, new_str));
			std::cout << "new line : " << new_str << std::endl;
			_buffer.erase(_buffer.begin(), _buffer.begin() + found);
		}
	}


	/**
	 * @brief adds new pair to body
	 * 
	 * @param _pair to be added to body 
	 */
	void		update_body(value_type _pair) {
		_body.insert(_pair);
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
