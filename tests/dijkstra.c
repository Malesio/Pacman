#include "dijkstra.h"
#include "priority_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef DEBUG
const char* dir_name(int dir)
{
    switch (dir)
    {
        case NORTH:
            return "NORTH";
        case EAST:
            return "EAST";
        case SOUTH:
            return "SOUTH";
        case WEST:
            return "WEST";
        default:
            return "";
    }
}
#endif

void graph_set_weight(graph g, int idx, int dir, unsigned char weight)
{
    g.ptr[idx] &= ~(0xff << (dir * 8));
    g.ptr[idx] |= (weight << (dir * 8));
}

unsigned char graph_get_weight(graph g, int idx, int dir)
{
    return (g.ptr[idx] >> (dir * 8)) & 0xff;
}

vec2 create_vec2(int x, int y)
{
    vec2 p = {x, y};
    return p;
}

vec2 wrap_coordinates(int width, int height, vec2 p)
{
    vec2 n = p;
    
    if (n.y < 0)
        n.y += height;
    if (n.y >= height)
        n.y -= height;
    if (n.x < 0)
        n.x += width;
    if (n.x >= width)
        n.x -= width;
    
    return n;
}

vec2 graph_index_to_coords(int idx, int width)
{
    vec2 r = {idx % width, idx / width};
    return r;
}

int coords_to_graph_index(vec2 pos, int width)
{
    return pos.y * width + pos.x;
}

int graph_get_neighbor_index(int width, int height, int src, int dir)
{
    vec2 p = graph_index_to_coords(src, width);
    
    switch (dir)
    {
        case NORTH:
            p.y--;
            break;
        case EAST:
            p.x++;
            break;
        case SOUTH:
            p.y++;
            break;
        case WEST:
            p.x--;
            break;
    }
    
    return coords_to_graph_index(wrap_coordinates(width, height, p), width);
}

unsigned char get_weight_for_entity(char c, entities_weights config)
{
    unsigned char w;
    
    switch (c)
    {
        case '.':
            w = config.unexplored;
            break;
        case ' ':
            w = config.explored;
            break;
        case 'O':
            w = config.energizer;
            break;
        case '$':
            w = config.ghosts[0];
            break;
        case '%':
            w = config.ghosts[1];
            break;
        case '#':
            w = config.ghosts[2];
            break;
        case '&':
            w = config.ghosts[3];
            break;
        default:
            w = 255;
            break;
    }
    
    return w;
}

void compute_weights(char** map, int w, int h, int x, int y, char* weights, entities_weights config)
{
    vec2 neighbors[] = {
        wrap_coordinates(w, h, create_vec2(x, y - 1)),
        wrap_coordinates(w, h, create_vec2(x + 1, y)),
        wrap_coordinates(w, h, create_vec2(x, y + 1)),
        wrap_coordinates(w, h, create_vec2(x - 1, y))
    };
    
    for (int dir = 0; dir < 4; dir++)
    {
        weights[dir] = get_weight_for_entity(map[neighbors[dir].y][neighbors[dir].x], config);
    }
}

graph generate_graph(char** map, int width, int height, entities_weights config)
{
    graph g;
    
    g.ptr = malloc(width * height * sizeof(unsigned int));
    g.map = map;
    g.w = width;
    g.h = height;
    
    memset(g.ptr, 0, width * height * sizeof(unsigned int));
    
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
        {
            int idx = width * j + i;
            char weights[4];
            
            compute_weights(map, width, height, i, j, weights, config);
            
            for (int dir = 0; dir < 4; dir++)
                graph_set_weight(g, idx, dir, weights[dir]);
        }
        
    return g;
}

void update_graph(graph g, entities_weights config)
{
    for (int i = 0; i < g.w; i++)
        for (int j = 0; j < g.h; j++)
        {
            int idx = g.w * j + i;
            char weights[4];
            
            compute_weights(g.map, g.w, g.h, i, j, weights, config);
            
            for (int dir = 0; dir < 4; dir++)
                graph_set_weight(g, idx, dir, weights[dir]);
        }
    
}

int compare_weights(value left, value right)
{
    return left.weight - right.weight;
}

path_result distance_nearest_entity(graph g, vec2 source, vec2 target)
{
    int w = g.w;
    int h = g.h;
    
    int i, j, dir;
    
    unsigned char* distances = malloc(w * h);
    bool* visited = malloc(w * h);
    int* predecessors = malloc(w * h * sizeof(int));
    bool finished = false;
    bool found = false;
    
    int src = w * source.y + source.x;
    int dest = w * target.y + target.x;
    
    memset(distances, 0xff, w * h);
    memset(predecessors, 0xff, w * h * sizeof(int));
    memset(visited, 0, w * h);
    
    distances[src] = 0;
    
    int current = src;
    
    priority_queue* q = priority_queue_new(compare_weights);
    
    value orig = {src, 0};
    priority_queue_push(q, orig);
    
    while (!finished)
    {
        value c;
        priority_queue_top(q, &c);
        priority_queue_pop(q);
        current = c.index;
        
        if (current == dest)
        {
            found = true;
            finished = true;
        }
        else
        {
            for (dir = 0; dir < 4; dir++)
            {
                int neighbor = graph_get_neighbor_index(w, h, current, dir);
                
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    
                    unsigned char weight = graph_get_weight(g, current, dir);
                    int cost = distances[current] + weight;
                    
                    if (cost < distances[neighbor])
                    {
#ifdef DEBUG
                        printf("Found near neighbor for [%d,%d]: [%d,%d] (dir=%s) {cost=%d,weight=%d}\n",
                            current % w, current / w, neighbor % w, neighbor / w, dir_name(dir), cost, weight
                        );
#endif
                        distances[neighbor] = cost;
                        predecessors[neighbor] = current;
                        
                        value n = {neighbor, cost};
                        priority_queue_push(q, n);
                    }
                }
            }
            
            if (priority_queue_size(q) == 0)
            {
                finished = true;
                found = false;
            }
        }
    }
    
    int shortest_distance = -1;
    int path_size = -1;
    vec2* shortest_path = NULL;
    
    if (found)
    {
        shortest_distance = distances[dest];
        
        int estimated_path_length = 30;
        
        shortest_path = malloc(estimated_path_length * sizeof(vec2));
        
        int pred = dest;
        i = 0;
        
        while (pred != src)
        {
            if (i == estimated_path_length)
            {
                estimated_path_length *= 2;
                shortest_path = realloc(shortest_path, estimated_path_length * sizeof(vec2));
            }
            
            shortest_path[i] = graph_index_to_coords(pred, w);
            pred = predecessors[pred];
            
            i++;
        }
        
        path_size = i;
        
        for (j = 0; j < path_size / 2; j++)
        {
            vec2 tmp = shortest_path[j];
            shortest_path[j] = shortest_path[path_size - 1 - j];
            shortest_path[path_size - 1 - j] = tmp;
        }
    }
    
    free(visited);
    free(distances);
    free(predecessors);
    
    priority_queue_delete(q);
    
    path_result res = {shortest_path, shortest_distance, path_size};
    
    return res;
}

void dispose_result(path_result p)
{
    free(p.path);
}

void dispose_graph(graph g)
{
    free(g.ptr);
}
