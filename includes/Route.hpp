
#ifndef ROUTE_H
# define ROUTE_H
# include "Conf.hpp"

class Route {
	public:
		typedef std::string                     path_type;
		typedef int                             code_type;
		typedef std::string                     name_type;
		typedef std::string                     method_type;
		typedef std::vector<method_type>        method_list;
		typedef std::string                     file_type;
		typedef short							dir_listing_type;
		typedef std::pair<code_type, path_type>	redir_type;
		typedef std::pair<name_type, path_type>	cgi_type;
		typedef std::map<name_type, path_type>	cgi_list;

		path_type	    	_path;
		method_list	        _methods;
		redir_type          _redir;
		path_type			_location;
		file_type			_default_file;
		dir_listing_type	_dir_listing;
		path_type			_upload_path;
		cgi_list            _cgis;

		// ? where is error type???????!!! <- flemme
		std::string			_error_message;
		bool				_err;

		Route();
		Route(const path_type path, method_list methods, dir_listing_type dir_listing, path_type upload_path, cgi_list cgi);
		Route(Route const &copy);
		Route 	&operator=(const Route &rhs);
		bool	operator==(const Route &rhs) const;
		~Route();

		bool        add_method(method_type method);
		bool		set_methods(method_list methods);
		bool		set_location(path_type location);
		bool		set_default_file(file_type file);
		bool		set_dir_listing(dir_listing_type dir_listing);
		bool		set_upload_path(path_type upload_path);
		bool        set_redir(redir_type redir);
		bool        add_cgi(cgi_type  cgi);
		bool		set_cgi(cgi_list cgis);
		bool		set_error_message(std::string error_message);

		void		print();

		bool        check();
};
#endif