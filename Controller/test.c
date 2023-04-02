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

    addView(a, "view1",100,100,0,0);
    addView(a, "view2",100,100,0,0);
    addView(a, "view3",100,100,0,0);

    showAquarium(a);

    saveAquarium(a,"aquarium2");

   

    return 0;
}
