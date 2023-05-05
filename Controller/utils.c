#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"

// Liste globale des valeurs valides pour mobilityPattern
const char* MobilityPatterns[] = {"RandomWayPoint", "HorinzontalPathWay"};

Aquarium *loadAquarium(char *AquariumName)
{
    logger_init("log_controller.txt");

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

        printf("    -> Impossible d'ouvrir le fichier %s \n", filename);
        logger_log(ERROR, "Impossible d'ouvrir le fichier %s", filename);
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
        sscanf(line, "%s %dx%d+%d+%d", new_view->name, &new_view->coord.x, &new_view->coord.y, &new_view->width, &new_view->height);

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
    logger_log(INFO, "Aquarium %s loaded (%d display view) !", aquarium->name, aquarium->num_views);
    printf("    -> aquarium loaded (%d display view) ! \n", aquarium->num_views);
    logger_close();
    return aquarium;
}

void showAquarium(const Aquarium *aquarium)
{
    logger_init("log_controller.txt");

    if (aquarium != NULL)
    {
        // Afficher les dimensions de l'aquarium
        printf("%dx%d\n", aquarium->dimensions[0], aquarium->dimensions[1]);

        // Pour chaque vue de l'aquarium, afficher son nom et ses coordonnées
        for (int i = 0; i < aquarium->num_views; i++)
        {
            printf("%s ", aquarium->views[i].name);
            printf("%dx%d+%d+%d\n", aquarium->views[i].coord.x, aquarium->views[i].coord.y, aquarium->views[i].width, aquarium->views[i].height);
        }
        logger_log(INFO, "Aquarium %s displayed.", aquarium->name);
    }
    else
    {
        printf("    -> Aquarium is NULL \n");
        logger_log(WARNING, "Aquarium is NULL");
    }
    logger_close();
}

void addView(Aquarium *a, const char *name, int x, int y, int width, int height)
{
    logger_init("log_controller.txt");

    if (a != NULL)
    {

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

        logger_log(INFO, "View %s added.", name);
        printf("    -> View %s added.\n", name);
    }
    else
    {
        printf("    -> Aquarium is NULL \n");
        logger_log(WARNING, "Aquarium is NULL");
    }
    logger_close();
}

void deleteView(Aquarium *a, char *viewName)
{

    logger_init("log_controller.txt");
    if (a != NULL)
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
            logger_log(ERROR, "View %s not found.", viewName);
            return;
        }

        // Supprimer la vue à l'index spécifié
        // free(a->views[index].fishes);
        for (int i = index; i < a->num_views - 1; i++)
        {
            a->views[i] = a->views[i + 1];
        }
        a->num_views--;

        logger_log(INFO, "View %s deleted.", viewName);
        printf("    -> View %s deleted.\n", viewName);
    }
    else
    {
        printf("    -> Aquarium is NULL \n");
        logger_log(WARNING, "Aquarium is NULL");
    }
    logger_close();
}

void saveAquarium(Aquarium *a, char *aquariumName)
{

    logger_init("log_controller.txt");

    if (a != NULL)
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
        logger_log(INFO, "Aquarium %s saved (%d display view)!", a->name, a->num_views);
    }
    else
    {
        printf("    -> Aquarium is NULL \n");
        logger_log(WARNING, "Aquarium is NULL");
    }

    logger_close();
}

