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

#ifndef _FT_PRINT_MEMORY_HPP
# define _FT_PRINT_MEMORY_HPP

#pragma once

# include <unistd.h>
# include <inttypes.h>
# include <string>

# define FTPM_BRED	"\x1B[31;40m"
# define FTPM_BCYN	"\x1B[36;40m"
# define FTPM_BBLU	"\x1B[34;40m"
# define FTPM_BYEL	"\x1B[33;40m"
# define FTPM_BBLK	"\x1B[0;40m"
# define FTPM_RST	"\x1B[0m"

// void	*ft_print_memory(void *addr, size_t size);

static
void	ft_pm_print_color(std::string &mem, const char *color, std::string content)
{
	mem += FTPM_BBLU;
	mem += ".";
	mem += FTPM_BBLK;
}

static
void	ft_print_hex(std::string &mem, uint64_t v, int nbd)
{
	unsigned long	tab[16];
	long			j;
	unsigned char	qwe;

	j = 0;
	while (j < nbd)
		tab[j++] = 0;
	j--;
	while ((v > 0) && (j || nbd == 1))
	{
		tab[j] = v % 16;
		v /= 16;
		j--;
	}
	j = 1;
	while (j < nbd)
	{
		if (tab[j] >= 10)
			qwe = (tab[j] - 10 + 'a');
		else
			qwe = (tab[j] += '0');
		mem += std::string(1, (char)qwe);
		j++;
	}
}

static
void	ft_prepare_oct_print(std::string &mem, void *addr)
{
	unsigned char	*l;

	l = reinterpret_cast<unsigned char *>(addr);
	if ((*l & 0xff) == 0)
	{
		mem += FTPM_BRED;
		ft_print_hex(mem, *l, 3);
		mem += FTPM_BBLK;
	}
	else if ((*l & 0xff) == 0xff)
	{
		mem += FTPM_BCYN;
		ft_print_hex(mem, *l, 3);
		mem += FTPM_BBLK;
	}
	else
		ft_print_hex(mem, *l, 3);
}

static
int	ft_aff_oct(std::string &mem, void *addr, unsigned int size)
{
	int		cpt;
	int		flagou;
	int		sp;

	sp = 0;
	flagou = 0;
	cpt = 16;
	if (size < 16)
		cpt = size;
	while (cpt)
	{
		ft_prepare_oct_print(mem, addr);
		if (flagou)
		{
			mem += " ";
			flagou = 0;
			sp++;
		}
		else
			flagou++;
		addr = ((unsigned char *)addr) + 1;
		cpt--;
		sp += 2;
	}
	return (sp);
}

static
void	ft_aff_msg(std::string &mem, void *addr, int nbr)
{
	char	*c;

	while (nbr)
	{
		c = (char *)addr;
		if ((*c & 0xff) == '.')
			mem += ".";
		else if ((*c >= ' ' && *c <= '~'))
			mem += std::string(1, *c);
		else if (*c == 0)
			ft_pm_print_color(mem, FTPM_BRED, ".");
		else if ((*c & 0xff) == 0xff)
			ft_pm_print_color(mem, FTPM_BCYN, ".");
		else
			ft_pm_print_color(mem, FTPM_BBLU, ".");
		addr = ((unsigned char *)addr) + 1;
		nbr--;
	}
}

void	*ft_print_memory(void *addr, size_t size)
{
	int			byte_left;
	std::string	mem = "";

	byte_left = 0;
	mem += FTPM_BBLK;

	while (size > 16)
	{
		mem += FTPM_BCYN;
		ft_print_hex(mem, (uint64_t)addr, 16);
		mem += ": ";
		mem += FTPM_BBLK;
		ft_aff_oct(mem, addr, size);
		ft_aff_msg(mem, addr, 16);
		mem += FTPM_RST;
		mem += "\n";
		mem += FTPM_BBLK;
		size -= 16;
		addr = ((unsigned char *)addr) + 16;
	}
	if (size)
	{

		mem += FTPM_BCYN;
		ft_print_hex(mem, (uint64_t)addr, 16);
		mem += ": ";
		mem += FTPM_BBLK;
		byte_left = 40 - ft_aff_oct(mem, addr, size);
		mem += std::string(byte_left, ' ');
		ft_aff_msg(mem, addr, size);
	}
	mem += FTPM_RST;
	mem += "\n";

	std::cout << mem;
	return (addr);
}

#endif
