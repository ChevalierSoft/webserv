/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 12:49:37 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Request.hpp"

class ResponseGenerator // * ___________________________________________________
{
	/// * Variables ____________________________________________________________

private:
	/* data */

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
	
	std::string			generate(Request& rq);

	std::string			get_file_content(std::string root, std::string paht);

}; // * ________________________________________________________________________
