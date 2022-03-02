/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_memory.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:55:06 by ljurdant          #+#    #+#             */
/*   Updated: 2022/03/02 10:00:16 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static
void	ft_pm_print_color(std::string &mem, const char *color, std::string content)
{
	mem += color;
	mem += content;
	mem += color;
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