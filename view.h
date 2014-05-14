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
	void (*click_callback)(model_t*);

} event_t;

view_t* init_view(config_t *conf);
double temporize(config_t *conf);
void reset_not_implemented(model_t *model);
void update_view(view_t *view, model_t *model, event_t *event);
void init_event(event_t *event);
void get_event(event_t *event, view_t *view);
void close_view(view_t *view);
void create_rec(double but_height);
void create_texture(view_t *view);
void init_vect2d(vect2d_t *vect, int x, int y);
void init_button(button_t *butt, int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*));
button_t* new_button(int xpos, int ypos, int xsize, int ysize, void (*callback)(model_t*));
void init_button_list(button_list_t *list);
void add_button(button_list_t *list, button_t *butt);
int is_in_butt(button_t *butt, int x, int y);
void apply_click(button_list_t *list, int x, int y, event_t *event);
GLuint load_text(char *filename);


#endif