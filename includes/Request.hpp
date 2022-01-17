#pragma once

#include "Message.hpp"

class Request : public Message {

public:
	Request() : Message() {}

	~Request() {}
	
	Request(const Request & src) : Message(src) {}
	
	Request &	operator=(const Request & src) {
		return *this;
	}
};
