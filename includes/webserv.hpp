/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:37 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/09 17:16:11 by lpellier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once	

# include <algorithm>
# include <cctype>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <fstream>				// ifstream
# include <iostream>
# include <list>
# include <map>
# include <sstream>
# include <string>
# include <utility>
# include <vector>

# include <arpa/inet.h>		    // inet_ntop
# include <dirent.h>
# include <errno.h>			    // errno
# include <fcntl.h>
# include <inttypes.h>
# include <limits.h>			// PATH_MAX
# include <netdb.h>			    // getnameinfo flags
# include <netinet/in.h>	    // htons, sockaddr_in6
# include <pthread.h>
# include <signal.h>			// signal,sigaction
# include <stdio.h>
# include <stdlib.h>
# include <string.h>		    // strlen
# include <sys/ioctl.h>
# include <sys/poll.h>		    // poll_wait...
# include <sys/socket.h>
# include <sys/stat.h>			// stat
# include <sys/time.h>          // gettimeofday
# include <sys/types.h>			// stat
# include <sys/un.h>			// sockaddr_un
# include <sys/wait.h>			// waitpid
# include <unistd.h>			// debug : usleep

# include "color.h"


# include "Route.hpp"
# include "Conf.hpp"
# include "Request.hpp"

# include "Client.hpp"
# include "Parser.hpp"
# include "ResponseGenerator.hpp"
# include "Server.hpp"

# include "directory_listing.hpp"
# include "ft_print_memory.hpp"
# include "ft_to_string.hpp"
# include "set_content_types.hpp"
# include "threads.hpp"
# include "utils.hpp"
