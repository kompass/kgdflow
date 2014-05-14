#ifndef CONFIG_KGDFLOW_HEADER
#define CONFIG_KGDFLOW_HEADER

/**
* \struct config_t
* \brief Tous les paramètres de configuration sont dans cette structure
*
* Cette structure contient tous les paramètres de configuration du programme,
* qui sont récuperés soit dans le fichier de configuration, soit dans le terminal.
* Ils sont classés en trois groupes : screen, physics et technics.
*/
typedef struct config
{
	int screen_height;
	int screen_width;
	double grid_delta;
	int grid_size;
	int size_of_chunk;
	double gravity;
	double h;
	double dens0;
	double k;
	double k_neigh;
	int delay;
	double sigma;
	double beta;
	double coeff_frot;
	
} config_t;

/**
* \fn void print_help(char *program_name, int exit_code)
* \brief Fonction qui affiche l'aide
*/
void print_help(char *program_name, int exit_code);

/**
* \fn int ini_file_handler(void *config, const char *section, const char *name, const char *value);
* \brief Fonction permettant de recuperer la valeur des paramètres dans le fichier de configuration.
*/
int ini_file_handler(void *config, const char *section, const char *name, const char *value);

/**
* \fn config_t* parse_args(int argc, char *argv[])
* \brief Fonction qui affiche l'aide
*/
config_t* parse_args(int argc, char *argv[]);

/**
* \fn void close_conf(config_t *conf)
* \brief détruit la structure config et libère la mémoire utilisée.
*/
void close_conf(config_t *conf);

#endif
