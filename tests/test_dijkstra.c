#include "dijkstra.h"
#include "map_loader.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "%s <file>\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        fprintf(stderr, "could not open file %s for reading\n", argv[1]);
        return 1;
    }

    int w, h;
    char** map = create_map(f, &w, &h);
    
    vec2 source = {13, 17};
    vec2 target = {13, 9};
    
    entities_weights c = {
        1, 1, 1, {1, 1, 1, 1}
    };
    
    graph g = generate_graph(map, w, h, c);
    path_result p = distance_nearest_entity(g, source, target);
    
    printf("Source: [%d,%d], Destination: [%d,%d]\n", source.x, source.y, target.x, target.y);
    
    if (p.size == -1)
    {
        puts("No path found");
    }
    else
    {
        printf("Nearest distance: %d\n", p.size);
        printf("Path: ");
        
        for (int i = 0; i < p.size; i++)
        {
            printf("[%d,%d] ", p.path[i].x, p.path[i].y);
        }
        
        putchar('\n');
        putchar('\n');
        
        map[source.y][source.x] = '#';
        map[target.y][target.x] = 'X';
        
        for (int i = 0; i < p.size; i++)
        {
            map[p.path[i].y][p.path[i].x] = 'o';
        }
        
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                putchar(map[y][x]);
            }
            putchar('\n');
        }
        
        putchar('\n');
    }
    
    dispose_result(p);
    dispose_graph(g);
    destroy_map(map, w, h);

    return 0;
}
