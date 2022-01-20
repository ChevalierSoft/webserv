/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_to_string.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/18 16:54:05 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/18 16:57:17 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <sstream>

template <typename T>
std::string	ft_to_string (T __n)
{
    std::ostringstream  oss;

    oss << __n;
    return (oss.str());
}
