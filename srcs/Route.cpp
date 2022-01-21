
#include "Route.hpp"
#include <iostream>

Route::Route(const path_type path, method_list methods, dir_listing_type dir_listing, path_type upload_path):
_path(path), _methods(methods), _dir_listing(dir_listing), _upload_path(upload_path) {
}

Route::Route(Route const &copy): _path(copy._path) { 
    *this = copy; 
}

Route 	&Route::operator=(const Route &rhs) {
    _methods = rhs._methods;
    _redir = rhs._redir;
    _root = rhs._root;
    _file = rhs._file;
    _dir_listing = rhs._dir_listing;
    _upload_path = rhs._upload_path;
    _cgis = rhs._cgis;
    return (*this);
}

Route::~Route(){}

bool	Route::add_method(method_type method) {
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

bool    Route::set_root(path_type root) {
	_root = root;
	return (true);
}

bool    Route::set_file(file_type file) {
	_file = file;
	return (true);
}

bool    Route::set_dir_listing(dir_listing_type dir_listing) {
	_dir_listing = dir_listing;
	return (true);
}

bool    Route::set_upload_path(path_type upload_path) {
	_upload_path = upload_path;
	return (true);
}

bool    Route::set_redir(redir_type redir) {
	_redir = redir;
	return (true);
}

bool    Route::add_cgi(cgi_type  cgi) {
	_cgis.insert(cgi);
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
	std::cout << "\troot = " << _root << std::endl;
	std::cout << "\tfile = " << _file << std::endl;
	std::cout << "\tdirectory listing = " << _dir_listing << std::endl;
	std::cout << "\tuploads = " << _upload_path << std::endl;
    std::cout << "\tcgis:" << std::endl;
	for (cgi_list::iterator it = _cgis.begin(); it != _cgis.end(); it++)
		std::cout << "\t\t" << (*it).first << ": " << (*it).second << std::endl;
    std::cout << std::endl;
}