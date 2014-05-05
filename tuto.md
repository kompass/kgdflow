Tuto de Nico
====
**Ici on apprend en s'amusant !**


-------

Parcourir l'ensemble des particules
----
Dans la fonction update_view, tu as une structure de type ```model_t``` passée en paramètre au travers du pointeur ```model_t *model```. Le header ```model.h``` est déjà inclu, donc tu peux utiliser la structure sans problème.
Voici le code pour parcourir toutes les particules et acceder à leur vecteur position:

```
particule_t *part = NULL;
int i=0, j=0;

for(i = 0; i < model->num_chunk; i++)
{
    for(j = 0; j < model->size_of_chunk; j++)
    {
        part = &(model->chunk_list[i][j]);
        // Voilà, là tu parcours toutes les particules,
        // Pour acceder à sa position, tu as part->pos.x, part->pos.y et part->pos.z
        // C'est ici que tu dois écrire le code pour afficher une particule (genre appeler une fonction
        // qui prend un pointeur vers une particule en paramètre et qui l'affiche).
    }
}
```

Voilà !

Avoir la taille du monde
----

```
model->part_grid.size // Voilà la taille d'un coté du monde (le monde est un cube).
```