#pragma once

#include "Message.hpp"
#include "Route.hpp"

class Request : public Message {

public:
	Route	_route;
	Route::redir_type	_redir;

	Request() : Message(),_route(Route()), _redir(Route::redir_type()) {}

	~Request() {}
	
	Request(const Request & src) : Message(src) {
		_route = src._route;
		_redir = src._redir;
	}
	
	Request &	operator=(const Request & src) {
		Message::operator=(src);
		_route = src._route;
		_redir = src._redir;
		return *this;
	}

	bool	is_upload() {
		std::string	ct = find_header("Content-Type");
		size_t		found_ct = ct.find("multiform/form-data");
		size_t		found_bound = ct.find("boundary=");
		if (_method == "POST" && found_ct != -1 && found_bound != -1) {
			
		}
	}
};
