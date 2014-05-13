#ifndef MODEL_KGDFLOW_HEADER
#define MODEL_KGDFLOW_HEADER

#include "config.h"

typedef struct event event_t;

/**
* \struct vect_t
* \brief Vecteur dans l'espace en 3 dimmensions
*
* Les coordonnées sont stockées en double précision.
*/
typedef struct vect
{
	double x, y, z;

} vect_t;

/**
* \struct particule_t
* \brief Particule dans l'espace
*
* Une particule est représentée par sa position et sa vitesse.
*/
typedef struct particule
{
	vect_t pos;
	vect_t speed;

} particule_t;

/**
* \struct part_list_cell_t
* \brief Cellule d'une liste doublement chainée de particules.
*
* Cette cellule permet d'acceder à une particule, et aux cellules suivante et précédente.
*/
typedef struct part_list_cell
{
	particule_t *part;
	struct part_list_cell *prev, *next;

} part_list_cell_t;

/**
* \struct part_list_t
* \brief Entête d'une liste doublement chainée de particules.
*
* Cette entête permet d'acceder à la taille de la liste, ainsi qu'à son premier élement.
*/
typedef struct part_list
{
	part_list_cell_t *first;
	int length;

} part_list_t;

/**
* \struct grid_t
* \brief Grille partitionnant l'espace, permettant une recherche plus rapide des proches voisins.
*
* Cette grille d'une taille fixée permet de partitionner l'espace en partitions de taille donnée,
* et d'acceder à chaque partition en O(1). La complexité de recherche des plus proches voisins approche
* donc O(1), avec une complexité d'ajout et de mise à jour d'élement proche elle aussi de O(1), en tout
* cas bien inférieure à O(n), même dans le pire des cas en pratique (car peut supposer avec quasi
* certitude que durant la similation, le nombre de particules dans un espace donné est limité et même,
* avec de bons parametrages, petit).
*
* La grille partitionne un cube de size*delta unités de coté.
*/
typedef struct grid
{
	double delta;
	int size;
	part_list_t *map;

} grid_t;

/**
* \struct model_t
* \brief Structure englobant toutes les données nécéssaires au bon fonctionnement de la simulation.
*
* Elle contient les paramètres nécéssaires, la liste des particules, et le partitionnement de l'espace.
* La liste des particules est organisée en tableau de tableaux de particules. Ces tableaux de particules
* sont de taille fixe et sont appelés "chunks". Au moment où ces chunks sont créés (ils peuvent l'être
* tout au long de la simulation), ils contiennent un grand ensemble de particules formant un bloc compact
* de particules.
*/
typedef struct model
{
	int size_of_chunk;
	int num_chunk;
	int max_chunk;
	particule_t **chunk_list;
	grid_t part_grid;
	vect_t gravity;
	double h;
	double sigma;
	double beta;
	double dens0;
	double k;
	double k_neigh;
	double coeff_frot;
	
} model_t;


/**
* \fn vect_t* new_vect(double x, double y, double z)
* \brief Créé un nouveau vecteur préalablement initialisé et renvoie son adresse en mémoire.
*/
vect_t* new_vect(double x, double y, double z);

/**
* \fn void init_vect(vect_t *cible, double x, double y, double z)
* \brief Initialise un vecteur vect_t en le modifiant sur place.
*/
void init_vect(vect_t *cible, double x, double y, double z);

/**
* \fn void copy_vect(vect_t *cible, vect_t *other)
* \brief Copie dans cible la valeur de other en le modifiant sur place.
*/
void copy_vect(vect_t *cible, vect_t *other);

/**
* \fn void add_vect(vect_t *cible, vect_t *other)
* \brief Ajoute vectoriellement parlant other à cible en modifiant ce dernier sur place.
*/
void add_vect(vect_t *cible, vect_t *other);

/**
* \fn void sub_vect(vect_t *cible, vect_t *other)
* \brief Soustrait vectoriellement parlant other à cible en modifiant ce dernier sur place.
*/
void sub_vect(vect_t *cible, vect_t *other);

/**
* \fn void mul_vect(vect_t *cible, double coef)
* \brief Multiplie chaque composant du vecteur cible par le scalaire coef en le modifiant sur place.
*/
void mul_vect(vect_t *cible, double coef);

