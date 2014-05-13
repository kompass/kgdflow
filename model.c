#include "model.h"

#include <stdlib.h>
#include <math.h>
#include "error.h"
#include "view.h"

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

void init_part(particule_t *cible, vect_t *pos, vect_t *speed)
{
	copy_vect(&(cible->pos), pos);
	copy_vect(&(cible->speed), speed);
}

void init_part_list(part_list_t* cible)
{
	cible->length = 0;
	cible->first = NULL;
}

void append_part_cell_list(part_list_t *cible, part_list_cell_t *cell)
{
	cell->prev = NULL;
	cell->next = cible->first;
	if(cible->first != NULL)
		cible->first->prev = cell;
	cible->first = cell;
	cible->length++;
}

int append_part_list(part_list_t *cible, particule_t *part)
{
	part_list_cell_t *new_cell = malloc(sizeof(part_list_cell_t));

	if(new_cell == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR,
			"Erreur d'allocation de part_list_cell_t dans append_part_part_list.");
		return 1;
	}

	new_cell->part = part;
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
	for(i = 0; i < size*size*size; i++)
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
		return NULL;
	}

	model->size_of_chunk = conf->size_of_chunk;
	model->beta = conf->beta;
	model->sigma = conf->sigma;
	model->dens0 = conf->dens0;

	init_vect(&(model->gravity), 0, 0, conf->gravity);
	model->h = conf->h;
	model->k = conf->k;
	model->k_neigh = conf->k_neigh;
	model->coeff_frot = conf->coeff_frot;

	return model;
}

void close_model(model_t *model)
{
	free(model);
}

int hash_grid(grid_t *grid, vect_t *pos)
{
	int x = (int) pos->x / grid->delta;
	int y = (int) pos->y / grid->delta;
	int z = (int) pos->z / grid->delta;
	int size = grid->size;

	return (x*size+y)*size + z;
}

int part_hash_grid(grid_t *grid, particule_t *part)
{
	return hash_grid(grid, &(part->pos));
}

int insert_part_grid(grid_t *grid, particule_t *part)
{
	int hash = part_hash_grid(grid, part);
	int error = 0;
	int size = grid->size;

	if(hash < 0 || hash >= size*size*size)
	{
		new_error(OUT_OF_BOUNDS_ERROR, "Hash hors limites dans insert_part_grid.");
		return 1;
	}

	error = append_part_list(&(grid->map[hash]), part);

	if(error)
		return 1;

	return 0;
}

part_list_t* get_same_case_list(grid_t *grid, vect_t *pos)
{
	int hash = hash_grid(grid, pos);
	int size = grid->size;

	if(hash < 0 || hash >= size*size*size)
	{
		new_error(OUT_OF_BOUNDS_ERROR, "Hash hors limites dans get_same_case_list.");
		return NULL;
	}

	return &(grid->map[hash]);
}

