/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 17:07:56 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/26 01:17:05 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

/**
 * @brief Get the file name from a link.
 * 
 * @param path link or url to a file. It needs to be a file link.
 * @return std::string the file name.
 */
std::string get_file_name(const std::string & path)
{
	return (path.substr(path.find_last_of("/\\") + 1));
}

/**
 * @brief Get the flie extention from a file name.
 * 
 * @param file_name 
 * @return std::string the extention of file_name
 */
std::string	get_file_extention(const std::string & file_name)
{
	std::string::size_type	position(file_name.find_last_of('.'));
	return (file_name.substr(position, file_name.size()));
}