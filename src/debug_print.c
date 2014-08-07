
#include "debug_print.h"
#include <stdio.h>
#include <stdlib.h>

#define BARRIER (75)

void print_data_line(char* buffer, int size){

    int num_bins = size / BIN_SIZE;
    int max_found_in_bin = 0;
    int i, j;

    for (i = 0; i < num_bins; i++){
        max_found_in_bin = 0;
        for (j = 0; j < BIN_SIZE; j++){
            if (buffer[BIN_SIZE*i + j] > max_found_in_bin)
                max_found_in_bin = buffer[BIN_SIZE*i + j];
        }
        if (max_found_in_bin > BARRIER)
            printf("X");
        else
            printf(" ");
    }
    printf(" | \n");
}
