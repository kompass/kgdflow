#ifndef VIEW_KGDFLOW_HEADER
#define VIEW_KGDFLOW_HEADER

#include "config.h"
#include "model.h"

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

typedef struct view
{
	SDL_Surface *screen;
	double part_size;
	double angle1,angle2,d;
} view_t;

typedef struct event
{
	int exit_wanted;
	int up, down, left, right, forward, backward;

} event_t;

view_t* init_view(config_t *conf);
double temporize(config_t *conf);
void update_view(view_t *view, model_t *model, event_t *event);
void init_event(event_t *event);
void get_event(event_t *event, view_t *view);
void close_view(view_t *view);
//Uint32 get_pixel(view_t *view, int x, int y);
//void set_pixel(view_t *view, int x, int y, Uint32 pixel);
//void set_transp_pixel(view_t *view, int x, int y, Uint32 pixel, int coef);

#endif