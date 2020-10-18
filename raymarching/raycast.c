/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jserrano <jserrano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/01 09:38:55 by jserrano          #+#    #+#             */
/*   Updated: 2020/10/17 13:19:57 by jserrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raymarching.h"

/*
**	El plano de camara tiene un origen "P" que es el centro.
**	A partir de él nacen los ejes en forma de vectores unitarios "Vx" y "Vy".
**	Para calcular cualquier punto del plano en el espacio solo
**	basta con hacer la siguiente operacion:
**			P' = P + (x - (res_X / 2) * Vx) + (y - (res_Y / 2) * Vy)
**	donde "x" es la coordenada dentro de la pantalla correspondiente
**	al eje horizontal que va desde 0 hasta la resolucion
**	(1920 o 600 por defecto)
**	e "y" es la coordenada dentro de la pantalla correspondiente al
**	eje vertical que va desde 0 hasta la resolucion (1080 o 600 por defecto).
*/

void	calculate_vectors(t_data *param)
{
	double module;
	int		i;

	param->cam.Vx[0] = -param->cam.Vn[1];
	param->cam.Vx[1] = param->cam.Vn[0];
	param->cam.Vx[2] = 0;
	param->cam.Vy[0] = -param->cam.Vn[0] * param->cam.Vn[2];
	param->cam.Vy[1] = -param->cam.Vn[1] * param->cam.Vn[2];
	param->cam.Vy[2] = pow(param->cam.Vn[1], 2) + pow(param->cam.Vn[0], 2);
	i = -1;
	while (++i < 3)
		param->cam.P[i] = param->cam.O[i] + param->cam.Vn[i] * param->scr.dist;
	module = sqrt(pow(param->cam.Vx[0], 2) + pow(param->cam.Vx[1], 2));
	i = -1;
	while (++i < 2)
		param->cam.Vx[i] /= module;
	i = -1;
	while (++i < 3)
		param->cam.Vy[i] /= module;
}

