/**
* \file main.c
*
* \brief Programme de simulation de liquide.
*
* \author Nicolas KIRCHNER, Matthieu GOUEL et Deborah DARDOUR
*
* \version 0.1
*
*
* KGDFlow est un programme de simulation de liquide utilisant la méthode de la SPH.
* 
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "config.h"
#include "view.h"
#include "model.h"
#include "error.h"

/**
* \fn int main(int argc, char *argv[])
* \brief Fonction principale.
*
*
* La fonction principale récupère les arguments et le fichier de configuration,
* initialise la vue, le modèle, puis éxécute la boucle de calcul.
* Une fois cette boucle terminée, elle libère la mémoire et finalise l'éxécution.
*
*
*/
int main(int argc, char *argv[])
{
	config_t *conf = parse_args(argc, argv);

	int error = 0;
	error_t *error_cell = NULL;

	if(conf == NULL)
	{
		return 1;
	}

	view_t *view = init_view(conf);

	model_t *model = init_model(conf);

	double delta_time = 0;
	event_t event;

	init_event(&event);

	vect_t chunk_pos;
	init_vect(&chunk_pos, 0.4, 0.4, 0.4);
	add_chunk(model, &chunk_pos);
	//add_chunk(model, &chunk_pos);
	//add_chunk(model, &chunk_pos);
	//add_chunk(model, &chunk_pos);
	//add_chunk(model, &chunk_pos);
	//add_chunk(model, &chunk_pos);


	while(!event.exit_wanted)
	{
		delta_time = temporize(conf);
		get_event(&event, view);

		if(event.click_callback != NULL)
			(*(event.click_callback))(model);

		error = update_model(model, &event, delta_time);

		if(error)
		{
			printf("ERREUR DETECTEE !\n");
			error_cell = get_error_list();
			int error_count = 0;

			while(error_cell != NULL)
			{
				printf("%s\n", error_cell->comment);
				error_cell = error_cell->next;
				error_count++;
				if(error_count >= 10)
				{
					printf("Appuyez sur ENTREE pour voir la suite, q puis ENTREE pour quitter.\n");
					if(getchar() == 'q')
						return 1;
					error_count = 0;
				}
			}
			return 1;
		}

		update_view(view, model, &event);
	}

	close_conf(conf);
	close_view(view);
	close_model(model);

	return 0;
}