int isValidMobilityPattern(const char* mobilityPattern) {
    for (int i = 0; i < sizeof(MobilityPatterns) / sizeof(MobilityPatterns[0]); i++) {
        if (strcmp(mobilityPattern,MobilityPatterns[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

char *addFish(Aquarium *a, int client, char *name, int x, int y, int width, int height, char *mobilityPattern)
{
    logger_init("log_controller.txt");
    char *message;
    if (a != NULL)

    {
        Fish newFish;
        newFish.coord.x = x;
        newFish.coord.y = y;
        strcpy(newFish.name, name);
        newFish.width = width;
        newFish.height = height;
        newFish.mobile = 0;
        if(!isValidMobilityPattern(mobilityPattern)){
            message = "NOK : modèle de mobilité non supporté \n";
            logger_log(WARNING, "modèle de mobilité %s non supporté", mobilityPattern);
            logger_close();
            return message; 
        }
        strcpy(newFish.mobilityPattern, mobilityPattern);
        for (int i = 0; i < a->num_views; i++)
        {
            if (a->views[i].socket == client)
            {

                if (x < 0 || x + (width * a->views[i].width / 100) + a->views[i].coord.x > a->views[i].width || y < 0 || y + (height * a->views[i].height / 100) + a->views[i].coord.y > a->views[i].height)
                {

                    message = "NOK: Les coordonnées du poisson sont en dehors des dimensions de vue\n";
                    logger_log(WARNING, "Fish %s coordinates are out of view %s dimensions", name, a->views[i].name);
                    logger_close();
                    return message;
                }

                int j;
                for (j = 0; j < a->views[i].num_fishes; j++)
                {
                    if (strcmp(a->views[i].fishes[j].name, name) == 0)
                    {
                        message = "NOK : Le nom du poisson existe déjà\n";
                        logger_log(WARNING, "Fish %s already exists", name);
                        logger_close();
                        return message;
                    }
                }
                a->views[i].fishes = (Fish *)realloc(a->views[i].fishes, (a->views[i].num_fishes + 1) * sizeof(Fish));
                a->views[i].fishes[a->views[i].num_fishes] = newFish;
                a->views[i].num_fishes++;

                message = "OK\n";
                logger_log(INFO, "Fish %s added to view %s", name, a->views[i].name);
                logger_close();
                return message;
            }
        }
    }
    message = "NOK : Le client n'a pas de vue\n";
    logger_log(WARNING, "Client %d has no view", client);
    logger_close();
    return message;
}

int deleteFish(Aquarium *a, int client, char *fishName)
{

    int i, j, k;
    View *v;
    Fish *f;

    logger_init("log_controller.txt");

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

                    v->fishes = (Fish *)realloc(v->fishes, v->num_fishes * sizeof(Fish));

                    if (v->fishes == NULL && v->num_fishes > 0)
                    {
                        logger_log(ERROR, "Fish %s not deleted", fishName);
                        logger_close();
                        return -1;
                    }

                    logger_log(INFO, "Fish %s deleted", fishName);
                    logger_close();
                    return 1; // Suppression réussie
                }
            }

            logger_log(WARNING, "Fish %s not found", fishName);
            logger_close();

            return 0; // Poisson non trouvé
        }
    }

    logger_log(WARNING, "Client %d has no view", client);
    logger_close();
    return 0; // Client non trouvé
}

char *authenticate(char *id, Aquarium *aquarium, int socket)
{
    logger_init("log_controller.txt");
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
                    logger_log(INFO, "Client %d connected to view %s", socket, id);
                    logger_close();

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
                logger_log(INFO, "Client %d connected to view %s", socket, aquarium->views[j].name);
                logger_close();

                return aquarium->views[j].name;
            }
        }

        logger_log(WARNING, "No view available for client %d", socket);
        logger_close();

        return NULL;
    }
}