/**
* \fn double length_vect(vect_t *cible)
* \brief Retourne la longueur du vecteur.
*
* La longueur du vecteur est en double précision.
*/
double length_vect(vect_t *cible);



/**
* \fn void init_part(particule_t *cible, vect *pos, vect_t *speed)
* \brief Initialise une particule particule_t en la modifiant sur place.
*/
void init_part(particule_t *cible, vect_t *pos, vect_t *speed);

/**
* \fn void init_part_list(part_list_t* cible)
* \brief Initialise une liste chainée de particules part_list_t en la modifiant sur place.
*
* Après initialisation, la liste chainée est de taille nulle,
* et ne contient donc aucune particule.
*/
void init_part_list(part_list_t* cible);

/**
* \fn void append_part_cell_list(part_list_t *cible, part_list_cell_t *cell)
* \brief Ajoute une cellule part_list_cell_t au début de la liste doublement chainée part_list_t.
*/
void append_part_cell_list(part_list_t *cible, part_list_cell_t *cell);

/**
* \fn int append_part_list(part_list_t *cible, particule_t *part)
* \brief Ajoute une particule particule_t au début de la liste doublement chainée part_list_t.
*
* Cette méthode créé une nouvelle cellule contenant la particule
* et l'ajoute au début de la liste.
*/
int append_part_list(part_list_t *cible, particule_t *part);



/**
* \fn int init_grid(grid_t *grid, config_t *conf)
* \brief Initialise la grille grid_t en la modifiant sur place.
*
* Après initialisation, la grille ne contient aucune particule.
*/
int init_grid(grid_t *grid, config_t *conf);

/**
* \fn int hash_grid(grid_t *grid, vect_t *pos)
* \brief Renvoi un hash sous forme d'entier dépendant d'une position.
*
* Deux particules assez proches (cela dépend du delta de la grille) auront le même hash.
* Deux particules éloignées ont de très faibles chances d'avoir le même hash.
*/
int hash_grid(grid_t *grid, vect_t *pos);


/**
* \fn int part_hash_grid(grid_t *grid, particule_t *part)
* \brief Renvoi le hash correspondant à la position de la particule.
*/
int part_hash_grid(grid_t *grid, particule_t *part);

/**
* \fn int insert_part_grid(grid_t *grid, particule_t *part)
* \brief Ajoute une particule dans la grille à son bon emplacement.
*/
int insert_part_grid(grid_t *grid, particule_t *part);

/**
* \fn void remove_part_cell_grid(grid_t *grid, part_list_cell_t *cell)
* \brief Enlève une cellule contenant une particule de la grille.
*/
void remove_part_cell_grid(grid_t *grid, part_list_cell_t *cell);

/**
* \fn int update_part_cell_grid(grid_t *grid, part_list_cell_t *cell)
* \brief Met à jour la position de la cellule dans la grille en fonction de la particule qu'elle contient.
*
* Cette fonction doit être appellée après chaque modification d'une particule
* afin d'éviter des incohérences dans le partitionnement de l'espace.
*/
int update_part_cell_grid(grid_t *grid, part_list_cell_t *cell);




//double quintic_kernel(vect_t *vect, double h);
//void compute_particule_position(particule_t *part, part_list *neighbors);



/*
* \fn model_t* init_model(config_t *conf)
* \brief Créé, initialise et renvoi une structure model_t.
*
* La structure est initialisée en fonction des paramètres inscrits dans conf.
*/
model_t* init_model(config_t *conf);

/**
* \fn int add_chunk(model_t *model, vect_t *pos)
* \brief Ajoute un bloc de particules à la position spécifiée.
*/
int add_chunk(model_t *model, vect_t *pos);

void apply_gravity(model_t *model, double delta);

int apply_viscosity(model_t *model, double delta);

int apply_double_intensity_relaxation(model_t *model, double delta);

/**
* \fn void update_model(model_t *model, event_t *event, double delta)
*
*/
int update_model(model_t *model, event_t *event, double delta);

void apply_collision(model_t *model, double delta);

/**
* \fn void close_model(model_t *model)
* \brief détruit la structure model et libère la mémoire utilisée.
*/
void close_model(model_t *model);

#endif