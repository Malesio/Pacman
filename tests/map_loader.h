#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <stdio.h>

char** create_map(FILE* f, int* w, int* h);
void destroy_map(char** map, int w, int h);

#endif // MAP_LOADER_H
