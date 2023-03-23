#include <stdio.h>
#include <stdlib.h>
#include "utils.h"




int main() {
    char * name = "aquarium";
    Aquarium* a = loadAquarium(name);
    if (a == NULL) {
        printf("Erreur : impossible de charger l'aquarium\n");
        exit(EXIT_FAILURE);
    }
    showAquarium(a);
    printf("*********** \n");
    addView(a,"N5",400,400,400,200);
    printf("************ \n");
    showAquarium(a);
    printf("*********** \n");
    deleteView(a,"N3");
    printf("*********** \n");
    showAquarium(a);
    return 0;
}
