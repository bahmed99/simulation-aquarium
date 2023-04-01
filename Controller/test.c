#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#include <regex.h>


int main() {
    char * name = "aquarium";
    Aquarium* a = loadAquarium(name);
    if (a == NULL) {
        printf("Erreur : impossible de charger l'aquarium\n");
        exit(EXIT_FAILURE);
    }
    showAquarium(a);


    char *ch = authenticate("N1", a, 1);
    printf("%s \n", ch);

    char *ch1 = authenticate("N2", a, 2);
    printf("%s \n", ch1);

    char *ch2 = authenticate("N3", a, 3);
    printf("%s \n", ch2);

    char *ch3 = authenticate(NULL, a, 4);
    printf("%s \n", ch3);

    addView(a, "N5", 10, 10, 10, 10);
    showAquarium(a);

    char *ch4 = authenticate("N5", a, 5);
    printf("%s \n", ch4);

    char nom[] = "John";
    char message[50];
    
    sprintf(message, "Bonjour %s", nom);
    
    printf("%s\n", message);  // Affiche "Bonjour John"
    
    return 0;

   

    return 0;
}
