#ifndef VIEW_KGDFLOW_HEADER
#define VIEW_KGDFLOW_HEADER

#include "config.h"
#include "model.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define NO_KEY 0 
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_FORWARD 5
#define KEY_BACKWARD 6

/**
* \struct vect2d_t
* \brief Structure permettant de créer un vecteur en 2 dimmensions.
*/
typedef struct vect2d
{
	int x, y;

} vect2d_t;

/**
* \struct button_t
* \brief Structure permettant de créer un bouton.
*/
typedef struct button
{
	vect2d_t pos;
	vect2d_t size;
	void (*callback)(model_t*);

} button_t;

/**
* \struct button_list_t
* \brief Structure permettant de créer une liste de boutons.
*/
typedef struct  button_list
{
	int size;
	button_t **tab;

} button_list_t;

/**
* \struct view_t
* \brief Tous les paramètres de la vue sont dans cette structure
*
* Cette structure contient toutes les variables permettant de gerer la vue, c'est à dire 
* l'affichage des particule, du cube representant le monde et des boutons.
*/
typedef struct view
{
	int screen_width;
	int screen_height;
	SDL_Surface *screen;
	double part_size;
	double angle1,angle2,d;
	GLuint cube;
	GLuint id_start_tex;
	GLuint id_stop_tex;
	GLuint id_pause_tex;
	GLuint id_reset_tex;
	button_list_t button_list;
} view_t;

/**
* \struct event_t
* \brief Tous les paramètres de la gestion des évenements sont dans cette structure
*
* Cette structure contiens toutes les variables permettant de gerer les evenements,
* que ce soit le mouvement d'une particule, le clic de la souris ou le mouvement de la
* camera.
*/
typedef struct event
{
	int exit_wanted;
	int key;
	void (*click_callback)(model_t*);

} event_t;

/**
* \fn view_t* init_view(config_t *conf)
* \brief Initialise la vue.
*
* C'est ici que qu'on initialise la SDL ainsi que tout ce qu'on a besoin pour OpenGL.
*/
view_t* init_view(config_t *conf);
/**
* \fn double temporize(config_t *conf)
* \brief Permet d'avoir 25fps au maximum
* 
* Une temporisation est necessaire afin de ne pas surcharger le processeur.
*/
double temporize(config_t *conf);

/**
* \fn void reset_not_implemented(model_t *model)
* \brief Gestion du bouton RESET pour recommencer la simulation.
*/
void reset_not_implemented(model_t *model);

/**
* \fn void update_view(view_t *view, model_t *model, event_t *event)
* \brief Update de la vue.
*
* C'est ici que les calculs de la camera et de l'affichage des particules sont fait en OpenGL.
*/
void update_view(view_t *view, model_t *model, event_t *event);

/**
* \fn void init_event(event_t *event)
* \brief Initialise la structure event
*/
void init_event(event_t *event);

/**
* \fn void get_event(event_t *event, view_t *view)
* \brief Recupère un évenement.
*/
void get_event(event_t *event, view_t *view);

/**
* \fn void close_view(view_t *view)
* \brief détruit la structure view et libère la mémoire utilisée.
*/
void close_view(view_t *view);

/**
* \fn void create_rec(double but_height)
* \brief Crée un rectengle en OpenGL pour afficher les boutons.
*/
void create_rec(double but_height);

/**
* \fn void create_texture(view_t *view)
* \brief Plaque la texture des boutons.
*/
void create_texture(view_t *view);

/**
* \fn void init_vect2d(vect2d_t *vect, int x, int y)
* \brief Initialise un vecteur à deux dimensions.
*/
void init_vect2d(vect2d_t *vect, int x, int y);

/**
* \fn void init_button(button_t *butt, int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*))
* \brief Initialise un nouveau bouton.
*/
void init_button(button_t *butt, int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*));

/**
* \fn button_t* new_button(int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*))
* \brief Crée un nouveau bouton.
*/
button_t* new_button(int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*));

/**
* \fn void init_button_list(button_list_t *list)
* \brief Initialise la liste des boutons.
*/
void init_button_list(button_list_t *list);

/**
* \fn void add_button(button_list_t *list, button_t *butt)
* \brief Ajoute un bouton à la liste.
*/
void add_button(button_list_t *list, button_t *butt);

/**
* \fn int is_in_butt(button_t *butt, int x, int y)
* \brief Permet de savoir si le clic de la souris est dans le bouton.
*/
int is_in_butt(button_t *butt, int x, int y);

/**
* \fn void apply_click(button_list_t *list, int x, int y, event_t *event)
* \brief Appelle la fonction associé à chaque bouton.
*/
void apply_click(button_list_t *list, int x, int y, event_t *event);

/**
* \fn GLuint load_text(char *filename)
* \brief Charge la texture de chaque bouton.
*/

GLuint load_text(char *filename);


#endif