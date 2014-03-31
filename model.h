#ifndef MODEL_KGDFLOW_HEADER
#define MODEL_KGDFLOW_HEADER

#include "config.h"

typedef struct vect
{
	double x, y, z;

} vect_t;

typedef struct particule
{
	vect_t pos;
	vect_t speed;

} particule_t;

typedef struct part_list_cell
{
	particule_t *part;
	struct part_list_cell *prev, *next;

} part_list_cell_t;

typedef struct part_list
{
	part_list_cell *first;
	int length;

} part_list_t;

typedef struct grid
{
	double delta;
	int size;
	part_list_t *map;

} grid_t;

typedef struct model
{
	int num_of_part;
	particule_t **chunk_list;
	grid_t part_grid;
	
} model_t;


vect_t* new_vect(double x, double y, double z);
void init_vect(vect_t *cible, double x, double y, double z);
void copy_vect(vect_t *cible, vect_t *other);
void add_vect(vect_t *cible, vect_t *other);
void sub_vect(vect_t *cible, vect_t *other);
void mul_vect(vect_t *cible, double coef);

void init_part(particule_t *cible, vect *pos, vect_t *speed);

void init_part_list(part_list_t* cible);
void append_part_cell_list(part_list_t *cible, part_list_cell_t *cell);
int append_part_list(part_list_t *cible, particule_t *part);

int init_grid(grid_t *grid, config_t *conf);
int part_hash_grid(grid_t *grid, particule_t *part);
int insert_part_grid(grid_t *grid, particule_t *part);
void remove_part_cell_grid(grid_t *grid, part_list_cell_t *cell);
int update_part_cell_grid(grid_t *grid, part_list_cell_t *cell);


model_t* init_model(config_t *conf);
void update_model(model_t *model, event_t *event, double delta);
void close_model(model_t *model);

#endif