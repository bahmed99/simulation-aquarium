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


/**
 * @brief Display an aquarium
 * 
 * @param aquarium
 */
void showAquarium(const Aquarium* aquarium);

/**
 * @brief Add a view to an aquarium
 * 
 * @param a 
 * @param name 
 * @param width 
 * @param height 
 * @param x 
 * @param y 
 */
void addView(Aquarium* a, const char* name, int width, int height, int x, int y);


/**
 * @brief Delete a view from an aquarium
 * 
 * @param a 
 * @param viewName 
 */
void deleteView(Aquarium* a, char* viewName);

/**
 * @brief Save an aquarium to a file
 * 
 * @param aquarium
 */


#endif
