/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/25 18:45:07 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Client.hpp"
# include "Conf.hpp"

class Client;

/**
 * @brief Thanks to the parsed Request,
 * 	      ResponseGenerator will generate a response to the client
 * 
 */
class ResponseGenerator // * ___________________________________________________
{
	/// * Variables ____________________________________________________________
private:
	const Conf				*_conf;

private:
	static const std::map<std::string, std::string>	_ss_content_types;	// ? map of content types

	/// * Constructors & Destructors ___________________________________________
public:
	// ? (1) default
	ResponseGenerator (void);

	// ? (2) by copy
	ResponseGenerator (const ResponseGenerator & copy);

	~ResponseGenerator (void);

	ResponseGenerator&	operator=(const ResponseGenerator &	copy);

	/// * Member function ______________________________________________________

	bool				generate(Client & client) const;

	void				set_conf(const Conf * c);

private:
	std::string			get_file_content(const std::string & path) const;

	std::string			perform_GET_methode(const Request & rq) const;

	std::string			set_file_content_type(const std::string & extention) const;

	Request				parse_request_route(Request  const & input_request) const;

	bool				is_directory(const std::string path) const;

}; // * ________________________________________________________________________
