#ifndef VIEW_KGDFLOW_HEADER
#define VIEW_KGDFLOW_HEADER

#include "config.h"
#include "model.h"

#include <SDL/SDL.h>

typedef struct view
{
	SDL_Surface *screen;
} view_t;

typedef struct event
{

} event_t;

view_t* init_view(config_t *conf);
void update_view(view_t *view, model_t *model, event_t *event);
event_t* get_event(view_t *view);
void close_view(view_t *view);
Uint32 get_pixel(view_t *view, int x, int y);
void set_pixel(view_t *view, int x, int y, Uint32 pixel);
void set_transp_pixel(view_t *view, int x, int y, Uint32 pixel, int coef);

#endif