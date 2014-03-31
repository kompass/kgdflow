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

void append_part_cell_list(part_list_t *cible, part_list_cell_t *cell)
{
	cell->part = part;
	cell->prev = NULL;
	cell->next = cible->first;
	cible->first->prev = cell;
	cible->first = cell;	
}

int append_part_list(part_list_t *cible, particule_t *part);
{
	part_list_cell_t *new_cell = malloc(sizeof(part_list_cell_t));

	if(new_cell == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR,
			"Erreur d'allocation de part_list_cell_t dans append_part_part_list.");
		return 1;
	}

	append_part_cell_list(cible, new_cell);

	return 0;
}

int init_grid(grid_t *grid, config_t *conf)
{
	int size = conf->grid_size;

	grid->delta = conf->grid_delta;
	grid->size = size;
	grid->map = malloc(size * size * size * sizeof(part_list_t));

	if(grid->map == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR,
			"Erreur d'allocation d'un tableau de part_list_t dans init_grid.");
		return 1;
	}

	int i = 0;
	for(i = 0; i < grid->size; i++)
		init_part_list(&(grid->map[i]));

	return 0;
}

model_t* init_model(config_t *conf)
{
	model_t *model = malloc(sizeof(model_t));

	if(model == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR, "Erreur d'allocation de model_t dans init_model.");
		return NULL;
	}

	model->num_of_part = 0;
	model->chunk_list = NULL;

	int error = 0;
	error = init_grid(&(model->part_grid), conf);

	if(error)
	{
		free(model);
		return 1;
	}

	return model;
}

int part_hash_grid(grid_t *grid, particule_t *part)
{
	int x = (int) part->pos->x / grid->delta;
	int y = (int) part->pos->y / grid->delta;
	int z = (int) part->pos->z / grid->delta;
	int size = grid->size;

	return (x*size+y)*size + z;
}

int insert_part_grid(grid_t *grid, particule_t *part)
{
	int hash = part_hash_grid(grid, part);
	int error = 0;
	int size = grid->size;

	if(hash < 0 || hash >= size*size*size)
	{
		new_error(OUT_OF_BOUNDS_ERROR, "Hash hors limites dans insert_part_grid");
		return 1;
	}

	error = append_part_list(grid->map[hash], part);

	if(error)
		return 1;

	return 0;
}

void remove_part_cell_grid(grid_t *grid, part_list_cell_t *cell)
{
	int i = 0;

	cell->next->prev = cell->prev;
	if(cell->prev == NULL)
	{
		for(i = 0; i < grid->size; i++)
		{
			if(grid->map[i] == cell)
			{
				grid->map[i] = cell->next;
				break;
			}
		}
	}
	else
		cell->prev->next = cell->next;

	cell->prev = NULL;
	cell->next = NULL;
}

int update_part_cell_grid(grid_t *grid, part_list_cell_t *cell)
{
	int hash = part_hash_grid(grid, cell->part);
	int size = grid->size;

	if(hash < 0 || hash >= size*size*size)
	{
		new_error(OUT_OF_BOUNDS_ERROR, "Hash hors limites dans update_part_cell_grid.");
		return 1;
	}

	remove_part_cell_grid(grid, cell);
	append_part_cell_list(grid, part);
}