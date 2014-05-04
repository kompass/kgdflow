#include "error.h"

error_t g_error_list = NULL;

void new_error(int error_type, char *comment)
{
	error_t error = malloc(sizeof(error_t));

	if(error == NULL)
	{
		fprintf(stderr,
			"Erreur fatale : Erreur d'allocation mémoire dans le système de gestion d'erreur.\n");

		exit(1);
	}

	error->type = error_type;
	error->comment = malloc((strlen(comment)+1)*sizeof(char));

	if(error->comment = NULL)
	{
		free(error);
		fprintf(stderr,
			"Erreur fatale : Erreur d'allocation mémoire dans le système de gestion d'erreur.\n");

		exit(1);
	}

	strcpy(error->comment, comment);

	error->next = g_error_list;
	g_error_list = error;
}

error_t* get_error_list()
{
	error_t *err = g_error_list;

	g_error_list = NULL;

	return err;
}