/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_content_types.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: %F{207}%n%f <%F{207}%n%f@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/22 08:19:54 by dait-atm          #+#    #+#             */
/*   Updated: 2022/02/09 16:45:23 by %F{207}%n%f      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#define REFERENCE_CONTENT_TYPE "./resources/content-types.webserv"

/**
 * @brief Set a map with commun content types
 * @details Commun content types where found here
 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
 * @return std::map<std::string, std::string> 
 */
std::map<std::string, std::string>	set_content_types()
{
	std::map<std::string, std::string>	ct;
	std::ifstream						ifs(REFERENCE_CONTENT_TYPE);
	std::string							key, value;

	while (ifs.good())
	{
		ifs >> key >> value;
		ct.insert(std::make_pair(key, value));
	}
	return (ct);
}
