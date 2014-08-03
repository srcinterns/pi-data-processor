
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "dbv.h"

void dbv(float* array, int size){
    int i;

    assert(array != NULL);

    for (i = 0; i < size; i++)
        array[i] = 20*logf(array[i]);
}
