/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   def_tr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jserrano <jserrano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/30 14:21:06 by jserrano          #+#    #+#             */
/*   Updated: 2020/11/03 23:58:59 by jserrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../raymarching.h"
#include "def_ft.h"

int		def_tr(t_data *param, char *line)
{
	int		i;
	double	A[3];
	double	B[3];
	double	C[3];
	int		rgb[3];
	int		c;
	int		error;

	i = 2;
	error = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	error += def_P(line, &i, A);
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	error += def_P(line, &i, B);
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	error += def_P(line, &i, C);
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	error += def_rgb(line, &i, rgb, 1);
	rgb_to_hex(rgb, &c);
	param->tr = add_tr(param->tr, A, B, C, c);
	if (error)
		return (1);
	return (0);
}
