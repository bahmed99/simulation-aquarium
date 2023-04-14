#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#include <regex.h>


int main() {
   

    Aquarium *a = malloc(sizeof(Aquarium));
    a=loadAquarium("aquarium");
        authenticate("N1",a,1);

    addFish(a,1,"fish1",1,1,1,1,"pattern1");


    char* msg = status(a,1);
    printf("%s \n",msg);
    free(msg);
    return 0;
}
