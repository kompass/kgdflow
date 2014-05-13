#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "error.h"
#include "ini.h"

void print_help(char *program_name, int exit_code)
{
	printf("Utilisation : %s [options]\n\n\n", program_name);

	printf("-h --help                 Affiche ce message d'aide\n");
	printf("-H --height   number      Hauteur de la fenêtre\n");
	printf("-W --width    number      Largeur de la fenêtre\n");
	printf("-g --gravity  number      Force de gravité en m.s-1\n");
	printf("-d --delay    number      Temps minimum entre deux mises à jour de l'affichage\n");
	printf("-p --partsize number      Diamètre d'une particule\n");
	printf("-s --size     number      Taille du monde\n");
	printf("-D --distance number      Distance entre deux particules\n");
	printf("-c --config   filename    Fichier de configuration\n");

	exit(exit_code);
}

int ini_file_handler(void *config, const char *section, const char *name, const char *value)
{
	config_t *conf = (config_t*) config;

	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

	if(MATCH("screen", "height"))
	{
		conf->screen_height = atoi(value);
		if(conf->screen_height <= 0)
		{
			printf("Erreur: Valeur de screen::height invalide.\n");
			return 0;
		}
	}
	else if(MATCH("screen", "width"))
	{
		conf->screen_width = atoi(value);
		if(conf->screen_width <= 0)
		{
			printf("Erreur: Valeur de screen::width invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "gravity"))
	{
		conf->gravity = strtod(value, NULL);
		if(conf->gravity <= 0)
		{
			printf("Erreur: Valeur de physics::gravity invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "h"))
	{
		conf->h = strtod(value, NULL);
		if(conf->h <= 0)
		{
			printf("Erreur: Valeur de physics::h invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "dens0"))
	{
		conf->dens0 = strtod(value, NULL);
		if(conf->dens0 <= 0)
		{
			printf("Erreur: Valeur de physics::dens0 invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "k"))
	{
		conf->k = strtod(value, NULL);
		if(conf->k <= 0)
		{
			printf("Erreur: Valeur de physics::k invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "k_neigh"))
	{
		conf->k_neigh = strtod(value, NULL);
		if(conf->k_neigh <= 0)
		{
			printf("Erreur: Valeur de physics::k_neigh invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "sigma"))
	{
		conf->sigma = strtod(value, NULL);
		if(conf->sigma <= 0)
		{
			printf("Erreur: Valeur de physics::sigma invalide.\n");
			return 0;
		}
	}
	else if(MATCH("physics", "beta"))
	{
		conf->k_neigh = strtod(value, NULL);
		if(conf->beta <= 0)
		{
			printf("Erreur: Valeur de physics::beta invalide.\n");
			return 0;
		}
	}
	else if(MATCH("technics", "grid_delta"))
	{
		conf->grid_delta = strtod(value, NULL);
		if(conf->grid_delta <= 0)
		{
			printf("Erreur: Valeur de technics::grid_delta invalide.\n");
			return 0;
		}
	}
	else if(MATCH("technics", "grid_size"))
	{
		conf->grid_size = strtod(value, NULL);
		if(conf->grid_size <= 0)
		{
			printf("Erreur: Valeur de technics::grid_size invalide.\n");
			return 0;
		}
	}
	else if(MATCH("technics", "size_of_chunk"))
	{
		conf->size_of_chunk = strtod(value, NULL);
		if(conf->size_of_chunk <= 0)
		{
			printf("Erreur: Valeur de technics::size_of_chunk invalide.\n");
			return 0;
		}
	}
	else if(MATCH("technics", "delay"))
	{
		conf->delay = strtod(value, NULL);
		if(conf->delay <= 0)
		{
			printf("Erreur: Valeur de technics::delay invalide.\n");
			return 0;
		}
	}
	
	return 1;
}

config_t* parse_args(int argc, char *argv[])
{
	const char* const short_options = "hH:W:g:d:p:s:D:";
	const struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"height", 1, NULL, 'H'},
		{"width", 1, NULL, 'W'},
		{"gravity", 1, NULL, 'g'},
		{"delay", 1, NULL, 'd'},
		{"partsize", 1, NULL, 'p'},
		{"size", 1, NULL, 's'},
		{"config", 1, NULL, 'c'},
		{"distance",1,NULL,'D'},
		{NULL, 0, NULL, 0}
	};

	char* conf_filename = "kgdflow.rc";

	int option = 0;
	config_t *new_conf = malloc(sizeof(config_t));

	if(new_conf == NULL)
	{
		new_error(MEMORY_ALLOC_ERROR,
			"Erreur d'allocation de config_t dans parse_args.");
		return NULL;
	}

	new_conf->screen_height = 0;
	new_conf->screen_width = 0;
	new_conf->gravity = 0;
	new_conf->delay = 0;
	new_conf->h = 0;
	new_conf->grid_size = 0;


	while(option != -1)
	{
		option = getopt_long(argc, argv, short_options, long_options, NULL);

		switch(option)
		{
			case 'h':
				print_help(argv[0], 0);
				break;
			case 'H':
				new_conf->screen_height = atoi(optarg);
				if(new_conf->screen_height <= 0)
					print_help(argv[0], 0);
				break;
			case 'W':
				new_conf->screen_width = atoi(optarg);
				if(new_conf->screen_width <= 0)
					print_help(argv[0], 0);
				break;
			case 'g':
				new_conf->gravity = strtod(optarg, NULL);
				if(new_conf->gravity <= 0)
					print_help(argv[0], 0);
				break;
			case 'd':
				new_conf->delay = strtod(optarg, NULL);
				if(new_conf->delay <= 0)
					print_help(argv[0], 0);
				break;
			case 'p':
				new_conf->h = strtod(optarg, NULL);
				if(new_conf->h <= 0)
					print_help(argv[0], 0);
				break;
			case 's':
				new_conf->grid_size = atoi(optarg);
				if(new_conf->grid_size <= 0)
					print_help(argv[0], 0);
				break;
			case 'c':
				conf_filename = strdup(optarg);
				break;
			case 'D':
				new_conf->h= strtod(optarg, NULL);
				if(new_conf->h <= 0)
					print_help(argv[0], 0);
				break;

		}
	}

	if(ini_parse(conf_filename, ini_file_handler, new_conf) < 0)
	{
		printf("Erreur: Impossible de lire le fichier de conf %s.\n", conf_filename);
		return NULL;
	}


	new_conf->coeff_frot = 0.25;

	return new_conf;
}

void close_conf(config_t *conf)
{
	free(conf);
}