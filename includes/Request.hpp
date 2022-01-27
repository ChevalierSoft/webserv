#pragma once

#include "Message.hpp"
#include "Route.hpp"

class Request : public Message {

public:
	Route	_route;

	Request() : Message() {}

	~Request() {}
	
	Request(const Request & src) : Message(src) {
		_route = src._route;
	}
	
	Request &	operator=(const Request & src) {
		_route = src._route;
		return *this;
	}
};
