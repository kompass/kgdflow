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

typedef struct vect2d
{
	int x, y;

} vect2d_t;

typedef struct button
{
	vect2d_t pos;
	vect2d_t size;
	void (*callback)(model_t*);

} button_t;

typedef struct  button_list
{
	int size;
	button_t **tab;

} button_list_t;

typedef struct view
{
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

typedef struct event
{
	int exit_wanted;
	int key;

} event_t;

view_t* init_view(config_t *conf);
double temporize(config_t *conf);
void update_view(view_t *view, model_t *model, event_t *event);
void init_event(event_t *event);
void get_event(event_t *event, view_t *view);
void close_view(view_t *view);
void create_rec(double but_height);
void create_texture(view_t *view);
//Uint32 get_pixel(view_t *view, int x, int y);
//void set_pixel(view_t *view, int x, int y, Uint32 pixel);
//void set_transp_pixel(view_t *view, int x, int y, Uint32 pixel, int coef);

#endif