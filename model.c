#include "model.h"

#include <stdlib.h>
#include <math.h>

void init_vect(vect_t *cible, double x, double y, double z)
{
	cible->x = x;
	cible->y = y;
	cible->z = z;
}

vect_t* new_vect(double x, double y, double z)
{
	vect_t *ret = malloc(sizeof(vect_t));
	init_vect(ret, x, y, y);

	return ret;
}

void copy_vect(vect_t *cible, vect_t *other)
{
	init_vect(cible, other->x, other->y, other->z);
}

void add_vect(vect_t *cible, vect_t *other)
{
	cible->x += other->x;
	cible->y += other->y;
	cible->z += other->z;
}

void sub_vect(vect_t *cible, vect_t *other)
{
	cible->x -= other->x;
	cible->y -= other->y;
	cible->z -= other->z;
}

void mul_vect(vect_t *cible, double coef)
{
	cible->x *= coef;
	cible->y *= coef;
	cible->z *= coef;
}

void mul_vect(vect_t *cible, double coef)
{
	double x = cible->x, y = cible->y, z = cible->z;
	return sqrt(x*x + y*y + z*z);
}

void init_part(particule_t *cible, vect *pos, vect_t *speed)
{
	copy_vect(&(cible->pos), pos);
	copy_vect(&(cible->speed), speed);
}

void init_part_list(part_list_t* cible)
{
	cible->size = 0;
	cible->first = NULL;
}

void init_grid(grid_t *grid, config_t *conf)
{
	grid->delta = conf->grid_delta;
	grid->size = grid_size;
	grid->map = malloc(grid->size * sizeof(part_list_t));

	int i = 0;
	for(i = 0; i < grid->size; i++)
		init_part_list(&(grid->map[i]));
}

model_t* init_model(config_t *conf)
{
	model_t *model = malloc(sizeof(model_t));

	model->num_of_part = 0;
	model->chunk_list = NULL;
	init_grid(&(model->part_grid), conf);
}