/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_memory.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/04 19:58:59 by dait-atm          #+#    #+#             */
/*   Updated: 2021/07/08 05:42:54 by dait-atm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINT_MEMORY_H
# define FT_PRINT_MEMORY_H

# include <unistd.h>
# include <inttypes.h>

void	*ft_print_memory(void *addr, size_t size);

static
void	ft_print_hex(uint64_t v, int nbd)
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
		write(1, &qwe, 1);
		j++;
	}
}

static
void	ft_prepare_oct_print(void *addr)
{
	unsigned char	*l;

	l = reinterpret_cast<unsigned char *>(addr);
	if ((*l & 0xff) == 0)
	{
		write(1, "\x1B[31;02m", 8);
		ft_print_hex(*l, 3);
		write(1, "\x1B[0m", 4);
	}
	else if ((*l & 0xff) == 0xff)
	{
		write(1, "\x1B[36;02m", 8);
		ft_print_hex(*l, 3);
		write(1, "\x1B[0m", 4);
	}
	else
		ft_print_hex(*l, 3);
}

static
int	ft_aff_oct(void *addr, unsigned int size)
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
		ft_prepare_oct_print(addr);
		if (flagou)
		{
			write(1, " ", 1);
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
void	ft_aff_msg(void *addr, int nbr)
{
	char	*c;

	while (nbr)
	{
		c = (char *)addr;
		if ((*c & 0xff) == 0x2e)
			write(1, ".", 1);
		else if ((*c >= ' ' && *c <= '~'))
			write(1, &(*c), 1);
		else if (*c == 0)
			write(1, "\x1B[31;02m.\x1B[0m", 13);
		else if ((*c & 0xff) == 0xff)
			write(1, "\x1B[36;02m.\x1B[0m", 13);
		else
			write(1, "\x1B[1;30m.\x1B[0m", 12);
		addr = ((unsigned char *)addr) + 1;
		nbr--;
	}
}

void	*ft_print_memory(void *addr, size_t size)
{
	size_t	aerosol;

	aerosol = 0;
	while (size > 16)
	{
		ft_print_hex((uint64_t)((uint64_t *)addr), 16);
		write(1, ": ", 2);
		ft_aff_oct(addr, size);
		ft_aff_msg(addr, 16);
		write(1, "\n", 1);
		size -= 16;
		addr = ((unsigned char *)addr) + 16;
	}
	if (size)
	{
		ft_print_hex((uint64_t)addr, 16);
		write(1, ": ", 2);
		aerosol = 40 - ft_aff_oct(addr, size);
		while (aerosol-- > 0)
			write(1, " ", 1);
		ft_aff_msg(addr, size);
	}
	return (addr);
}

#endif
