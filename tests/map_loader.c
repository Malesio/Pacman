#include "map_loader.h"

#include <stdlib.h>
#include <string.h>

void get_map_size(FILE* f, int* w, int* h)
{
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    
    char line[80];
    
    fgets(line, 80, f);
    
    *w = strlen(line) - 1;
    *h = size / *w;
    
    rewind(f);
}

char** create_map(FILE* f, int* w, int* h)
{
    int i;
    
    get_map_size(f, w, h);
    
    char** map = malloc((*h) * sizeof(char*));
    char line[80];
    
    for (i = 0; i < *h; i++)
    {
        map[i] = malloc(*w);
        
        fgets(line, 80, f);
        
        memcpy(map[i], line, *w);
    }
    
    fclose(f);
    
    return map;
}

void destroy_map(char** map, int w, int h)
{
    int i;
    
    for (i = 0; i < h; i++)
        free(map[i]);
    
    free(map);
}

