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

std::string 	directory_listing (std::string path, std::string root_path);
