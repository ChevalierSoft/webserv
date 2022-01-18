
#ifndef ROUTE_H
# define ROUTE_H

# include "Conf.hpp"
# include "typing.hpp"

class Route {
    private:
        Route();
    public:
        typedef std::pair<code_type, path_type>	redir_type;
        typedef std::pair<name_type, path_type> cgi_type;
        typedef std::map<name_type, path_type>	cgi_list;

        const path_type		_path;
        method_list			_methods;
        redir_type			_redir;
        path_type			_root;
        file_type			_file;
        dir_listing_type	_dir_listing;
        path_type			_upload_path;
        cgi_list			_cgis;

		std::string			_error_message;
		bool				_errno;

        Route(const path_type path, method_list methods, dir_listing_type dir_listing, path_type upload_path);
        Route(Route const &copy);
        Route 	&operator=(const Route &rhs);
        ~Route();

        bool        add_method(method_type method);
        bool		set_methods(method_list methods);
        bool		set_root(path_type root);
        bool		set_file(file_type file);
        bool		set_dir_listing(dir_listing_type dir_listing);
        bool		set_upload_path(path_type upload_path);
        bool        set_redir(redir_type redir);
        bool        add_cgi(cgi_type  cgi);

        void		print();
};
#endif