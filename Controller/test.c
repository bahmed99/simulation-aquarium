#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#include <regex.h>

// void addFishVisitor(View *v, Fish f, int original_view)
// {

//     if (v->num_fishes_visitors == 0)
//     {
//         v->fishes_visitors = (Fish *)malloc(sizeof(Fish));
//         v->fishes_visitors[0] = f;
//         v->num_fishes_visitors++;
//         //v->fishes_visitors_view = (int *)malloc(sizeof(int));
//         v->fishes_visitors_view[0] = original_view;
//     }
//     else
//     {
//         v->num_fishes_visitors++;
//         v->fishes_visitors = (Fish *)realloc(v->fishes_visitors, v->num_fishes_visitors * sizeof(Fish));
//         v->fishes_visitors[v->num_fishes_visitors - 1] = f;
//         //v->fishes_visitors_view = (int *)realloc(v->fishes_visitors_view, v->num_fishes_visitors * sizeof(int));
//         v->fishes_visitors_view[v->num_fishes_visitors - 1] = original_view;
//     }
   


// }

int main() {
   

    Aquarium *aquarium = loadAquarium("aquarium");
    authenticate("hello in as N1", aquarium, 1);
    authenticate("hello in as N2", aquarium, 2);

    addFish(aquarium,1,"fish" ,61,52,4,3, "RandomWayPoint");

    addFishVisitor(&aquarium->views[0], aquarium->views[0].fishes[0], 1);
    addFishVisitor(&aquarium->views[1], aquarium->views[0].fishes[0], 2);

    printf("num_fishes_visitors: %d\n", aquarium->views[1].num_fishes_visitors);
    

    return 0;
}
