#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
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

        new_view->socket = -1;

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
    printf("    -> aquarium loaded (%d display view) ! \n", aquarium->num_views);
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

    a->views = (View *)realloc(a->views, (a->num_views + 1) * sizeof(View));
    View newView;
    strcpy(newView.name, name);
    newView.width = width;
    newView.height = height;
    newView.coord.x = x;
    newView.coord.y = y;
    newView.socket = -1;

    // Ajouter la nouvelle vue à l'aquarium
    a->views[a->num_views] = newView;
    a->num_views++;

    printf("    -> View %s added.\n", name);
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

    printf("    -> View %s deleted.\n", viewName);
}

void saveAquarium(Aquarium *a, char *aquariumName)
{

    // Ouvrir le fichier en mode écriture
    char *extension = ".txt";
    char *filename = (char *)malloc(strlen(aquariumName) + strlen(extension) + 1);
    strcpy(filename, aquariumName);
    strcat(filename, extension);

    int fd = open(filename, O_CREAT | O_RDWR, 0666);

    // Écrire les dimensions de l'aquarium dans le fichier
    dprintf(fd, "%dx%d\n", a->dimensions[0], a->dimensions[1]);

    // Écrire les vues de l'aquarium dans le fichier
    for (int i = 0; i < a->num_views; i++)
    {
        dprintf(fd, "%s %dx%d+%d+%d\n", a->views[i].name, a->views[i].width, a->views[i].height, a->views[i].coord.x, a->views[i].coord.y);
    }

    close(fd);
    free(filename);

    printf("    -> Aquarium saved (%d display view)!\n", a->num_views);
}

int addFish(Aquarium *a, char *viewName, char *name, int height, int weight, char *mobilityPattern)
{

    int test = 0;
    Fish newFish;
    strcpy(newFish.name, name);
    newFish.weight = weight;
    newFish.height = height;
    strcpy(newFish.mobilityPattern, mobilityPattern);
    for (int i = 0; i < a->num_views; i++)
    {
        if (strcmp(a->views[i].name, viewName) == 0)
        {
            a->views[i].fishes[a->views[i].num_fishes] = newFish;
            a->views[i].num_fishes++;
            test = 1;
            break;
        }
    }

    return test;
}

int deleteFish(Aquarium *a, char *viewName, char *name)
{

    int test = 0;

    for (int i = 0; i < a->num_views; i++)
    {
        if (strcmp(a->views[i].name, viewName) == 0)
        {
            for (int j = 0; j < a->views[i].num_fishes; j++)
            {
                if (strcmp(a->views[i].fishes[j].name, name) == 0)
                {
                    for (int k = j; k < a->views[i].num_fishes - 1; k++)
                    {
                        a->views[i].fishes[k] = a->views[i].fishes[k + 1];
                    }
                    a->views[i].num_fishes--;
                    test = 1;
                    break;
                }
            }
            break;
        }
    }

    return test;
}

char *authenticate(char *id, Aquarium *aquarium, intptr_t socket)
{

    int i;

    if (id != NULL)
    {

        for (i = 0; i < aquarium->num_views; i++)
        {
            if (strcmp(aquarium->views[i].name, id) == 0 && aquarium->views[i].socket == -1)
            {
                aquarium->views[i].socket = socket;

                return id;
            }
        }
    }

    int j;
    for (j = 0; j < aquarium->num_views; j++)
    {
        if (aquarium->views[j].socket == -1)
        {

            aquarium->views[j].socket = socket;

            return aquarium->views[j].name;
        }
    }

    return NULL;
}

void disconnect(Aquarium *aquarium, intptr_t client_socket)
{
    int i;
    for (i = 0; i < aquarium->num_views; i++)
    {
        if (aquarium->views[i].socket == client_socket)
        {
            aquarium->views[i].socket = -1;
            break;
        }
    }

    // remove fish
    if(aquarium->views[i].num_fishes != 0){
        aquarium->views[i].num_fishes = 0;
        free(aquarium->views[i].fishes);
    }
   
}

int verifRegex(char *buffer, char *pattern)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    int len = strcspn(buffer, "\n");
    buffer[len] = '\0';

    /* Compile regular expression */
    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex");
    }

    /* Execute regular expression */
    reti = regexec(&regex, buffer, 0, NULL, 0);

    if (!reti)
    {
        return 1;
    }
    else if (reti == REG_NOMATCH)
    {
        return 0;
    }
    else
    {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Erreur lors de l'exécution de la recherche: %s\n", msgbuf);
        return 0;
    }
}

char *extractString(char *buffer, char *pattern)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    int len = strcspn(buffer, "\n");
    buffer[len] = '\0';

    char *word = NULL;

    regmatch_t match[2];

    reti = regcomp(&regex, pattern, REG_EXTENDED);

    if (reti)
    {
        fprintf(stderr, "Could not compile regex");
    }

    reti = regexec(&regex, buffer, 2, match, 0);

    if (!reti)
    {
        int start = match[1].rm_so;
        int end = match[1].rm_eo;
        int size = end - start;
        word = (char *)malloc(size + 1);
        strncpy(word, buffer + start, size);
        word[size] = '\0';
        return word;
    }
    else if (reti == REG_NOMATCH)
    {
        return NULL;
    }
    else
    {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Erreur lors de l'exécution de la recherche: %s\n", msgbuf);
        return NULL;
    }
}
