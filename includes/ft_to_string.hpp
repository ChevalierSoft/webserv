/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_to_string.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljurdant <ljurdant@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:55:36 by ljurdant          #+#    #+#             */
/*   Updated: 2022/02/11 13:55:38 by ljurdant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

template <typename T>
std::string	ft_to_string (T __n)
{
    std::ostringstream  oss;

    oss << __n;
    return (oss.str());
}
