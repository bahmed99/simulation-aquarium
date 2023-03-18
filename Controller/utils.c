#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

Aquarium* loadAquarium(char *AquariumName){

    char *extension = ".txt";
    char *filename = (char *)malloc(strlen(AquariumName) + strlen(extension) + 1);

    // Copier le nom de l'aquarium dans le nom du fichier
    strcpy(filename, AquariumName);

    // Ajouter l'extension .txt au nom du fichier
    strcat(filename, extension);

    // Ouvrir le fichier en mode lecture
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        /** TODO: afficher dans un fichier de log **/
        printf("Erreur: impossible d'ouvrir le fichier.\n");

        return NULL;
    }

    // Lecture des dimensions de l'aquarium à partir du fichier
    int dimensions[2];
    fscanf(fp, "%dx%d", &dimensions[0], &dimensions[1]);

    // Création d'une variable de type Aquarium et initialisation des dimensions
    Aquarium* aquarium = (struct Aquarium *)malloc(sizeof(struct Aquarium));
    aquarium->num_views = 0;
    strcpy(aquarium->name, AquariumName);
    aquarium->dimensions[0] = dimensions[0];
    aquarium->dimensions[1] = dimensions[1];

    // Lecture des vues à partir du fichier
    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[0] == '\n')
        {
            continue;
        }

        if (line[0] >= '0' && line[0] <= '9')
        {
            continue;
        }

        // Création d'une nouvelle vue et initialisation de ses membres à partir de la ligne lue
        View *new_view = (struct View *)malloc(sizeof(struct View));
        sscanf(line, "%s %dx%d+%d+%d", new_view->name, &new_view->width, &new_view->height, &new_view->coord.x, &new_view->coord.y);

        // Ajout de la vue au tableau de vues
        if (aquarium->num_views == 0)
        {
            aquarium->views = (View *)malloc(sizeof(View));
        }
        else
        {
            aquarium->views = (View *)realloc(aquarium->views, (aquarium->num_views + 1) * sizeof(View));
        }
        aquarium->views[aquarium->num_views] = *new_view;
        aquarium->num_views++;
    }

    // Fermer le fichier
    fclose(fp);

    return aquarium;
}