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
        new_view->num_fishes = 0;

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

    if (aquarium == NULL)
        return;
    // Afficher les dimensions de l'aquarium
    printf("%dx%d\n", aquarium->dimensions[0], aquarium->dimensions[1]);

    // Pour chaque vue de l'aquarium, afficher son nom et ses coordonnées
    for (int i = 0; i < aquarium->num_views; i++)
    {
        printf("%s ", aquarium->views[i].name);
        printf("%dx%d+%d+%d\n", aquarium->views[i].width, aquarium->views[i].height, aquarium->views[i].coord.x, aquarium->views[i].coord.y);
    }
}

void addView(Aquarium *a, const char *name, int x, int y, int width, int height)
{
    if (a == NULL)
        return;

    a->views = (View *)realloc(a->views, (a->num_views + 1) * sizeof(View));
    View newView;
    strcpy(newView.name, name);
    newView.width = width;
    newView.height = height;
    newView.coord.x = x;
    newView.coord.y = y;
    newView.socket = -1;
    newView.num_fishes = 0;

    // Ajouter la nouvelle vue à l'aquarium
    a->views[a->num_views] = newView;
    a->num_views++;

    printf("    -> View %s added.\n", name);
}

void deleteView(Aquarium *a, char *viewName)
{

    if (a == NULL)
        return;

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
    if (a == NULL)
        return;

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

char* addFish(Aquarium *a, int client, char *name, int x , int y, int height, int weight, char *mobilityPattern)
{   
    char* message;
    if (a != NULL)

    {

        Fish newFish;
        newFish.coord.x = x;
        newFish.coord.y = y;
        strcpy(newFish.name, name);
        newFish.width = weight;
        newFish.height = height;
        newFish.mobile=0;
        strcpy(newFish.mobilityPattern, mobilityPattern);
        for (int i = 0; i < a->num_views; i++)
        {
            if (a->views[i].socket == client)
            {   

                //Vérifier les coordonnées de fish par rapport aux dimensions de la view
                if (x < 0 || x > a->views[i].width || y < 0 || y > a->views[i].height)
                {   
    
                    message = "Error : fish coordinates are out of view dimensions";
                    return message;
                }
                int j;
                for (j = 0; j < a->views[i].num_fishes; j++)
                {
                    if (strcmp(a->views[i].fishes[j].name, name) == 0)
                    {    
                        message = "Error : fish name already exists";
                        return message;
                    }
                }
                a->views[i].fishes = (Fish *)realloc(a->views[i].fishes, (a->views[i].num_fishes + 1) * sizeof(Fish));
                a->views[i].fishes[a->views[i].num_fishes] = newFish;
                a->views[i].num_fishes++;

                message = "OK :Fish added successfully";
                return message;
            }
        }

        message="NOK";
        return message;
    }
}

int deleteFish(Aquarium *a, int client, char *fishName)
{
    int i, j, k;
    View *v;
    Fish *f;

    for (i = 0; i < a->num_views; i++)
    {
        if (a->views[i].socket == client)
        {
            v = &a->views[i];

            for (j = 0; j < v->num_fishes; j++)
            {
                f = &v->fishes[j];

                if (strcmp(f->name, fishName) == 0)
                {
                    // Suppression du poisson
                    for (k = j; k < v->num_fishes - 1; k++)
                    {
                        v->fishes[k] = v->fishes[k + 1];
                    }

                    v->num_fishes--;

                    v->fishes = (Fish*) realloc(v->fishes, v->num_fishes * sizeof(Fish));

                    if (v->fishes == NULL && v->num_fishes > 0) {
                        return -1; 
                    }

                    return 1; // Suppression réussie
                }
            }

            return 0; // Poisson non trouvé
        }
    }

    return 0; // Client non trouvé
}

char *authenticate(char *id, Aquarium *aquarium, int socket)
{
    if (aquarium != NULL)

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
}

void disconnect(Aquarium *aquarium, int client_socket)
{
    if (aquarium != NULL)
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
        if (aquarium->views[i].num_fishes != 0)
        {
            aquarium->views[i].num_fishes = 0;
            free(aquarium->views[i].fishes);
        }
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

char **extractStrings(char *buffer, char *pattern, int num_params)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    int len = strcspn(buffer, "\n");
    buffer[len] = '\0';

    regmatch_t match[num_params + 1];

    reti = regcomp(&regex, pattern, REG_EXTENDED);

    if (reti)
    {
        fprintf(stderr, "Could not compile regex");
        return NULL;
    }

    // Tableau dynamique pour stocker les paramètres extraits
    char **params = (char **)malloc(num_params * sizeof(char *));
    for (int i = 0; i < num_params; i++)
    {
        params[i] = NULL;
    }

    reti = regexec(&regex, buffer, num_params + 1, match, 0);

    if (!reti)
    {
        for (int i = 1; i <= num_params; i++)
        {
            int start = match[i].rm_so;
            int end = match[i].rm_eo;
            int size = end - start;
            params[i - 1] = (char *)malloc(size + 1);
            strncpy(params[i - 1], buffer + start, size);
            params[i - 1][size] = '\0';
        }
    }
    else if (reti == REG_NOMATCH)
    {
        free(params);
        params = NULL;
    }
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Error during regex execution: %s\n", msgbuf);
        free(params);
        params = NULL;
    }

    regfree(&regex);
    return params;
}

