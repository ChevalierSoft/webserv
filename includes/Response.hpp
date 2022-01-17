#pragma once

#include "Message.hpp"

class Response : public Message {

public:
	Response() : Message() {}

	~Response() {}
	
	Response(const Response & src) : Message(src) {}
	
	Response &	operator=(const Response & src) {
		return *this;
	}
};
