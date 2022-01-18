#ifndef TYPING_H
# define TYPING_H

# include <string>
# include <map>
# include <vector>

/* Conf Class*/
typedef std::string                     name_type;
typedef std::string                     host_type;
typedef int                             port_type;
typedef std::string                     file_type;
typedef size_t                          code_type;
typedef std::pair<code_type, file_type> error_type;
typedef	std::map<code_type, file_type>	error_list;
typedef size_t                          size_type;
typedef std::string						method_type;
typedef std::vector<method_type>		method_list;
typedef short                           dir_listing_type;
typedef std::string                     path_type;
typedef class Route                     route_type;
typedef std::vector<route_type>			route_list;
typedef bool                            errno_type;
typedef std::string                      error_message_type;

/*Route Class*/
typedef std::pair<code_type, path_type>	redir_type;
typedef std::pair<name_type, path_type> cgi_type;
typedef std::map<name_type, path_type>	cgi_list;

#endif