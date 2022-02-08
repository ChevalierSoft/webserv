/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 04:55:37 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/19 15:52:55 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// # include <sys/time.h>		// gettimeofday
# include "Server.hpp"
# include "color.h"
# include "Conf.hpp"
# include "Parser.hpp"
# include <vector>
# include <pthread.h>

std::string			directory_listing (std::string path);
void	            *routine(void *args);

std::vector<Conf>   	*get_confs(const Conf &current, const std::vector<Conf> &confs);
//  {
//     std::vector<T>	output(1, current);
			
//     //loop to make vector of confs with same host:ip
//     for (typename std::vector<T>::iterator it2 = confs.begin(); it2 != confs.end(); it2++)
//         if (it2->_hosts == current._hosts && it2->_names != current._names)
//             output.push_back(*it2);
//     return (output);
// }