char *extractString(char *buffer, char *pattern)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    regmatch_t match[2];
    int len = strcspn(buffer, "\n");
    buffer[len] = '\0';
    char *word = NULL;

    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti)
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Erreur lors de l'exécution de la recherche: %s\n", msgbuf);
        regfree(&regex);
        return NULL;
    }

    reti = regexec(&regex, buffer, 2, match, 0);
    if (!reti && match[1].rm_so != -1 && match[1].rm_eo != -1)
    {
        int start = match[1].rm_so;
        int end = match[1].rm_eo;
        int size = end - start;
        word = (char *)malloc(size + 1);
        if (!word)
        {
            fprintf(stderr, "Erreur lors de l'allocation de mémoire\n");
            regfree(&regex);
            return NULL;
        }
        strncpy(word, buffer + start, size);
        word[size] = '\0';
    }

    regfree(&regex);
    return word;
}

char *status(Aquarium *aquarium, int client)
{
    if (aquarium != NULL)
    {
        char *status = (char *)malloc(1000);
        char *fishes = (char *)malloc(1000);
        char *tmp = (char *)malloc(1000);
        int i, j;

        for (i = 0; i < aquarium->num_views; i++)
        {
            if (aquarium->views[i].socket == client)

            {
                if (aquarium->views[i].num_fishes == 0)
                {
                    sprintf(status, "OK : Connecté au contrôleur, 0 poisson trouvé\n");
                    free(fishes);
                    free(tmp);
                    return status;
                }

                sprintf(status, "OK : Connecté au contrôleur, %d poissons trouvés\n", aquarium->views[i].num_fishes);

                for (j = 0; j < aquarium->views[i].num_fishes; j++)
                {
                    sprintf(tmp, "Fish %s at %dx%d,%dx%d %s\n", aquarium->views[i].fishes[j].name, aquarium->views[i].fishes[j].coord.x, aquarium->views[i].fishes[j].coord.y, aquarium->views[i].fishes[j].width, aquarium->views[i].fishes[j].height, aquarium->views[i].fishes[j].mobile==1 ? "started" : "notStarted");
                    strcat(fishes, tmp);
                }
                break;
            }
        }

        strcat(status, fishes);

        free(fishes);
        free(tmp);
        return status;
    }
}

char *pong(char *port)
{
    char *pong = (char *)malloc(1000);
    sprintf(pong, "pong %s\n", port);
    return pong;
}