/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_bx.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jserrano <jserrano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 18:24:32 by jserrano          #+#    #+#             */
/*   Updated: 2020/11/04 16:11:00 by jserrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../raymarching.h"

static void	cpy_sq(t_box **sq, t_box **aux, int i)
{
	int	j;

	j = -1;
	while (++j < 3)
	{
		aux[i]->o[j] = sq[i]->o[j];
		aux[i]->v[j] = sq[i]->v[j];
		aux[i]->x[j] = sq[i]->x[j];
		aux[i]->y[j] = sq[i]->y[j];
	}
	aux[i]->h = sq[i]->h;
	aux[i]->col = sq[i]->col;
}

static void	new_sq(t_box **aux, double **o_v, double h, int *var)
{
	int j;

	j = -1;
	while (++j < 3)
	{
		aux[var[0]]->o[j] = o_v[0][j];
		aux[var[0]]->v[j] = o_v[1][j];
	}
	aux[var[0]]->x[0] = 0;
	aux[var[0]]->x[1] = -o_v[1][2];
	aux[var[0]]->x[2] = o_v[1][1];
	j = -1;
	while (++j < 3)
		aux[var[0]]->y[j] = cross_prod(o_v[1], aux[var[0]]->x, j);
	norm(aux[var[0]]->y, aux[var[0]]->y);
	aux[var[0]]->h = h;
	aux[var[0]]->col = var[1];
}

static int	init_sq(t_box **sq, t_box ***aux, int *var)
{
	var[2] = 0;
	while (sq[var[2]])
		var[2]++;
	if (!(*aux = (t_box **)malloc(sizeof(t_box *) * (var[2] + 2))))
		return (0);
	return (1);
}

/*
**	var[3] = i, c, len
*/

t_box		**add_bx(t_box **sq, double **o_v, double h, int c)
{
	t_box		**aux;
	int			var[3];

	if (!init_sq(sq, &aux, var))
		return (sq);
	var[1] = c;
	norm(o_v[1], o_v[1]);
	var[0] = -1;
	while (++var[0] < var[2] + 1)
	{
		aux[var[0]] = (t_box *)malloc(sizeof(t_box));
		if (var[0] < var[2])
			cpy_sq(sq, aux, var[0]);
		else
			new_sq(aux, o_v, h, var);
	}
	aux[var[0]] = 0;
	var[0] = 0;
	if (var[2])
		while (sq[var[0]])
			free(sq[var[0]++]);
	free(sq);
	return (aux);
}