static void	my_mlx_pixel_put(t_data *param, int x, int y, int color)
{
	char	*dst;

	dst = param->img.addr + (y * param->img.line_length + x *
			(param->img.bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void	calculate_rotation(t_data *param)
{
	double rotation_z_axis;
	double rotation_y_axis;
	double xy_projection;

	rotation_z_axis = param->crd.x * 2 * param->mou.speed * M_PI / 250 +
						param->cam.rot_z;
	rotation_y_axis = param->crd.y * 2 * param->mou.speed * M_PI / 250 +
						param->cam.rot_y;
	xy_projection = cos(rotation_y_axis);
	param->cam.Vn[0] = cos(rotation_z_axis) * xy_projection;
	param->cam.Vn[1] = sin(rotation_z_axis) * xy_projection;
	param->cam.Vn[2] = sin(rotation_y_axis);
}

void	gen_ray(t_data *param, int x, int y, int boolean)
{
	int		i;

	i = -1;
	while (++i < 3)
		param->cam.pixel[i] = param->cam.P[i] - (param->scr.x / 2 - x) *
			param->cam.Vx[i] + (param->scr.y / 2 - y) * param->cam.Vy[i];
	i = -1;
	while (++i < 3)
		param->cam.ray[i] = param->cam.pixel[i] - param->cam.O[i];
	if (boolean)
		param->cam.ray_module = sqrt(pow(param->cam.ray[0], 2) +
					pow(param->cam.ray[1], 2) + pow(param->cam.ray[2], 2));
	i = -1;
	while (++i < 3)
		param->cam.ray[i] /= param->cam.ray_module;
}

double	sp_dist(t_data *param)
{
	double	dist;
	double	aux;
	int		i;

	dist = 2147483647;
	i = 0;
	while (param->sp[i])
	{
		aux = sqrt(pow(param->sp[i]->O[0] - param->cam.pixel[0], 2) +
					pow(param->sp[i]->O[1] - param->cam.pixel[1], 2) +
					pow(param->sp[i]->O[2] - param->cam.pixel[2], 2)) -
					param->sp[0]->r;
		dist = (aux < dist) ? aux : dist;
		i++;
	}
	return (dist);
}

int		is_pixel(t_data *param)
{
	double dist;
	double tray;
	int		j;

	dist = 10;
	tray = 0;
	while (dist > 5)
	{
		dist = sp_dist(param);
		tray += dist;
		if (tray > 20000)
			return (0);
		j = -1;
		while (++j < 3)
			param->cam.pixel[j] += param->cam.ray[j] * dist;
	}
	return (1);
}

t_sphere	**add_sp(t_sphere **sp, double *O, double r)
{
	t_sphere	**aux;
	int			len;
	int			i;
	int			j;

	len = 0;
	while (sp[len])
		len++;
	if (!(aux = (t_sphere **)malloc(sizeof(t_sphere *) * (len + 2))))
		return (sp);
	i = -1;
	while(++i < len + 1)
	{
		aux[i] = (t_sphere *)malloc(sizeof(t_sphere));
		j = -1;
		if (i < len)
		{
			while (++j < 3)
				aux[i]->O[j] = sp[i]->O[j];
			aux[i]->r = sp[i]->r;
		}
		else
		{
			while (++j < 3)
				aux[i]->O[j] = O[j];
			aux[i]->r = r;
		}
	}
	aux[i] = 0;
	i = 0;
	if (len)
		while (sp[i])
			free(sp[i++]);
	free(sp);
	return (aux);
}

t_light		**add_l(t_light **l, double *O)
{
	t_light		**aux;
	int			len;
	int			i;
	int			j;

	len = 0;
	while (l[len])
		len++;
	if (!(aux = (t_light **)malloc(sizeof(t_light *) * (len + 2))))
		return (l);
	i = -1;
	while(++i < len + 1)
	{
		aux[i] = (t_light *)malloc(sizeof(t_light));
		j = -1;
		if (i < len)
			while (++j < 3)
				aux[i]->O[j] = l[i]->O[j];
		else
			while (++j < 3)
				aux[i]->O[j] = O[j];
	}
	aux[i] = 0;
	i = 0;
	if (len)
		while (l[i])
			free(l[i++]);
	free(l);
	return (aux);
}

/*
** ****************************************************************************
** *************************FUNCIONES DE PRUEBA********************************
** ****************************************************************************
*/

/*
** Funcion de refresco de imagen
*/

void	show_camera(t_data *param)
{
	int x;
	int y;

	int O_planta[2];
	int O_alzado[2];
	int O_perfil[2];

	int up_left[3];
	int up_right[3];
	int down_left[3];
	int down_right[3];

	O_planta[0] = param->scr.x / 4;
	O_planta[1] = (param->scr.y / 4) * 3;

	O_alzado[0] = param->scr.x / 4;
	O_alzado[1] = param->scr.y / 4;

	O_perfil[0] = (param->scr.x / 4) * 3;
	O_perfil[1] = param->scr.y / 4;

	up_left[0] = param->cam.P[0] - 50 * param->cam.Vx[0] + 25 * param->cam.Vy[0];
	up_left[1] = param->cam.P[1] - 50 * param->cam.Vx[1] + 25 * param->cam.Vy[1];
	up_left[2] = param->cam.P[2] - 50 * param->cam.Vx[2] + 25 * param->cam.Vy[2];

	up_right[0] = param->cam.P[0] + 50 * param->cam.Vx[0] + 25 * param->cam.Vy[0];
	up_right[1] = param->cam.P[1] + 50 * param->cam.Vx[1] + 25 * param->cam.Vy[1];
	up_right[2] = param->cam.P[2] + 50 * param->cam.Vx[2] + 25 * param->cam.Vy[2];

	down_left[0] = param->cam.P[0] - 50 * param->cam.Vx[0] - 25 * param->cam.Vy[0];
	down_left[1] = param->cam.P[1] - 50 * param->cam.Vx[1] - 25 * param->cam.Vy[1];
	down_left[2] = param->cam.P[2] - 50 * param->cam.Vx[2] - 25 * param->cam.Vy[2];

	down_right[0] = param->cam.P[0] + 50 * param->cam.Vx[0] - 25 * param->cam.Vy[0];
	down_right[1] = param->cam.P[1] + 50 * param->cam.Vx[1] - 25 * param->cam.Vy[1];
	down_right[2] = param->cam.P[2] + 50 * param->cam.Vx[2] - 25 * param->cam.Vy[2];

	y = 0;
	while (y < param->scr.y)
	{
		x = 0;
		while (x < param->scr.x)
		{
			if (x == param->scr.x / 2 || y == param->scr.y / 2)
				my_mlx_pixel_put(param, x, y, 0xFFFFFF);
			else
				my_mlx_pixel_put(param, x, y, 0x000000);
			x++;
		}
		y++;
	}

	my_mlx_pixel_put(param, O_planta[0] + up_left[0],
					O_planta[1] + up_left[1], 0xFF0000);
	my_mlx_pixel_put(param, O_planta[0] + up_right[0],
					O_planta[1] + up_right[1], 0xFF0000);
	my_mlx_pixel_put(param, O_planta[0] + down_left[0],
					O_planta[1] + down_left[1], 0x4EC7FF);
	my_mlx_pixel_put(param, O_planta[0] + down_right[0],
					O_planta[1] + down_right[1], 0x4EC7FF);
	my_mlx_pixel_put(param, O_planta[0] + param->cam.P[0],
					O_planta[1] + param->cam.P[1], 0xFFFFFF);
	my_mlx_pixel_put(param, O_planta[0] + param->cam.O[0],
					O_planta[1] + param->cam.O[1], 0xFFFFFF);

	my_mlx_pixel_put(param, O_alzado[0] + up_left[0],
					O_alzado[1] - up_left[2], 0xFF0000);
	my_mlx_pixel_put(param, O_alzado[0] + up_right[0],
					O_alzado[1] - up_right[2], 0xFF0000);
	my_mlx_pixel_put(param, O_alzado[0] + down_left[0],
					O_alzado[1] - down_left[2], 0x4EC7FF);
	my_mlx_pixel_put(param, O_alzado[0] + down_right[0],
					O_alzado[1] - down_right[2], 0x4EC7FF);
	my_mlx_pixel_put(param, O_alzado[0] + param->cam.P[0],
					O_alzado[1] - param->cam.P[2], 0xFFFFFF);
	my_mlx_pixel_put(param, O_alzado[0] + param->cam.O[0],
					O_alzado[1] - param->cam.O[2], 0xFFFFFF);

	my_mlx_pixel_put(param, O_perfil[0] + up_left[1],
					O_perfil[1] - up_left[2], 0xFF0000);
	my_mlx_pixel_put(param, O_perfil[0] + up_right[1],
					O_perfil[1] - up_right[2], 0xFF0000);
	my_mlx_pixel_put(param, O_perfil[0] + down_left[1],
					O_perfil[1] - down_left[2], 0x4EC7FF);
	my_mlx_pixel_put(param, O_perfil[0] + down_right[1],
					O_perfil[1] - down_right[2], 0x4EC7FF);
	my_mlx_pixel_put(param, O_perfil[0] + param->cam.P[1],
					O_perfil[1] - param->cam.P[2], 0xFFFFFF);
	my_mlx_pixel_put(param, O_perfil[0] + param->cam.O[1],
					O_perfil[1] - param->cam.O[2], 0xFFFFFF);

	mlx_put_image_to_window(param->id, param->win_id, param->img.img, 0, 0);

}

void	show_pov(t_data *param)
{
	int x;
	int y;

	y = 0;
	while (y < param->scr.y)
	{
		x = 0;
		while (x < param->scr.x)
		{
			gen_ray(param, x, y, 0);
			if (param->cam.ray[2] < 0)
			{
				if (param->cam.ray[1] < 0)
				{
					if (param->cam.ray[0] < 0)
						my_mlx_pixel_put(param, x, y, 0x864545);
					else
						my_mlx_pixel_put(param, x, y, 0x858645);
				}
				else
				{
					if (param->cam.ray[0] < 0)
						my_mlx_pixel_put(param, x, y, 0x458686);
					else
						my_mlx_pixel_put(param, x, y, 0x864581);
				}
			}
			else
			{
				if (param->cam.ray[1] < 0)
				{
					if (param->cam.ray[0] < 0)
						my_mlx_pixel_put(param, x, y, 0xFF5A5A);
					else
						my_mlx_pixel_put(param, x, y, 0xFFFF5A);
				}
				else
				{
					if (param->cam.ray[0] < 0)
						my_mlx_pixel_put(param, x, y, 0x5AFAFF);
					else
						my_mlx_pixel_put(param, x, y, 0xFF5AFF);
				}
			}
			x += 2;
		}
		y += 2;
	}
	mlx_put_image_to_window(param->id, param->win_id, param->img.img, 0, 0);
}

void	show_sp(t_data *param)
{
	int x;
	int y;
	int i;
	int j;
	double dist;
	int color;

	y = 0;
	while (y <= param->scr.y / 2)
	{
		x = 0;
		while (x <= param->scr.x / 2)
		{
			i = -1;
			while (++i < 2)
			{
				j = -1;
				while (++j < 2)
				{
					if (!i && !j)
						gen_ray(param, x, y, 1);
					else
						gen_ray(param, j * (param->scr.x - x) + (1 - j) * x, i * (param->scr.y - y) + (1 - i) * y, 0);
					if (is_pixel(param))
						my_mlx_pixel_put(param, j * (param->scr.x - x) + (1 - j) * x, i * (param->scr.y - y) + (1 - i) * y, 0xFFFFFF);
					else
						my_mlx_pixel_put(param, j * (param->scr.x - x) + (1 - j) * x, i * (param->scr.y - y) + (1 - i) * y, 0);
				}
			}
			x += 4;
		}
		y += 4;
	}
	mlx_put_image_to_window(param->id, param->win_id, param->img.img, 0, 0);
}