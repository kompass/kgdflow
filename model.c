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

double scalar_product_vect(vect_t *a, vect_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

double length_vect(vect_t *cible)
{
	double x = cible->x;
	double y = cible->y;
	double z = cible->z;

	return sqrt(x*x + y*y + z*z);
}

void normalize_vect(vect_t *cible)
{
	mul_vect(cible, 1 / length_vect(cible));
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
	cible->length++;
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

	model->num_chunk = 0;
	model->max_chunk = 0;
	model->chunk_list = NULL;

	int error = 0;
	error = init_grid(&(model->part_grid), conf);

	if(error)
	{
		free(model);
		return 1;
	}

	init_vect(model->gravity, 0, 0, config->gravity);
	model->h = config->h;

	return model;
}

void close_model(model_t *model)
{
	free(model);
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

/*
double quintic_kernel(vect_t *vect, double h)
{
	double alpha = 7 / (8*pi*h*h*h);
	double q = length_vect(vect) / h;

	double x = (1 - q/2);
	double x4 = x*x*x*x;

	return alpha*x4*(2*q + 1);
}
*/

/*
double quintic_kernel_deriv(vect_t *vect, double h)
{
	double alpha = 7 / (8*pi*h*h*h);
	double q = length_vect(vect) / h;

	double x = (1 - q/2);
}
*/

/*
void compute_particule_position(particule_t *part, part_list *neighbors)
{
	
}
*/

int add_chunk(model_t *model, vect_t *pos)
{
	int i = 0;

	model->num_chunk++;
	if(model->num_chunk > model->max_chunk)
	{
		particule_t **new_chunk_list = malloc((model->max_chunk + 8) * sizeof(*particule_t));
		for(i = 0; i < model->num_chunk - 1; i++)
			new_chunk_list[i] = model->chunk_list[i];
		model->chunk_list = new_chunk_list;
		model->max_chunk += 8;
	}

	particule_t *new_chunk = malloc()
	model->chunk_list[model->num_chunk - 1] =
}

void update_model(model_t *model, event_t *event, double delta)
{
	particule_t *part, *neighbor;
	vect_t gravity_action, r_ij, u_ij, v_ji, viscosity_action, relax_action;
	double h = model->h;
	double q, u;
	double dens, dens_neigh, press, press_neigh;
	double dens0 = model->dens0;
	double k = model->k;
	double k_neigh = model->k_neigh;
	double buffer;

	//pour toute particule part:
	//Calcul Viscosité:
	//pour toute particule neighbor voisine de part tq i<j :
	copy_vect(&r_ij, &(neighbor->pos));
	sub_vect(&r_ij, &(part->pos));
	q = length_vect(&r_ij) / h;
	if(q < 1)
	{
		copy_vect(&u_ij, &r_ij);
		normalize_vect(&u_ij);

		copy_vect(&v_ji, &(part->speed));
		sub_vect(&v_ji, &(neighbor->speed));

		//vitesse radiale entrante
		u = scalar_product_vect(&v_ji, &u_ij);

		if(u > 0)
		{
			copy_vect(&viscosity_action, &u_ij);
			mul_vect(&viscosity_action, delta * (1 - q) * (sigma*u + beta*u*u));
			mul_vect(&viscosity_action, 1 / 2);

			sub_vect(&(part->speed), &viscosity_action);
			add_vect(&(neighbor->speed), &viscosity_action);
		}
	}
	//Fin Pour tous voisins

	//Calcul gravité
	copy_vect(&gravity_action, &(model->gravity));
	mul_vect(&gravity_action, delta);
	add_vect(&(part->speed), &gravity_action);
	//Fin pour toute particule

	//pour toute particule
	//Calcul relaxation double densité
	dens = 0;
	dens_neigh = 0;
	//Pour toute particule voisine
	copy_vect(&r_ij, &(neighbor->pos));
	sub_vect(&r_ij, &(part->pos));
	q = length_vect(&r_ij) / h;

	if(q < 1)
	{
		buffer = 1 - q;
		dens += buffer * buffer;
		dens_neigh += buffer * buffer * buffer;
	}

	//Fin pour toute particule voisine

	press = k * (dens - dens0);
	press_neigh = k_neigh * dens_neigh;

	//Pour toute particule voisine
	copy_vect(&r_ij, &(neighbor->pos));
	sub_vect(&r_ij, &(part->pos));
	q = length_vect(&r_ij) / h;
	copy_vect(&u_ij, &r_ij);
	normalize_vect(&u_ij);
	if(q < 1)
	{
		copy_vect(&relax_action, &u_ij);
		buffer = 1 - q;
		mul_vect(&relax_action, delta*delta*(press*buffer + press_neigh*buffer*buffer));
		mul_vect(&relax_action, 1/2);
		add_vect(&(neighbor->pos), &relax_action);
		sub_vect(&(part->pos), &relax_action);
	}
}