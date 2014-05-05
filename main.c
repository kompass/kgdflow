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

double temporize(config_t *conf)
{

}

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

	view_t *view = init_view(conf);

	model_t *model = init_model(conf);

	int run = 1;
	double delta_time = 0;
	event_t event;

	init_event(&event);

	while(!event.exit_wanted)
	{
		delta_time = temporize(conf);
		get_event(&event, view);
		//update_model(model, event, delta_time);
		update_view(view, model, &event);
	}

	close_conf(conf);
	close_view(view);
	close_model(model);
}