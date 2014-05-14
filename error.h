#ifndef ERROR_KGDFLOW_HEADER
#define ERROR_KGDFLOW_HEADER

#define MEMORY_ALLOC_ERROR 1
#define SDL_ERROR 2
#define OUT_OF_BOUNDS_ERROR 3
#define LOGIC_ERROR 4

/**
* \struct error_t
* \brief Tous les paramètres de gestion des erreurs sont dans cette structure
*
* Cette structure contient tous les paramètres de gestion des erreurs du programme.
*/
typedef struct error
{
	int type;
	char *comment;
	struct error *next;
	
} error_t;

extern error_t *g_error_list;

/**
* \fn void new_error(int error_type, char *comment)
* \brief Fonction qui creée une nouvelle erreur.
*/
void new_error(int error_type, char *comment);

/**
* \fn error_t* get_error_list()
* \brief Fonction qui donne la liste des erreurs.
*/
error_t* get_error_list();



#endif