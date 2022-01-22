/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/22 08:37:00 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Request.hpp"

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
	
	std::string			generate(const Request & rq);

	std::string			get_file_content(const std::string & root, const std::string & paht);

	std::string			perform_GET_methode(const Request & rq);

	std::string			set_file_content_type(const std::string & extention);

}; // * ________________________________________________________________________
