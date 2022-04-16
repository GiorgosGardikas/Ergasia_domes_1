#include "funcs.h"
#include <stdlib.h>

int compare_ints(Pointer a, Pointer b){
    return *(int * a) - *(int *)b;
}

int create_it (int value){
    int *p = malloc(sizeof(int));
    *p = value ;
    return p;
}