void remove_part_cell_grid(grid_t *grid, part_list_cell_t *cell)
{
	int i = 0;

	cell->next->prev = cell->prev;
	if(cell->prev == NULL)
	{
		for(i = 0; i < grid->size; i++)
		{
			if(grid->map[i].first == cell)
			{
				grid->map[i].first = cell->next;
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
	append_part_cell_list(&(grid->map[hash]), cell);
	return 0;
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
	int i = 0, j = 0, k = 0;
	int soc = model->size_of_chunk;
	double h = model->h;

	model->num_chunk++;
	if(model->num_chunk > model->max_chunk)
	{
		particule_t **new_chunk_list = malloc((model->max_chunk + 8) * sizeof(particule_t*));
		if(new_chunk_list == NULL)
		{
			new_error(MEMORY_ALLOC_ERROR,
				"Erreur d'allocation d'un tableau de pointeurs dans add_chunk.");
			return 1;
		}

		for(i = 0; i < model->num_chunk - 1; i++)
			new_chunk_list[i] = model->chunk_list[i];

		model->chunk_list = new_chunk_list;
		model->max_chunk += 8;
	}

	particule_t *new_chunk = malloc(soc * soc * soc * sizeof(particule_t));
	if(new_chunk == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR,
			"Erreur d'allocation d'un tableau de particule_t dans add_chunk.");
		return 1;
	}

	for(i = 0; i < soc; i++)
	{
		for(j = 0; j < soc; j++)
		{
			for(k = 0; k < soc; k++)
			{
				init_vect(&(new_chunk[(i*soc + j)* soc + k].speed), 0, 0, 0);
				init_vect(&(new_chunk[(i*soc + j)* soc + k].pos),
					pos->x + (double) i * h, pos->y + (double) j * h, pos->z + (double) k * h);
				insert_part_grid(&(model->part_grid), &(new_chunk[(i*soc + j)* soc + k]));
			}
		}
	}

	model->chunk_list[model->num_chunk - 1] = new_chunk;
	return 0;
}

void apply_gravity(model_t *model, double delta)
{
	int i = 0, j = 0;
	vect_t gravity_action;
	particule_t *part = NULL;

	copy_vect(&gravity_action, &(model->gravity));
	mul_vect(&gravity_action, delta);

	for(i = 0; i < model->num_chunk; i++)
	{
		for(j = 0; j < model->size_of_chunk; j++)
		{
			part = &(model->chunk_list[i][j]);
			add_vect(&(part->speed), &gravity_action);
		}
	}

}

/*
* L'algorithme de cette fonction est inspirée du pseudo-code
* d'un projet d'élèves de l'Ensimag disponible sur le site de l'école.
* Cf le compte-rendu de ce projet.
*/
int apply_viscosity(model_t *model, double delta)
{
	int i = 0, j = 0, k = 0;
	particule_t *part = NULL, *neighbor = NULL;
	vect_t viscosity_action, r_ij, u_ij, v_ji;
	double q, u;
	part_list_t *list = NULL;
	part_list_cell_t *cell = NULL;
	vect_t pos, delta_x, delta_y, delta_z;

	double sigma = model->sigma;
	double beta = model->beta;
	double h = model->h;

	init_vect(&delta_x, model->part_grid.delta, 0, 0);
	init_vect(&delta_y, 0, model->part_grid.delta, 0);
	init_vect(&delta_z, 0, 0, model->part_grid.delta);

	//Pour toute particule part
	for(i = 0; i <model->num_chunk; i++)
	{
		int size = model->size_of_chunk;
		for(j = 0; j < size*size*size; j++)
		{
			part = &(model->chunk_list[i][j]);

			// pour toute particule neighbor voisine de part
			// sauf celles pour lesquelles ont a déjà rencontré le couple(part, neighbor)
			// pour éviter d'appliquer deux fois la transformation
			for(k = 0; k < 4; k++)
			{
				switch(k)
				{
					case 0:
						copy_vect(&pos, &(part->pos));
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;

						//On ignore les particules précédant part
						while(cell->part != part)
						{
							if(cell == NULL)
							{
								new_error(LOGIC_ERROR,
									"Une particule n'est pas gérée par la structure de partitionnement.");
								return 1;
							}

							cell = cell->next;
						}
						cell = cell->next;
						break;
					case 1:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_x);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 2:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_y);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 3:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_z);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
				}

				while(cell != NULL)
				{
					neighbor = cell->part;

					copy_vect(&r_ij, &(neighbor->pos));
					sub_vect(&r_ij, &(part->pos));
					q = length_vect(&r_ij) / h;

					if(q < 1)
					{
						copy_vect(&u_ij, &r_ij);
						normalize_vect(&u_ij);

						copy_vect(&v_ji, &(part->speed));
						sub_vect(&v_ji, &(neighbor->speed));

						u = scalar_product_vect(&v_ji, &u_ij);

						if(u > 0)
						{
							copy_vect(&viscosity_action, &u_ij);
							mul_vect(&viscosity_action, delta * (1 - q) * (sigma*u + beta*u*u));
							mul_vect(&viscosity_action, 1/2);

							sub_vect(&(part->speed), &viscosity_action);
							add_vect(&(neighbor->speed), &viscosity_action);
						}
					}
					cell = cell->next;
				}
			}
		}
	}

	return 0;
}

