#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

Aquarium *loadAquarium(char *AquariumName)
{

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
    Aquarium *aquarium = (struct Aquarium *)malloc(sizeof(struct Aquarium));
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
    printf("aquarium loaded (%d display view) ! \n" , aquarium->num_views);
    return aquarium;
}

void showAquarium(const Aquarium *aquarium)
{
    // Afficher les dimensions de l'aquarium
    printf("%dx%d\n", aquarium->dimensions[0], aquarium->dimensions[1]);

    // Pour chaque vue de l'aquarium, afficher son nom et ses coordonnées
    for (int i = 0; i < aquarium->num_views; i++)
    {
        printf("%s ", aquarium->views[i].name);
        printf("%dx%d+%d+%d\n", aquarium->views[i].width, aquarium->views[i].height, aquarium->views[i].coord.x, aquarium->views[i].coord.y);
    }
}

void addView(Aquarium *a, const char *name, int width, int height, int x, int y)
{
    // Vérifier que l'aquarium a suffisamment de place pour une nouvelle vue
    // if (a->num_views >= MAX_VIEWS)
    // {
    //     printf("Impossible d'ajouter une nouvelle vue : l'aquarium est complet.\n");
    //     return;
    // }

    // Créer une nouvelle vue avec les paramètres donnés
    View newView;
    strcpy(newView.name, name);
    newView.width = width;
    newView.height = height;
    newView.coord.x = x;
    newView.coord.y = y;

    // Ajouter la nouvelle vue à l'aquarium
    a->views[a->num_views] = newView;
    a->num_views++;

    printf("View %s added.\n", name);
}

void deleteView(Aquarium *a, char *viewName)
{
    int index = -1;

    // Rechercher l'index de la vue à supprimer
    for (int i = 0; i < a->num_views; i++)
    {
        if (strcmp(a->views[i].name, viewName) == 0)
        {
            index = i;
            break;
        }
    }

    // Vérifier si la vue a été trouvée
    if (index == -1)
    {
        printf("Erreur : la vue %s n'existe pas.\n", viewName);
        return;
    }

    // Supprimer la vue à l'index spécifié
    // free(a->views[index].fishes);
    for (int i = index; i < a->num_views - 1; i++)
    {
        a->views[i] = a->views[i + 1];
    }
    a->num_views--;

    printf("View %s deleted.\n", viewName);
}

void saveAquarium(Aquarium *a, char* aquariumName)
{
    // Ouvrir le fichier en mode écriture
    char *extension = ".txt";
    char *filename = (char *)malloc(strlen(aquariumName) + strlen(extension) + 1);
    strcpy(filename, aquariumName);
    strcat(filename, extension);
    FILE *fp = fopen(filename, "w");

    // Écrire les dimensions de l'aquarium dans le fichier
    fprintf(fp, "%dx%d \n", a->dimensions[0], a->dimensions[1]);

    // Écrire les vues de l'aquarium dans le fichier
    for (int i = 0; i < a->num_views; i++)
    {
        fprintf(fp, "%s %dx%d+%d+%d \n", a->views[i].name, a->views[i].width, a->views[i].height, a->views[i].coord.x, a->views[i].coord.y);
    }

    

    printf("Aquarium saved (%d display view)!\n", a->num_views);

}