/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Generator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 10:06:11 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/21 10:12:20 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Response_Generator
{
private:
	/* data */
public:
	// Constructors & Destructors
	Response_Generator(void);
	Response_Generator(/* args */);
	~Response_Generator(void);

	// Copy constructor
	Response_Generator(const Response_Generator & copy);

	// Operation overload =
	Response_Generator &	operator=(const Response_Generator &	copy);
};

Response_Generator::Response_Generator(void)
{
}

Response_Generator::Response_Generator(/* args */)
{
}

Response_Generator::~Response_Generator(void)
{
}

Response_Generator::Response_Generator(const Response_Generator & copy)
{
	*this = copy;
	return ;
}

Response_Generator &	Response_Generator::operator=(const Response_Generator& copy)
{
	if (this != &copy)
	{
		// copy
	}
	return (*this);
}
