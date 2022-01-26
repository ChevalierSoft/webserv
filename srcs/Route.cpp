
#include "Route.hpp"
#include <iostream>

Route::Route():
_path(path_type()), 
_methods(method_list()), 
_dir_listing(-1), 
_upload_path(path_type()), 
_cgi(cgi_list()),
_error_message(std::string()), 
_err(0){

}
Route::Route(const path_type path, method_list methods, dir_listing_type dir_listing, path_type upload_path, cgi_list cgi):
_path(path), 
_methods(methods), 
_dir_listing(dir_listing), 
_upload_path(upload_path),
_cgi(cgi),
_error_message(std::string()), 
_err(0) {
}

Route::Route(Route const &copy): _path(copy._path) { 
    *this = copy; 
}

Route 	&Route::operator=(const Route &rhs) {
    _methods = rhs._methods;
    _redir = rhs._redir;
    _location = rhs._location;
    _default_file = rhs._default_file;
    _dir_listing = rhs._dir_listing;
    _upload_path = rhs._upload_path;
    _cgi = rhs._cgi;
    return (*this);
}

Route::~Route(){}

bool	Route::add_method(method_type method) {
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (set_error_message("Invalid value: methods"));
	_methods.push_back(method);
	return (true);
}

bool	Route::set_methods(method_list methods) {
	_methods.clear();
    for (method_list::iterator it = methods.begin(); it != methods.end(); it++)
		if (!add_method(*it))
				return (false);
	return (true);
}

bool    Route::set_location(path_type location) {
	if (location == "")
		return(set_error_message("Invalid value: location"));
	_location = location;
	return (true);
}

bool    Route::set_default_file(file_type file) {
	if (file == "")
		return(set_error_message("Invalid value: file"));
	_default_file = file;
	return (true);
}

bool    Route::set_dir_listing(dir_listing_type dir_listing) {
	if (dir_listing < 0)
		return (set_error_message("Invalid value: directory_listing"));
	_dir_listing = dir_listing;
	return (true);
}

bool    Route::set_upload_path(path_type upload_path) {
	if (upload_path == "")
		return(set_error_message("Invalid value: uploads"));
	_upload_path = upload_path;
	return (true);
}

bool    Route::set_redir(redir_type redir) {
	if (!(redir.first >= 300 && redir.first <= 308)|| redir.second == "")
		return(set_error_message("Invalid value: redirections"));
	_redir = redir;
	return (true);
}

bool	Route::add_cgi(file_type cgi) {
	if (cgi == "")
		return (set_error_message("Invalid value: cgi"));
	_cgi.push_back(cgi);
	return (true);
}

bool	Route::set_cgi(cgi_list cgi) {
	_cgi.clear();
    for (cgi_list::iterator it = cgi.begin(); it != cgi.end(); it++)
		if (!add_cgi(*it))
				return (false);
	return (true);
}

void	Route::print() {
	std::cout << "\tpath = " << _path << std::endl;
	std::cout << "\tmethods = ";
	for (method_list::iterator it = _methods.begin(); it != _methods.end(); it++)
	{
		if  (it != _methods.begin())
			std::cout << ", ";
		std::cout << *it;
	}
	std::cout << std::endl;
    std::cout << "\tredirection:" << std::endl << "\t\t" << _redir.first << ": " << _redir.second << std::endl;
	std::cout << "\tlocation = " << _location << std::endl;
	std::cout << "\tfile = " << _default_file << std::endl;
	std::cout << "\tdirectory listing = " << _dir_listing << std::endl;
	std::cout << "\tuploads = " << _upload_path << std::endl;
    	std::cout << "\tmethods = ";
	for (method_list::iterator it = _cgi.begin(); it != _cgi.end(); it++)
	{
		if  (it != _cgi.begin())
			std::cout << ", ";
		std::cout << *it;
	}
	std::cout << std::endl;

}

bool		Route::set_error_message(std::string error_message) {
	_error_message = _path + ": "+error_message;
	_err = true;
	return (false);
}

bool	Route::check() {
	if (_methods == method_list())
		return (set_error_message("Required value: methods"));
	else if (_location == path_type())
		return (set_error_message("Required value: location"));
	else if (_default_file == file_type())
		return (set_error_message("Required value: file"));
	else if (_dir_listing == -1)
		return (set_error_message("Required value: directory_listing"));
	else if (_upload_path == path_type())
		return (set_error_message("Required value: upload_path"));
	else if (_cgi == cgi_list())
		return (set_error_message("Required value: cgi"));
	return (true);
}