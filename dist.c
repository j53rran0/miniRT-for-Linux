/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dist.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jserrano <jserrano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/20 11:30:12 by jserrano          #+#    #+#             */
/*   Updated: 2020/10/28 12:53:38 by jserrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dist/dist.h"

static void		obj_dist_p1(t_data *param, double *dist, double *aux)
{
	*dist = sp_dist(param);
	param->cam.ray.obj_c = (*dist < 2147483647) ?
		param->sp[param->cam.ray.obj_n]->col : param->cam.ray.obj_c;
	*aux = pl_dist(param);
	param->cam.ray.obj_c = (*aux < *dist) ?
		param->pl[param->cam.ray.obj_n]->col : param->cam.ray.obj_c;
	*dist = (*aux < *dist) ? *aux : *dist;
	*aux = cy_dist(param);
	param->cam.ray.obj_c = (*aux < *dist) ?
		param->cy[param->cam.ray.obj_n]->col : param->cam.ray.obj_c;
	*dist = (*aux < *dist) ? *aux : *dist;
}

static void		obj_dist_p2(t_data *param, double *dist, double *aux)
{
	*aux = sq_dist(param);
	param->cam.ray.obj_c = (*aux < *dist) ?
		param->sq[param->cam.ray.obj_n]->col : param->cam.ray.obj_c;
	*dist = (*aux < *dist) ? *aux : *dist;
	*aux = bx_dist(param);
	param->cam.ray.obj_c = (*aux < *dist) ?
		param->bx[param->cam.ray.obj_n]->col : param->cam.ray.obj_c;
	*dist = (*aux < *dist) ? *aux : *dist;
	*aux = tr_dist(param);
	param->cam.ray.obj_c = (*aux < *dist) ?
		param->tr[param->cam.ray.obj_n]->col : param->cam.ray.obj_c;
	*dist = (*aux < *dist) ? *aux : *dist;
}

double			obj_dist(t_data *param)
{
	double	dist;
	double	aux;

	obj_dist_p1(param, &dist, &aux);
	obj_dist_p2(param, &dist, &aux);
	return (dist);
}
