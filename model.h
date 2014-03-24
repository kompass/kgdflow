#ifndef MODEL_KGDFLOW_HEADER
#define MODEL_KGDFLOW_HEADER

#include "config.h"

typedef struct model
{
	
} model_t;

model_t* init_model(config_t *conf);
void update_model(model_t *model, event_t *event, double delta);

#endif