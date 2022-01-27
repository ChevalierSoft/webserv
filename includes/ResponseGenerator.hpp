/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/27 16:07:27 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Client.hpp"
# include "Conf.hpp"

# define CGI_BUFF_SIZE	1200

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
	const Conf										*_conf;
	static const std::map<std::string, std::string>	_ss_content_types;
	static const std::map<int, std::string>			_ss_error_messages;

	/// * Constructors & Destructors ___________________________________________
public:
	// ? (1) default
	ResponseGenerator (void);

	// ? (2) by copy
	ResponseGenerator (const ResponseGenerator & copy);

	~ResponseGenerator (void);

	ResponseGenerator&	operator=(const ResponseGenerator &	copy);

	/// * Member function ______________________________________________________
public:
	bool				generate(Client & client) const;

	void				set_conf(const Conf * c);

private:
	std::string			set_file_content_type(const std::string & extention) const;

	std::string			set_header (int err, std::string ct, size_t size) const;

	std::string			generic_error(int err) const;

	std::string			get_error_file(int err) const;

	std::string			get_file_content(const std::string & root, const std::string & paht) const;

	std::string			perform_GET_method(const Client & client) const;

}; // * ________________________________________________________________________