int apply_double_intensity_relaxation(model_t *model, double delta)
{
	int i = 0, j = 0, l = 0;
	particule_t *part = NULL, *neighbor = NULL;
	vect_t pos, delta_x, delta_y, delta_z;
	vect_t r_ij, u_ij, relax_action;
	part_list_t *list = NULL;

	double q = 0, buffer = 0;

	double k = model->k;
	double k_neigh = model->k_neigh;

	double dens, dens_neigh, press, press_neigh;

	double dens0 = model->dens0;

	double h = model->h;

	part_list_cell_t *cell = NULL; 

	init_vect(&delta_x, model->part_grid.delta, 0, 0);
	init_vect(&delta_y, 0, model->part_grid.delta, 0);
	init_vect(&delta_z, 0, 0, model->part_grid.delta);

	for(i = 0; i < model->num_chunk; i++)
	{
		int size = model->size_of_chunk;
		for (j = 0; j < size*size*size; ++j)
		{
			part = &(model->chunk_list[i][j]);

			dens = 0;
			dens_neigh = 0;

			for(l = 0; l < 7; l++)
			{
				switch(l)
				{
					case 0:
						copy_vect(&pos, &(part->pos));
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 1:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_x);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 2:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_y);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 3:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_z);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 4:
						copy_vect(&pos, &(part->pos));
						sub_vect(&pos, &delta_x);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 5:
						copy_vect(&pos, &(part->pos));
						sub_vect(&pos, &delta_y);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 6:
						copy_vect(&pos, &(part->pos));
						sub_vect(&pos, &delta_z);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
				}

				while(cell != NULL)
				{
					neighbor = cell->part;

					copy_vect(&r_ij, &(neighbor->pos));
					sub_vect(&r_ij, &(part->pos));
					q = length_vect(&r_ij) / h;

					if(q < 1)
					{
						buffer = 1 - q;
						dens += buffer * buffer;
						dens_neigh += buffer * buffer * buffer;
					}

					cell = cell->next;
				}
			}

			press = k * (dens - dens0);
			press_neigh = k_neigh * dens_neigh;

			for(l = 0; l < 7; l++)
			{
				switch(l)
				{
					case 0:
						copy_vect(&pos, &(part->pos));
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 1:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_x);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 2:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_y);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 3:
						copy_vect(&pos, &(part->pos));
						add_vect(&pos, &delta_z);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 4:
						copy_vect(&pos, &(part->pos));
						sub_vect(&pos, &delta_x);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 5:
						copy_vect(&pos, &(part->pos));
						sub_vect(&pos, &delta_y);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
					case 6:
						copy_vect(&pos, &(part->pos));
						sub_vect(&pos, &delta_z);
						list = get_same_case_list(&(model->part_grid), &pos);
						if(list == NULL)
							return 1;
						cell = list->first;
						break;
				}

				while(cell != NULL)
				{
					neighbor = cell->part;

					copy_vect(&r_ij, &(neighbor->pos));
					sub_vect(&r_ij, &(part->pos));
					q = length_vect(&r_ij) / h;
					copy_vect(&u_ij, &r_ij);
					normalize_vect(&u_ij);

					if(q < 1)
					{
						copy_vect(&relax_action, &u_ij);
						buffer = 1 - q;
						mul_vect(&relax_action,
							delta*delta*(press*buffer + press_neigh*buffer*buffer));
						mul_vect(&relax_action, 1/2);
						add_vect(&(neighbor->pos), &relax_action);
						sub_vect(&(part->pos), &relax_action);
					}

					cell = cell->next;
				}
			}
		}
	}

	return 0;
}

void apply_collision(model_t *model, double delta)
{
	int i = 0, j = 0;
	particule_t *part = NULL;

	for(i = 0; i < model->num_chunk; i++)
	{
		int size = model->size_of_chunk;
		for (j = 0; j < size*size*size; ++j)
		{
			part = &(model->chunk_list[i][j]);

			if(part->pos.x <= 0)
			{
				part->speed.y *= model->coeff_frot;
				part->speed.z *= model->coeff_frot;

				part->pos.x = 0;
			}
			else if(part->pos.x >= model->part_grid.delta * model->part_grid.size)
			{
				part->speed.y *= model->coeff_frot;
				part->speed.z *= model->coeff_frot;

				part->pos.x = model->part_grid.delta * model->part_grid.size;
			}

			if(part->pos.y <= 0)
			{
				part->speed.x *= model->coeff_frot;
				part->speed.z *= model->coeff_frot;

				part->pos.y = 0;
			}
			else if(part->pos.y >= model->part_grid.delta * model->part_grid.size)
			{
				part->speed.x *= model->coeff_frot;
				part->speed.z *= model->coeff_frot;

				part->pos.y = model->part_grid.delta * model->part_grid.size;
			}

			if(part->pos.z <= 0)
			{
				part->speed.y *= model->coeff_frot;
				part->speed.x *= model->coeff_frot;

				part->pos.z = 0;
			}
			else if(part->pos.z >= model->part_grid.delta * model->part_grid.size)
			{
				part->speed.y *= model->coeff_frot;
				part->speed.x *= model->coeff_frot;

				part->pos.z = model->part_grid.delta * model->part_grid.size;
			}
		}
	}
}

int update_model(model_t *model, event_t *event, double delta)
{
	int error = 0;

	apply_gravity(model, delta);

	error = apply_viscosity(model, delta);

	error |= apply_double_intensity_relaxation(model, delta);

	apply_collision(model, delta);

	return error;
}

/*
void update_model(model_t *model, event_t *event, double delta)
{
	particule_t *part, *neighbor;
	vect_t gravity_action, r_ij, u_ij, v_ji, viscosity_action, relax_action;
	double h = model->h;
	double q, u;
	double dens, dens_neigh, press, press_neigh;
	double dens0 = model->dens0;
	double k = model->k;
	double k_neigh =  model->k_neigh;
	double buffer;

	apply_viscosity_and_gravity(model, delta);
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
*/
