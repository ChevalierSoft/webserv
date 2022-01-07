/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 00:54:13 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/07 04:05:28 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <vector>
# include <string>

// ? a client linked to a Server
class Client // * ______________________________________________________________
{
	/// * Variables ____________________________________________________________
public:
	int							_id;		// it's index in Server's _fds array
	std::vector<std::string>	_i_msg;		// the input message buffer_size by buffer_size

	/// * Constructors & Destructors ___________________________________________

	Client();
	
	~Client();

}; // * ________________________________________________________________________
