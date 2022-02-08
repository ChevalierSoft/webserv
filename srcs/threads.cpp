
#include "webserv.hpp"

void	*routine(void *args) {
	std::vector<Conf> confs = *(static_cast<std::vector<Conf> *>(args));
	Server	s(confs.front());
	int		*err = new int;
	
	s._conf_list = confs;
	*err = s.start();
	return (err);
}

std::vector<Conf>   *get_confs(const Conf &current, const std::vector<Conf> &confs) {
    std::vector<Conf>	*output = new std::vector<Conf>;
    
    output->push_back(current);
	
    //loop to make vector of confs with same host:ip
    for (std::vector<Conf>::const_iterator it = confs.begin(); it != confs.end(); it++)
    {
        if (it->_name != current._name)
        {
            for (Conf::host_list::const_iterator it2 = it->_hosts.begin(); it2 != it->_hosts.end(); it2++)
                if (*it2 == current._hosts.front())
                    output->push_back(*it);
        }
    }
    return (output);
}