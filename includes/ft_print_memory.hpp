/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_memory.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpellier <lpellier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/04 19:58:59 by dait-atm          #+#    #+#             */
/*   Updated: 2022/01/18 17:58:50 by lpellier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

# define FTPM_BRED	"\x1B[31;40m"
# define FTPM_BCYN	"\x1B[36;40m"
# define FTPM_BBLU	"\x1B[34;40m"
# define FTPM_BYEL	"\x1B[33;40m"
# define FTPM_BBLK	"\x1B[0;40m"
# define FTPM_RST	"\x1B[0m"

void	*ft_print_memory(void *addr, size_t size);