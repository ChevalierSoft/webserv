/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljurdant <ljurdant@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:56:10 by ljurdant          #+#    #+#             */
/*   Updated: 2022/02/11 13:56:12 by ljurdant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

static bool	run = true;

void	                *routine(void *args);
std::vector<Conf>   	*get_confs(const Conf &current, const std::vector<Conf> &confs);