void disconnect(Aquarium *aquarium, int client_socket)
{
    logger_init("log_controller.txt");
    if (aquarium != NULL)
    {
        int i;
        for (i = 0; i < aquarium->num_views; i++)
        {
            if (aquarium->views[i].socket == client_socket)
            {
                aquarium->views[i].socket = -1;
                logger_log(INFO, "Client %d disconnected from view %s", client_socket, aquarium->views[i].name);
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

    logger_close();
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
    logger_init("log_controller.txt");
    int test = 0;
    if (aquarium != NULL)
    {
        char *status = (char *)malloc(5000);
        char *fishes = (char *)malloc(1000);
        char *tmp = (char *)malloc(1000);
        int i, j;

        strcpy(fishes, "");

        for (i = 0; i < aquarium->num_views; i++)
        {
            if (aquarium->views[i].socket == client)

            {
                if (aquarium->views[i].num_fishes == 0)
                {
                    sprintf(status, "OK : Connecté au contrôleur, 0 poisson trouvé\n");
                    logger_log(INFO, "OK : Connecté au contrôleur, 0 poisson trouvé\n");
                    free(fishes);
                    free(tmp);

                    logger_close();
                    return status;
                }

                sprintf(status, "OK : Connecté au contrôleur, %d poissons trouvés\n", aquarium->views[i].num_fishes);
                logger_log(INFO, "OK : Connecté au contrôleur, %d poissons trouvés\n", aquarium->views[i].num_fishes);

                for (j = 0; j < aquarium->views[i].num_fishes; j++)
                {
                    sprintf(tmp, "  Fish %s at %dx%d,%dx%d %s\n", aquarium->views[i].fishes[j].name, aquarium->views[i].fishes[j].coord.x, aquarium->views[i].fishes[j].coord.y, aquarium->views[i].fishes[j].width, aquarium->views[i].fishes[j].height, aquarium->views[i].fishes[j].mobile == 1 ? "started" : "notStarted");
                    strcat(fishes, tmp);
                }
                test = 1;
                break;
            }
        }

        if (test == 0)
        {
            sprintf(status, "NOK : Vous n'êtes pas connecté au contrôleur\n");
            logger_log(ERROR, "ERROR : Vous n'êtes pas connecté au contrôleur\n");
            free(fishes);
            free(tmp);
            logger_close();
            return status;
        }

        strcat(status, fishes);
        logger_close();

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

char *startFish(Aquarium *aquarium, int client, char *fishName)
{

    logger_init("log_controller.txt");
    int test = 0;
    if (aquarium != NULL)
    {
        char *startFish = (char *)malloc(1000);
        int i, j;

        for (i = 0; i < aquarium->num_views; i++)
        {
            if (aquarium->views[i].socket == client)
            {
                for (j = 0; j < aquarium->views[i].num_fishes; j++)
                {
                    if (strcmp(aquarium->views[i].fishes[j].name, fishName) == 0)
                    {
                        if (aquarium->views[i].fishes[j].mobile == 1)
                        {
                            sprintf(startFish, "NOK : Le poisson %s est déjà en mouvement\n", fishName);
                            logger_log(ERROR, "NOK : Le poisson %s est déjà en mouvement\n", fishName);
                            logger_close();
                            return startFish;
                        }
                        else
                        {
                            aquarium->views[i].fishes[j].mobile = 1;
                            sprintf(startFish, "OK : Le poisson %s est en mouvement\n", fishName);
                            logger_log(INFO, "OK : Le poisson %s est en mouvement\n", fishName);
                            logger_close();
                            return startFish;
                        }
                    }
                }
                sprintf(startFish, "NOK : Le poisson %s n'a pas été trouvé\n", fishName);
                logger_log(ERROR, "NOK : Le poisson %s n'a pas été trouvé\n", fishName);
                logger_close();
                return startFish;
            }
        }
        if (test == 0)
        {
            sprintf(startFish, "NOK : Vous n'êtes pas connecté au serveur\n");
            logger_log(ERROR, "NOK : Vous n'êtes pas connecté au serveur\n");
            logger_close();
            return startFish;
        }
    }
}

char *getFishes(Aquarium *aquarium, int client) {
    char *FishList = (char *)malloc(1000);
    int ViewWidth = 0;
    int ViewHeight = 0;
    strcpy(FishList, "list ");
    for (int i = 0; i < aquarium->num_views; i++) {
        if (aquarium->views[i].socket == client) {
            ViewHeight = aquarium->views[i].height;
            ViewWidth = aquarium->views[i].width;
        }
        for (int j = 0; j < aquarium->views[i].num_fishes; j++) {
            Fish fish = aquarium->views[i].fishes[j];
            if (fish.coord.x < ViewWidth && fish.coord.y < ViewHeight) {
                sprintf(FishList, "%s [%s at %dx%d, %dx%d, %s] ", FishList, fish.name, fish.coord.x, fish.coord.y, fish.height, fish.width, "5");
            }
        }
    }
    return FishList;
}


char *getFishesContinuously(Aquarium *aquarium, int client) {
    char *FishList = (char *)malloc(1000);
    int * destination;
    int ViewWidth = 0;
    int ViewHeight = 0;
    strcpy(FishList, "list ");
    for (int i = 0; i < aquarium->num_views; i++) {
        if (aquarium->views[i].socket == client) {
            ViewHeight = aquarium->views[i].height;
            ViewWidth = aquarium->views[i].width;
        }
        for (int j = 0; j < aquarium->views[i].num_fishes; j++) {
            destination = RandomPathWay(aquarium);
            aquarium->views[i].fishes[j].coord.x = 0;
            aquarium->views[i].fishes[j].coord.y = 0;
            Fish fish = aquarium->views[i].fishes[j];
            if (fish.coord.x < ViewWidth && fish.coord.y < ViewHeight) {
                sprintf(FishList, "%s [%s at %dx%d, %dx%d, %s] ", FishList, fish.name, fish.coord.x, fish.coord.y, fish.height, fish.width, "5");
            }
        }
    }
    return FishList;
}

int *RandomPathWay(Aquarium *aquarium)
{
    int *coord = (int *)malloc(2 * sizeof(int));
    int i;
    int j;

    int x = rand() % aquarium->dimensions[0];
    int y = rand() % aquarium->dimensions[1];

    coord[0] = x;
    coord[1] = y;

    return coord;
}
// Fonction qui recherche une entrée dans la table de hachage des fonctions de PathWay
// PathWayFunction findPathWayFunction(char* name) {
//     for (int i = 0; i < PATH_WAY_TABLE_SIZE; i++) {
//         if (strcmp(name, pathWayTable[i].name) == 0) {
//             return pathWayTable[i].function;
//         }
//     }
//     return NULL;
// }

int* applyPathWay(Aquarium* aquarium, char* pathWay){
    // PathWayFunction function = findPathWayFunction(pathWay);
    if (strcmp(pathWay,"RandomPathWay")) {
        return RandomPathWay(aquarium);
    }   
    
}

char *ls(Aquarium *aquarium, int client)
{
    char *ls_msg = (char *)malloc(1000);
    if (aquarium != NULL)
    {
        int i;

        int test = 0;
        for (i = 0; i < aquarium->num_views; i++)
        {
            if (aquarium->views[i].socket == client)
            {
                test = 1;
                char *tmp = (char *)malloc(1000);
                int j;

                int k;
                strcpy(ls_msg, "");
                for (k = 0; k < 3; k++)
                {
                    strcat(ls_msg, "List ");
                    for (j = 0; j < aquarium->views[i].num_fishes; j++)
                    {
                        if (aquarium->views[i].fishes[j].mobile == 1)
                        {
                            int *corr = applyPathWay(aquarium,aquarium->views[i].fishes[j].mobilityPattern);
                            sprintf(tmp, "[%s at %dx%d,%dx%d, 5] ", aquarium->views[i].fishes[j].name, corr[0], corr[1], aquarium->views[i].fishes[j].width, aquarium->views[i].fishes[j].height);
                            strcat(ls_msg, tmp);
                            free(corr);
                        }
                    }
                    strcat(ls_msg, "\n");
                }
                free(tmp);
            }
        }
        if (test == 0)
        {
            sprintf(ls_msg, "NOK : Vous n'avez pas de vue associé\n");
            return ls_msg;
        }

        return ls_msg;
    }
    sprintf(ls_msg, "NOK : l'aquarium n'est pas initilialisé\n");

    return ls_msg;
}