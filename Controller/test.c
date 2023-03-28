#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#include <regex.h>


int main() {
    // char * name = "aquarium";
    // Aquarium* a = loadAquarium(name);
    // if (a == NULL) {
    //     printf("Erreur : impossible de charger l'aquarium\n");
    //     exit(EXIT_FAILURE);
    // }
    // showAquarium(a);


    // char *ch = authenticate("hello in as N1", a, 1);
    // printf("%s \n", ch);

    // char *ch1 = authenticate("hello in as N2", a, 2);
    // printf("%s \n", ch1);

    // char *ch2 = authenticate("hello in as N3", a, 3);
    // printf("%s \n", ch2);

    // char *ch3 = authenticate("hello in as N4", a, 4);
    // printf("%s \n", ch3);

    // addView(a, "N5", 10, 10, 10, 10);
    // showAquarium(a);

    // char *ch4 = authenticate("hello", a, 5);
    // printf("%s \n", ch4);

     // Chaîne de caractères à vérifier
   // Chaîne de caractères à vérifier
    char str[] = "load aquarium0";

    // Expression régulière pour vérifier si la chaîne commence par "load" suivi d'une seule chaîne de caractères
    char *pattern = "^load [a-zA-Z0-9]+$";

    // Compiler l'expression régulière
    regex_t regex;
    int reti = regcomp(&regex, pattern, REG_EXTENDED);
   
    // Exécuter la recherche sur la chaîne de caractères
    reti = regexec(&regex, str, 0, NULL, 0);
    if (!reti) {
        printf("La chaîne de caractères commence par 'load' suivi d'une seule chaîne de caractères\n");
    } else if (reti == REG_NOMATCH) {
        printf("La chaîne de caractères ne vérifie pas le motif\n");
    } 
    // Libérer la mémoire utilisée par l'expression régulière
    regfree(&regex);

    return 0;
}
