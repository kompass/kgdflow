#ifndef VIEW_KGDFLOW_HEADER
#define VIEW_KGDFLOW_HEADER

#include "config.h"
#include "model.h"

typedef struct view
{

} view_t;

typedef struct event
{

} event_t;

view_t* init_view(config_t *conf);
void update_view(view_t *view, model_t *model, event_t *event);
event_t* get_event(view_t *view);

#endif