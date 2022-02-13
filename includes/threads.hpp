/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljurdant <ljurdant@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:56:10 by ljurdant          #+#    #+#             */
/*   Updated: 2022/02/13 15:04:48 by ljurdant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

extern pthread_mutex_t mutex;
static bool	run = true;

void	                *routine(void *args);
std::vector<Conf>   	*get_confs(const Conf &current, const std::vector<Conf> &confs);

void    mutex_print(std::ostream &o, std::string const s);