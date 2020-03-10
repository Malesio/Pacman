#ifndef DIJKSTRA_H
#define DIJKSTRA_H

// Definition of the direction type, which relies on the enum type compass:
enum compass {NORTH, EAST, SOUTH, WEST};

//  A direction is a value from the enum compass type, ie NORTH, EAST, SOUTH or WEST
typedef enum compass direction;

typedef struct
{
    int x;
    int y;
} vec2;

typedef struct
{
    unsigned char unexplored;
    unsigned char explored;
    unsigned char energizer;
    unsigned char ghosts[4];
} entities_weights;

typedef struct
{
    vec2* path;
    int distance;
    int size;
} path_result;

typedef struct
{
    unsigned int* ptr;
    char** map;
    int w;
    int h;
} graph;

graph generate_graph(char** map, int w, int h, entities_weights weights);
void update_graph(graph g, entities_weights config);

path_result distance_nearest_entity(graph g, vec2 source, vec2 target);

void dispose_result(path_result p);
void dispose_graph(graph g);


#endif // DIJKSTRA_H
