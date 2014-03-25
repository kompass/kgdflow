#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "config.h"
#include "view.h"
#include "model.h"

double temporize(config_t *conf)
{
	
}

int main(int argc, char *argv[])
{
	config_t *conf = parse_args(argc, argv);

	view_t *view = init_view(conf);

	model_t *model = init_model(conf);

	int run = 1;
	double delta_time = 0;

	while(run)
	{
		delta_time = temporize(conf);
		event_t *event = get_event(view);
		update_model(model, event, delta_time);
		update_view(view, model, event);
	}

	close_conf(conf);
	close_view(view);
	close_model(model);
}