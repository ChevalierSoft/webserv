/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 11:27:14 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class ResponseGenerator
{
private:
	/* data */
public:
	// Constructors & Destructors
	ResponseGenerator(void);
	ResponseGenerator(/* args */);
	~ResponseGenerator(void);

	// Copy constructor
	ResponseGenerator(const ResponseGenerator & copy);

	// Operation overload =
	ResponseGenerator &	operator=(const ResponseGenerator &	copy);
};

ResponseGenerator::ResponseGenerator(void)
{
}

ResponseGenerator::ResponseGenerator(/* args */)
{
}

ResponseGenerator::~ResponseGenerator(void)
{
}

ResponseGenerator::ResponseGenerator(const ResponseGenerator & copy)
{
	*this = copy;
	return ;
}

ResponseGenerator &	ResponseGenerator::operator=(const ResponseGenerator& copy)
{
	if (this != &copy)
	{
		// copy
	}
	return (*this);
}
