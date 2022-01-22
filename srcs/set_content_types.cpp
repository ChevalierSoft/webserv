/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_content_types.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/22 08:19:54 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/22 08:29:15 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include <string>

std::map<std::string, std::string>	set_content_types()
{
	std::map<std::string, std::string> ct;

	ct.insert(std::make_pair(".html", "text/html"));
	ct.insert(std::make_pair(".css", "text/css"));
	return (ct);
}
