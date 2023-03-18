#ifndef _UTILS_H
#define _UTILS_H

/********* Déclaration des structures *********/


// Définition de la structure Fish

struct Fish
{
    char name[50];
    int weight;
    char mobilityPattern[50];

};
typedef struct Fish Fish;



// Définition de la structure Coordinates
struct Coordinates
{
    int x;
    int y;
};
typedef struct Coordinates Coordinates;

// Définition de la structure View
struct View
{
    char name[50];
    struct Coordinates coord;
    int width;
    int height;
    int num_fishes;
    Fish *fishes;
   
};
typedef struct View View;

// Définition de la structure Aquarium
struct Aquarium
{
    char name[50];
    int dimensions[2];
    struct View *views;
    int num_views;
};
typedef struct Aquarium Aquarium;


/********* Déclaration des fonctions *********/

/**
 * @brief Parse a file and return an Aquarium structure
 * 
 * @param AquariumName 
 * @return Aquarium 
 */
Aquarium* loadAquarium(char *AquariumName);


#endif
