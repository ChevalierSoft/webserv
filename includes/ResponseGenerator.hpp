/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 17:57:28 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Client.hpp"

class Client;

/**
 * @brief Thanks to the generated Request,
 * 	      ResponseGenerator will generate a response to the client
 * 
 */
class ResponseGenerator // * ___________________________________________________
{
	/// * Variables ____________________________________________________________

private:
	static const std::map<std::string, std::string>	_ss_content_types;	// ? map of content types

	/// * Constructors & Destructors ___________________________________________

public:
	// ? (1) default
	ResponseGenerator (void);

	// ? (2) by copy
	ResponseGenerator (const ResponseGenerator & copy);

	~ResponseGenerator (void);

	/// * Member function ______________________________________________________

	// Operation overload =
	ResponseGenerator&	operator=(const ResponseGenerator &	copy);
	
	bool				generate(Client & client);

private:
	std::string			get_file_content(const std::string & root, const std::string & paht);

	std::string			perform_GET_methode(const Request & rq);

	std::string			set_file_content_type(const std::string & extention);

}; // * ________________________________________________________________________
