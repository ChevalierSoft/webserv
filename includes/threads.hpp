/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: %F{207}%n%f <%F{207}%n%f@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 16:46:48 by %F{207}%n%f       #+#    #+#             */
/*   Updated: 2022/02/09 16:46:51 by %F{207}%n%f      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

void	                *routine(void *args);
std::vector<Conf>   	*get_confs(const Conf &current, const std::vector<Conf> &confs);