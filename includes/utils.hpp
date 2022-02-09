/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 17:07:53 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/26 10:47:00 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

std::string					get_file_name (const std::string & path);
std::string					get_file_extention (const std::string & file_name);
std::map<int, std::string>	set_error_map ();
