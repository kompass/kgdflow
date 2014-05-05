#include "config.h"

#include <stdlib.h>
#include "error.h"

config_t* parse_args(int argc, char *argv[])
{
	config_t *new_conf = malloc(sizeof(config_t));

	if(new_conf == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR,
			"Erreur d'allocation de config_t dans parse_args.");
		return NULL;
	}

	new_conf->screen_height = 500;
	new_conf->screen_width = 800;
	new_conf->grid_delta = 10;
	new_conf->grid_size = 10;
	new_conf->size_of_chunk = 10;
	new_conf->gravity = 10;
	new_conf->h = 10;
	new_conf->dens0 = 10;
	new_conf->k = 10;

	return new_conf;
}

void close_conf(config_t *conf)
{
	free(conf);
}