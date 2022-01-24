#pragma once

#include "Message.hpp"

class Response : public Message {
private:
	size_t	_buffer_index;

public:
	Response() : Message(), _buffer_index(0) {}

	~Response() {}
	
	Response(const Response & src) : Message(src), _buffer_index(src._buffer_index) {}
	
	Response &	operator=(const Response & src) {
		*this = src;
		this->_buffer_index = src._buffer_index;
		return *this;
	}

	bool	send_itself(int sd_out) {
		int		rc;
		size_t	char_read = BUFFER_SIZE;
		std::cout << "response n°" << (void *)this << std::endl;
		
		if (_buffer.size() > char_read)
			char_read = _buffer.size();

		rc = send(sd_out, _buffer.c_str() + _buffer_index, char_read, 0);
		if (rc < 0)
		{
			perror("  send() failed");
			return (true);
		}
		_buffer_index += char_read;
		if (_buffer_index == _buffer.size())
			return (true); // finished sending response
		return (true);
	}
};
