// add the needed C libraries below
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand, malloc, realloc, free
#include <stdio.h> // printf
#include <string.h> // memset

// look at the file below for the definition of the direction type
// pacman.h must not be modified!
#include "pacman.h"

// ascii characters used for drawing levels
extern const char PACMAN; // ascii used for pacman
extern const char WALL; // ascii used for the walls
extern const char PATH; // ascii used for the explored paths
extern const char DOOR; // ascii used for the ghosts' door
extern const char VIRGIN_PATH; // ascii used for the unexplored paths
extern const char ENERGY; // ascii used for the energizers
extern const char GHOST1; // ascii used for the ghost 1
extern const char GHOST2; // ascii used for the ghost 2
extern const char GHOST3; // ascii used for the ghost 3
extern const char GHOST4; // ascii used for the ghost 4

// reward (in points) when eating dots/energizers 
extern const int VIRGIN_PATH_SCORE; // reward for eating a dot
extern const int ENERGY_SCORE; // reward for eating an energizer

// put the student names below (mandatory)
const char * binome="Feraux, Elain";

// put the prototypes of your additional functions/procedures below

// ***********************************************************************************
// Linked list structures & functions declaration
// ***********************************************************************************

// A "graph node" is the value being held by the list.
typedef struct
{
    int index;
    int weight;
} value;

// The base type denoting a node in the list.
typedef struct node
{
    value v;
    struct node* next;
} node;

// The base structure representing the linked list.
typedef struct
{
    node* first;
    int size;
} list;

// This type is useful when iterating over the linked list
// (i.e. provides useful abstractions for basic operations).
typedef struct
{
    list* l;
    node** ptr;
} list_iterator;

/**
 * @brief Create an empty list.
 * @return The newly created list
 */
list* list_new();
/**
 * @brief Delete a list, freeing all its nodes.
 * @param l The list to delete
 */
void list_delete(list* l);

/**
 * @brief Accessor to the list size, useful to not rely on the internal
 * representation of the underlying list.
 * @param l A list
 * @return The number of nodes in the list
 */
int list_size(list* l);

/**
 * @brief Append a graph node at the end of the list.
 * @param l The list to append an element to
 * @param v A value to be appended to the list
 * @return 1 if the element could be appended, 0 otherwise
 */
int list_append(list* l, value v);
/**
 * @brief Remove a graph node at the given position in the list.
 * @param l The list to remove an element from
 * @param i The position of the element to remove
 * @return 1 if the element could be removed (i.e. there was an element at
 * the given position), 0 otherwise
 */
int list_remove(list* l, int i);

/**
 * @brief Get an iterator on the beginning of the list.
 * @param l The list on which to iterate
 * @return An iterator pointing to the first node of the list
 */
list_iterator list_iterator_begin(list* l);

/**
 * @brief Determine if we reached the end of the list.
 * @param it The iterator on the current node.
 * @return 1 if the node is NULL, 0 otherwise
 */
int list_iterator_end(list_iterator it);

/**
 * @brief Go to the next element. Always check if there is a
 * next element to the current node, this will lead to null-pointer
 * dereferencing otherwise.
 * @param it An iterator pointing to the current node
 * @return The next node in the list
 */
list_iterator list_iterator_next(list_iterator it);

/**
 * @brief Insert a graph node before the node pointed by the 
 * iterator.
 * @param it The iterator referencing the node to be put after
 * the one created here
 * @return 1 if the value could be inserted at the given position
 * denoted by the iterator, 0 otherwise
 */
int list_insert(list_iterator it, value v);

/**
 * @brief Dereference the iterator and get the graph node held by
 * the current node. This will result in undefined behaviour if the
 * iterator does not reference a valid node.
 * @param it The iterator to dereference
 * @return A graph node
 */
value list_iterator_get(list_iterator it);

// ***********************************************************************************
// Priority queue structures & functions declaration
// ***********************************************************************************

// A priority queue is defined as a list, using the definition above, with a
// comparator used to sort values according to a user-specified criteria.
typedef struct
{
    list* l;
    int (*cmp_func)(value l, value r);
} priority_queue;

/**
 * @brief Create an empty priority queue, using the specified comparator to sort values.
 * @param cmp A function comparing two values in the priority queue
 * @return The newly created priority queue
 */
priority_queue* priority_queue_new(int(*cmp)(value, value));

/**
 * @brief Delete the priority queue, effectively freeing all its elements.
 * @param q The priority queue to delete
 */
void priority_queue_delete(priority_queue* q);

/**
 * @brief Get the number of elements in the priority queue.
 * @param q A priority queue
 * @return The size of the priority queue
 */
int priority_queue_size(priority_queue* q);

/**
 * @brief Enqueue a graph node in the priority queue, and sort it.
 * @param q The priority queue to add a graph node to
 * @param v The graph node to additional
 */
void priority_queue_push(priority_queue* q, value v);

/**
 * @brief Remove the first element of the priority queue,
 * a.k.a the 'lowest' one according to the comparator.
 * @param q The priority queue to pop the front element from
 * @return 1 if the priority queue was not empty, 0 otherwise
 */
int priority_queue_pop(priority_queue* q);

/**
 * @brief Peek the first element of the priority queue.
 * @param q The priority queue to peek the front element from
 * @param v The graph node of the front element, passed by address
 * @return 1 if the priority queue was not empty, 0 otherwise
 */
int priority_queue_top(priority_queue* q, value* v);

// ***********************************************************************************
// Pathfinding structures & functions declaration
// ***********************************************************************************

// A simple type to denote an x-y position.
typedef struct
{
    int x;
    int y;
} vec2;

/**
 * @brief Convenience function to create a position when brace initialisation does not work.
 * @param x The x coordinate
 * @param y The y coordinate
 * @return The position vector
 */
vec2 create_vec2(int x, int y);

/**
 * @brief Useful function to "mirror" a position around when getting
 * out of the map.
 * @param w The map width
 * @param h The map height
 * @param in The position to wrap
 */
vec2 wrap_coordinates(int w, int h, vec2 in);

/**
 * @brief Convert a graph position to two-dimensional coordinates.
 * @param idx The graph position to convert
 * @param w The map width
 * @return The position in the x-y world
 */
vec2 graph_index_to_coords(int idx, int w);

/**
 * @brief Convert a x-y position to a graph index.
 * @param pos The x-y position to convert
 * @param w The map width
 * @return The position in the graph world
 */
unsigned int coords_to_graph_index(vec2 pos, int w);

// A type to configure how the pathfinding will behave by tweaking the weights of
// the different entities in the game.
typedef struct
{
    unsigned char unexplored;
    unsigned char explored;
    unsigned char energizer;
    unsigned char ghost;
} entities_weights;

/**
 * @brief Map the given element on the map to a certain weight.
 * @param c The map element
 * @param weights The weights to assign to elements
 * @return The weight assigned to the specified character
 */
unsigned char get_weight_for_entity(char c, entities_weights weights);

/**
 * @brief Compute the weights for each neighbor of the specified element. Supports mirroring of
 * the map.
 * @param map The game map
 * @param w The map width
 * @param h The map height
 * @param x The x coordinate of the element
 * @param y The y coordinate of the element
 * @param weight_values The computed weights for all four neighbor, must be a pointer to an 4-element array
 * @param weights The weights to assign to elements, delegated to `get_weight_for_entity`
 */
void compute_weights(char** map, int w, int h, int x, int y, char* weight_values, entities_weights weights);

// A simple type to produce a grouped result of the shortest path, and its length.
typedef struct
{
    vec2 next_move;
    int distance;
    int size;
} path_result;

// A simple type to hold the internal representation of the graph used by the 
// following algorithms.
typedef struct
{
    unsigned int* ptr;
    char** map;
    int w;
    int h;
} graph;

/**
 * @brief Set the weight to go to a given neighbor for the specified graph position.
 * @param g The graph to update
 * @param idx The graph position
 * @param dir The neighbor considered
 * @param weight The weight to go to this neighbor
 */
void graph_set_weight(graph g, int idx, direction dir, unsigned char weight);

/**
 * @brief Get the weight to go to a given neighbor for the specified graph position.
 * @param g The graph to analyse
 * @param idx The graph position
 * @param dir The neighbor considered
 * @return The weight to go to this neighbor
 */
unsigned char graph_get_weight(const graph g, int idx, direction dir);

/**
 * @brief Get the graph position of the desired neighbor for the given graph positon.
 * @param w The map width
 * @param h The map height
 * @param idx The graph position of the source
 * @param dir The neighbor considered
 * @return The graph position of the selected neighbor
 */
unsigned int graph_get_neighbor_index(int w, int h, int idx, direction dir);

/**
 * @brief Create a graph that will be usable by the Dijkstra's algorithm.
 * @param map The current game map
 * @param w The map width
 * @param h The map height
 * @return The newly created graph
 */
graph create_graph(char** map, int w, int h);

/**
 * @brief Fill the graph with the given weights for various entities.
 * @param g The graph to update
 * @param weights The weights to apply
 */
void update_graph(graph g, entities_weights weights);

/**
 * @brief An implementation fitted for the game of Dijkstra's algorithm to find the shortest path between a source and a target.
 * @param g The graph representing the current game map
 * @param source The begin node to search from
 * @param target The end node, stops the algorithm when it is reached
 * @return The node to go on the next move that shall lead to the shortest path, along with the weighted distance of the path.
 */
path_result shortest_path(const graph g, vec2 source, vec2 target);

/**
 * @brief A convenience function to delete the graph when it is no longer needed.
 * @param g The graph to dispose of
 */
void dispose_graph(graph g);

/**
 * @brief The weight comparator used by the priority queue in Dijkstra's algorithm.
 * @param left A value
 * @param right A value
 * @return < 0 if left < right, 0 if left == right, > 0 if left > right
 */
int compare_weights(value left, value right);

// ***********************************************************************************
// Entity finder structures & functions
// ***********************************************************************************

/**
 * @brief Iterate over the map to find the positions of the four ghosts.
 * @param map The game map
 * @param w The map width
 * @param h The map height
 * @param ghosts_pos The positions of the ghosts on the map
 */
void find_ghosts(char** map, int w, int h, vec2* ghosts_pos);

// A simple type to represent our findings in the map.
typedef struct
{
    vec2* positions;
    int count;
} findings;

/**
 * @brief Iterate over the map to find the positions of the specified entity.
 * @param map The game map
 * @param w The map width
 * @param h The map height
 * @param entity The entity type to look for
 * @param estimated_number The estimated number of entities there might be on the map
 * @param out A findings structure passed by address to store the result of the search
 */
void find_entities(char** map, int w, int h, char entity, int estimated_number, findings* out);

/**
 * @brief Convenience function to free the allocated resources created by the findings.
 * @param f The findings to free
 */
void dispose_findings(findings f);

// ***********************************************************************************
// Strategy structures & functions declarations
// ***********************************************************************************

// A new type to handle the whole context of the AI.
typedef struct
{
    graph g;
    vec2 pacman;
    entities_weights weights;
    
    findings ghosts;
    findings energizers;
    findings virgin_paths;
    
    path_result* paths_to_ghosts;
    path_result* paths_to_energizers;
    path_result* paths_to_virgin_paths;
    
    direction decision;
} ai_engine;

// A struct to tweak how pacman behaves when searching for paths.
typedef enum
{
    DEFAULT = 0, // Be wary of ghosts and energizers
    IGNORE_GHOST = 1, // Consider ghosts not as obstacles
    IGNORE_ENERGIZER = 2 // Consider energizers not as obstacles
} search_settings;

/// High level functions for strategy making

/**
 * @brief Create the AI engine, allocating the graph and storing round info.
 * @param map The game map
 * @param w The x position of pacman
 * @param y The y position of pacman
 * @param w The map width
 * @param h The map height
 * @return A newly allocated AI engine
 */
ai_engine* ai_engine_create(char** map, int x, int y, int w, int h);

/**
 * @brief Initialise the AI engine, finding ghosts, virgin paths and energizers.
 * @param ai The engine to initialise
 */
void ai_engine_initialise(ai_engine* ai);

/**
 * @brief Search the shortest paths between Pacman and the ghosts, trying to avoid energizers.
 * @param ai The engine to perform this action on
 */
void ai_engine_search_ghosts(ai_engine* ai);

/**
 * @brief Search the shortest paths between Pacman and the energizers, trying to avoid ghosts.
 * @param ai The engine to perform this action on
 */
void ai_engine_search_energizers(ai_engine* ai);

/**
 * @brief Search the shortest paths between Pacman and the remaining Pacgums.
 * @param ai The engine to perform this action on
 * @param s A flag to tell the pathfinding algorithm to not avoid ghosts, or energizers
 */
void ai_engine_search_unexplored_paths(ai_engine* ai, search_settings s);

/**
 * @brief Set the AI engine decision to target the nearest ghost.
 * @param ai The engine to perform this action on
 */
void ai_engine_target_nearest_ghost(ai_engine* ai);

/**
 * @brief Set the AI engine decision to target the nearest energizer.
 * @param ai The engine to perform this action on
 */
void ai_engine_target_nearest_energizer(ai_engine* ai);

/**
 * @brief Set the AI engine decision to target the nearest Pacgum.
 * @param ai The engine to perform this action on
 */
void ai_engine_target_nearest_unexplored_path(ai_engine* ai);

/**
 * @brief Get the number of ghosts around Pacman.
 * @param ai The engine to perform this action on
 * @param max_distance The maximum distance to brand ghosts near at
 * @return The number of ghosts where their distance to Pacman is less than max_distance
 */
int ai_engine_get_number_ghosts_near(const ai_engine* ai, int max_distance);

/**
 * @brief Get the number of energizers that are yet to be eaten.
 * @param ai The engine to perform this action on
 * @return The number of energizers left
 */
int ai_engine_get_number_energizers_left(const ai_engine* ai);

/**
 * @brief Get the number of Pacgums that are yet to be eaten.
 * @param ai The engine to perform this action on
 * @return The number of Pacgums left
 */
int ai_engine_get_number_virgin_paths_left(const ai_engine* ai);

/**
 * @brief Compute the final decision of the AI engine.
 * @param ai The engine to perform this action on
 * @return The next move of Pacman, ready to be passed to the game engine
 */
direction ai_engine_get_next_move(const ai_engine* ai);

/**
 * @brief Clean up any resources taken by the AI engine (dynamic allocs).
 * @param ai The engine to destroy
 */
void ai_engine_destroy(ai_engine* ai);

/// Low level functions

/**
 * @brief A convenience function to get the index of the shortest path.
 * @param paths The paths to analyse
 * @param path_count The number of paths to analyse
 * @return The index of the shortest path.
 */
int get_nearest_entity_index(const path_result* paths, int path_count);

/**
 * @brief Compute the shortest path for each entity provided at the given positions.
 * The results array must be allocated and of size position_count.
 * @param g The graph to use to execute the pathfinding algorithm
 * @param pacman The x-y position of Pacman
 * @param positions The entities to be taken as targets by the pathfinding algorithm
 * @param position_count The number of entities
 * @param results The path results produced by the pathfinding algorithm
 */
void compute_shortest_paths(graph g, vec2 pacman, const vec2* positions, int position_count, path_result* results);

/**
 * @brief Calculate the direction to go from a given x-y target.
 * The target must exactly be at a distance of one from Pacman.
 * @param pacman The position of Pacman
 * @param target The position to go on the next round
 * @param w The game width, to handle map borders
 * @param h The game height, to handle map borders
 */
direction orientation(vec2 pacman, vec2 target, int w, int h);

// ***********************************************************************************
// **************************** END OF PROTOTYPES SECTION ****************************
// ***********************************************************************************

// change the pacman function below to build your own player
// your new pacman function can use as many additional functions/procedures as needed; put the code of these functions/procedures *AFTER* the pacman function
direction pacman(
		 char * * map, // the map as a dynamic array of strings, ie of arrays of chars
		 int xsize, // number of columns of the map
		 int ysize, // number of lines of the map
		 int x, // x-position of pacman in the map 
		 int y, // y-position of pacman in the map
		 direction lastdirection, // last move made by pacman (see pacman.h for the direction type; lastdirection value is -1 at the beginning of the game
		 bool energy, // is pacman in energy mode? 
		 int remainingenergymoderounds // number of remaining rounds in energy mode, if energy mode is true
		 ) {
    direction d;
    
    const int ghost_chasing_threshold = 65; // Below this threshold, Pacman shall stop chasing ghosts
    const int ghost_proximity_threshold = 1; // If there are more than this value of ghosts around Pacman, it shall seek an energizer, if any
    
    // Create and initialise the AI engine from the game map
    ai_engine* ai = ai_engine_create(map, x, y, xsize, ysize);
    ai_engine_initialise(ai);
    
    // Search for ghosts before making any kind of decision
    ai_engine_search_ghosts(ai);
    
    if (energy && remainingenergymoderounds > ghost_chasing_threshold) // If we have enough time in powered-up mode...
    {
        // Pursue those evil ghosts.
        ai_engine_target_nearest_ghost(ai);
    }
    // If we are pursued by at least one ghost, and there are energizers out there, or there are only energizers left...
    else if ((!energy && ai_engine_get_number_ghosts_near(ai, 5) >= ghost_proximity_threshold && ai_engine_get_number_energizers_left(ai) > 0)
        || (ai_engine_get_number_energizers_left(ai) > 0 && ai_engine_get_number_virgin_paths_left(ai) == 0))
    {
        // Get them energizers.
        ai_engine_search_energizers(ai);
        ai_engine_target_nearest_energizer(ai);
    }
    else
    {
        // By default, Pacman shall avoid eating energizers needlessly, and will avoid ghosts
        search_settings s = DEFAULT;
        
        if (energy) // If Pacman is powered up, then treat ghosts as simple snacks
            s |= IGNORE_GHOST;
        
        // Go get all those Pacgums
        ai_engine_search_unexplored_paths(ai, s);
        ai_engine_target_nearest_unexplored_path(ai);
    }
    
    // Ask the game engine for the next move
    d = ai_engine_get_next_move(ai);
    
    // Cleanup the AI engine, we are not allowed to keep any kind of state across calls of the pacman function
    ai_engine_destroy(ai);
    
    // Anwser the game engine
    return d;
}


// the code of your additional functions/procedures must be put below

// **********************************************************************************
// Linked list functions implementation
// **********************************************************************************

list* list_new()
{
    list* l = malloc(sizeof(list));
    l->first = NULL;
    l->size = 0;
    
    return l;
}

void list_delete(list* l)
{
    node* current = l->first;
    
    // To free a linked list properly, one must iterate
    // through all its elements and free them one by one.
    while (current)
    {
        node* next = current->next;
        free(current);
        
        current = next;
    }
    
    free(l);
}

int list_size(list* l)
{
    return l->size;
}

int list_append(list* l, value v)
{
    // Appending an element is equivalent to
    // insert it to the end of the list.
    
    list_iterator it = list_iterator_begin(l);
    
    while (!list_iterator_end(it))
    {
        it = list_iterator_next(it);
    }
    
    // Here, it points to the latest element, which is NULL
    
    return list_insert(it, v);
}

int list_remove(list* l, int idx)
{
    // Don't try to remove an element out of the list.
    if (idx >= l->size)
        return 0;

    int i;
    
    node* n = l->first;
    
    // Deleting the first element needs special attention: 
    // we can't iterate until its predecessor, since it does 
    // not have any.
    if (idx == 0)
    {
        l->first = n->next;
        free(n);
    }
    else
    {
        // Iterate until the predecessor...
        for (i = 0; i < idx - 1; i++)
        {
            n = n->next;
        }
        
        // ...and swap the places before freeing the
        // element to delete.
        node* rip = n->next;
        n->next = rip->next;
        
        free(rip);
    }
    
    // Don't forget to decrease the list size.
    l->size--;
    
    return 1;
}

list_iterator list_iterator_begin(list* l)
{
    // Return an iterator object pointing to the first element
    // of the list.
    list_iterator beg = {l, &l->first};
    return beg;
}

int list_iterator_end(list_iterator it)
{
    // Signal if the iterator has reached the end of the list.
    return *it.ptr == NULL;
}

list_iterator list_iterator_next(list_iterator it)
{
    // Return a iterator pointing to the element following
    // the one pointed by this iterator.
    list_iterator next = {it.l, &(*it.ptr)->next};
    return next;
}

int list_insert(list_iterator it, value v)
{
    // Insert a list node before the node pointed by
    // the iterator.
    
    node* n = malloc(sizeof(node));
    if (!n) // Node allocation failure, exit early
        return 0;
    
    n->v = v; // Fill the actual value
    
    // Swap the places with current node.
    n->next = *it.ptr;
    *it.ptr = n;
    
    // Don't forget to increase the list size.
    it.l->size++;
    
    return 1;
}

value list_iterator_get(list_iterator it)
{
    // Dereference the iterator to get the underlying value.
    // One must check if the iterator points to a valid node
    // before calling this function.
    return (*it.ptr)->v;
}

// **********************************************************************************
// Priority queue functions implementation
// **********************************************************************************

priority_queue* priority_queue_new(int(*cmp)(value, value))
{
    // Build a priority queue on top of a linked list.
    priority_queue* q = malloc(sizeof(priority_queue));
    q->l = list_new();
    
    // The order of elements will be determined by the comparator.
    q->cmp_func = cmp;
    
    return q;
}

void priority_queue_delete(priority_queue* q)
{
    // To properly delete the priority queue, we must
    // delete the underlying list too.
    list_delete(q->l);
    
    free(q);
}

int priority_queue_size(priority_queue* q)
{
    // The priority queue size is the underlying list size.
    return list_size(q->l);
}

void priority_queue_push(priority_queue* q, value v)
{
    // Iterate through the list until we find the right place
    // to insert the element. This shall be determined by the
    // comparator function saved by the priority queue during
    // its creation.
    list_iterator it = list_iterator_begin(q->l);
    
    // While we are not at the end of the list, and the elements
    // in the priority queue have lower priority than us
    // (lowest priority means first), go to the next node.
    while (!list_iterator_end(it) 
        && q->cmp_func(list_iterator_get(it), v) < 0)
    {
        it = list_iterator_next(it);
    }
    
    // We have arrived at the right place, insert the new element
    // here. The priority queue remains sorted.
    list_insert(it, v);
}

int priority_queue_pop(priority_queue* q)
{
    // Remove the first element in the priority queue, i.e. the one
    // with the lowest priority. If none, just silently fail.
    if (priority_queue_size(q) == 0)
        return 0;
    
    return list_remove(q->l, 0);
}

int priority_queue_top(priority_queue* q, value* v)
{
    // Pass the first element in the priority queue, i.e. the one
    // with the lowest priority by address to the caller.
    //If none, signal that we could not extract any element.
    
    if (priority_queue_size(q) == 0)
        return 0;
    
    // Get the first element.
    *v = list_iterator_get(list_iterator_begin(q->l));
    
    return 1; // A valid value has been put in *v
}

// **********************************************************************************
// Pathfinding functions implementation
// **********************************************************************************

void graph_set_weight(graph g, int idx, direction dir, unsigned char weight)
{
    // The int type in C language is at least 4 bytes long on most machines, 
    // meaning we could interpret an int as a contiguous array of at least 4 bytes.
    
    // The weight value is coded as a char (1 byte long), so we can store 4 weight values
    // in an int. As we have 4 directions to handle in our game, this is perfect.
    // One might wonder about endianness when treating ints that way, but since we only
    // manipulate bytes individually, this is needless worry.
    
    // g.ptr[idx] = |    0xff    |    0xff    |    0xff    |    0xff    |
    //              | WEST weight|SOUTH weight| EAST weight|NORTH weight|
    
    g.ptr[idx] &= ~(0xff << (dir * 8)); // Clean up the old value...
    g.ptr[idx] |= (weight << (dir * 8)); // ...before putting the new one.
}

unsigned char graph_get_weight(const graph g, int idx, direction dir)
{
    // In the same way, we access weights for a specific direction by bitshifting.
    return (g.ptr[idx] >> (dir * 8)) & 0xff;
}

vec2 create_vec2(int x, int y)
{
    // This function places x in the x field of vec2, and y in its y field
    // (order sensitive). This is useful as the version of C used does not support
    // brace initialisation directly passed as a parameter to a function.
    vec2 p = {x, y};
    return p;
}

vec2 wrap_coordinates(int w, int h, vec2 p)
{
    // Be cautious we never go out of the map, and mirror the
    // coordinate that overflows accordingly.
    vec2 n = p;
    
    if (n.y < 0)
        n.y += h;
    if (n.y >= h)
        n.y -= h;
    if (n.x < 0)
        n.x += w;
    if (n.x >= w)
        n.x -= w;
    
    return n;
}

vec2 graph_index_to_coords(int idx, int w)
{
    // This function converts the indexing system used by the graph
    // to the 2-dimensional system used by the game.
    vec2 r = {idx % w, idx / w};
    return r;
}

unsigned int coords_to_graph_index(vec2 pos, int w)
{
    // This function coverts the 2-dimensional coordinate system used
    // by the game to the indexing system used by the graph.
    return pos.y * w + pos.x;
}

unsigned int graph_get_neighbor_index(int w, int h, int src, direction dir)
{
    // Get the graph index of the neighbor of src in the given direction.
    // This is easily done by converting the graph index to the game coordinates...
    vec2 p = graph_index_to_coords(src, w);
    
    // We change the coordinates accordingly.
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
    
    // ...and then reverting back to the graph indexing system, without forgetting
    // to mirror the coordinates if they ever overflowed.
    return coords_to_graph_index(wrap_coordinates(w, h, p), w);
}

unsigned char get_weight_for_entity(char c, entities_weights config)
{
    // This function essentially associate a game entity with its weight,
    // following what we wanted to set for each entity.
    unsigned char w;
    
    if (c == VIRGIN_PATH)
    {
        w = config.unexplored;
    }
    else if (c == PATH)
    {
        w = config.explored;
    }
    else if (c == ENERGY)
    {
        w = config.energizer;
    }
    else if (c == GHOST1
        || c == GHOST2
        || c == GHOST3
        || c == GHOST4)
    {
        // The ghosts have all the same weight.
        w = config.ghost;
    }
    else
    {
        // Walls and the Door are impossible to pass through.
        w = 255;
    }
    
    return w;
}

void compute_weights(char** map, int w, int h, int x, int y, char* weights, entities_weights config)
{
    // Compute the weight to go between the x-y position provided and all its neighbors.
    // A neighbor to a extreme position is the mirror in the right direction.
    
    vec2 neighbors[] = {
        wrap_coordinates(w, h, create_vec2(x, y - 1)),
        wrap_coordinates(w, h, create_vec2(x + 1, y)),
        wrap_coordinates(w, h, create_vec2(x, y + 1)),
        wrap_coordinates(w, h, create_vec2(x - 1, y))
    };

    int dir;
    
    // For each direction, get the weight to go to the neighbor in that direction.
    for (dir = 0; dir < 4; dir++)
    {
        weights[dir] = get_weight_for_entity(map[neighbors[dir].y][neighbors[dir].x], config);
    }
}

graph create_graph(char** map, int width, int height)
{
    // Create a reusable graph structure, much more easily modifiable 
    // than the map itself.
    // Each element in the graph will match a unique position on the map.
    // This is assured by the existence and unicity of the Euclidean division:
    //      - graph_idx = y * width + x
    //      - {x = graph_idx % width, y = graph_idx / width}
    graph g;
    
    g.ptr = malloc(width * height * sizeof(unsigned int));
    g.map = map;
    g.w = width;
    g.h = height;
    
    memset(g.ptr, 0, width * height * sizeof(unsigned int));
        
    return g;
}

void update_graph(graph g, entities_weights config)
{
    // Populate the provided graph, using the procedures defined
    // above.
    
    int i, j, dir;

    for (i = 0; i < g.w; i++)
    {
        for (j = 0; j < g.h; j++)
        {
            int idx = g.w * j + i; // This is the math explained above.
            char weights[4];
            
            compute_weights(g.map, g.w, g.h, i, j, weights, config);
            
            for (dir = 0; dir < 4; dir++)
                graph_set_weight(g, idx, dir, weights[dir]);
        }
    }
}

int compare_weights(value left, value right)
{
    // Compare the weight values to sort nodes in the priority queue.
    return left.weight - right.weight;
}

path_result shortest_path(const graph g, vec2 source, vec2 target)
{
    // An adapted implementation of the Dijkstra's algorithm.
    
    // Some aliases for less typing
    int w = g.w;
    int h = g.h;
    
    int i, dir; // Define some iterators
    
    int current; // The current graph node being analysed
    
    unsigned char* distances; // distance[k] = distance from source to k
    bool* visited; // visited[k] is true if the algorithm already analysed it
    unsigned int* predecessors; // predecessors[k] is the node explored before the current one
    priority_queue* q; // The priority queue to extract the nodes to analyse from
    
    bool finished = false; // A flag signalling we should stop the Dijkstra's algorithm
    bool found = false; // A flag signalling we found the target.
    
    unsigned int src; // The graph index of the source
    unsigned int dest; // The graph index of the target
    unsigned int pred; // The current predecessor, used when rebuilding the path from target to source
    
    int shortest_distance = -1; // The weighted distance between the source and the target (-1 means no path)
    int real_path_size = -1; // The raw distance between the source and the target (-1 means no path)
    vec2 next_target; // The position Pacman should go to for the next round
    
    // Those arrays are allocated in the same fashion as the graph.
    distances = malloc(w * h);
    visited = malloc(w * h);
    predecessors = malloc(w * h * sizeof(unsigned int));
    
    // Fill those arrays with default values.
    memset(distances, 0xff, w * h);
    memset(predecessors, 0xff, w * h * sizeof(unsigned int));
    memset(visited, 0, w * h);
    
    src = coords_to_graph_index(source, w);
    dest = coords_to_graph_index(target, w);
    
    // The distance from the source to the source is 0.
    distances[src] = 0;
    
    current = src;
    
    // Create the priority queue.
    q = priority_queue_new(compare_weights);
    
    // Add the source element with a zero weight to the priority queue.
    // This shall be our starting point.
    value orig = {src, 0};
    priority_queue_push(q, orig);
    
    while (!finished)
    {
        // Grab the topmost element in the priority queue, we will 
        value c;
        priority_queue_top(q, &c);
        priority_queue_pop(q);
        current = c.index;
        
        if (current == dest) // If we reached the destination...
        {
            // Signal it so we exit the loop.
            finished = true;
            found = true;
        }
        else
        {
            // Otherwise, let us visit every neighbor of this position.
            for (dir = 0; dir < 4; dir++)
            {
                int neighbor = graph_get_neighbor_index(w, h, current, dir);
                
                if (!visited[neighbor]) // If we have not visited this neighbor yet...
                {
                    visited[neighbor] = true; // Mark it as visited
                    
                    unsigned char weight = graph_get_weight(g, current, dir);
                    int cost = distances[current] + weight;
                    
                    // See if going to this neighbor is cheaper than before...
                    if (cost < distances[neighbor])
                    {
                        distances[neighbor] = cost; // Set the new cost to go to this neighbor.
                        predecessors[neighbor] = current; // Set the predecessor to build the path afterwards.
                        
                        // Add this neighbor to the queue to visit it later, and possibly build the shortest
                        // path from it.
                        value n = {neighbor, cost};
                        priority_queue_push(q, n);
                    }
                }
            }

            if (priority_queue_size(q) == 0) // If we emptied the priority queue without finishing...
            {
                // We could not find a path between the source and the target.
                finished = true;
                found = false;
            }
        }
    }

    if (found) // If a path was found...
    {
        shortest_distance = distances[dest]; // The weighted distance is stored here.
        
        pred = dest; // The starting point is the destination...
        
        i = 0; // This will give the raw size of the path.
        while (pred != src) // ...and we iterate backwards until we meet the source.
        {
            // Update the target each loop iteration, so in the end we keep the first
            // item of the whole path, that is the position Pacman should head to.
            next_target = graph_index_to_coords(pred, w);
            pred = predecessors[pred];
            
            i++;
        }
        
        real_path_size = i; // We store the raw size here to construct a path result.
    }
    
    // Those were allocated by dynamic allocation, don't forget to free them.
    free(visited);
    free(distances);
    free(predecessors);
    
    // Release the resources held by the priority queue.
    priority_queue_delete(q);
    
    // Build the path result...
    path_result res = {next_target, shortest_distance, real_path_size};
    
    // ...then give our final answer.
    return res;
}

void dispose_graph(graph g)
{
    // Release the resources held by the graph.
    free(g.ptr);
}

void find_ghosts(char** map, int w, int h, vec2* ghosts_pos)
{
    int i, j; // Define some iterators.
    
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
            // Look for the ghosts on the map and report their
            // position to the caller.
            if (map[j][i] == GHOST1)
            {
                ghosts_pos[0] = create_vec2(i, j);
            }
            else if (map[j][i] == GHOST2)
            {
                ghosts_pos[1] = create_vec2(i, j);
            }
            else if (map[j][i] == GHOST3)
            {
                ghosts_pos[2] = create_vec2(i, j);
            }
            else if (map[j][i] == GHOST4)
            {
                ghosts_pos[3] = create_vec2(i, j);
            }
        }
    }
}

void find_entities(char** map, int w, int h, char entity, int estimated_number, findings* out)
{
    int i, j; // Define some iterators.
    int nb_entities, entities_old_count; // We need to keep hold of how much entities there is.
    
    nb_entities = 0;
    // We begin by an estimated number of entities, provided by the caller, as a starting point.
    entities_old_count = estimated_number;
    out->positions = malloc(entities_old_count * sizeof(vec2));
    
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
            if (map[j][i] == entity)
            {
                // As we find entities, we need to increase the array size if the total number of
                // said entity exceeds the old array size.
                if (nb_entities == entities_old_count)
                {
                    // We double the array size each time we hit the limit, so we don't hit it
                    // too often.
                    entities_old_count *= 2;
                    out->positions = realloc(out->positions, entities_old_count * sizeof(vec2));
                }
                
                // Add this entity to the list.
                out->positions[nb_entities] = create_vec2(i, j);
                nb_entities++;
            }
        }
    }
    
    // A final realloc here to adjust the array size to the actual number of entities.
    out->positions = realloc(out->positions, nb_entities * sizeof(vec2));
    out->count = nb_entities;
}

void dispose_findings(findings f)
{
    // Release the resources held by findings.
    if (f.count > 0)
        free(f.positions);
}

// ***********************************************************************************
// Strategy functions implementations
// ***********************************************************************************

ai_engine* ai_engine_create(char** map, int x, int y, int w, int h)
{
    // Create the base structure to be used in the main pacman function.
    // We make sure all its fields have correct default values, and we
    // give it all the information it needs to guess a good direction for
    // Pacman to go.
    // The engine is not ready after its creation; it needs to be properly
    // initialised by ai_engine_initialise().
    
    ai_engine* ctx = malloc(sizeof(ai_engine));
    
    ctx->g = create_graph(map, w, h);
    ctx->pacman = create_vec2(x, y);
    
    ctx->weights.explored = 1;
    ctx->weights.unexplored = 1;
    
    ctx->ghosts.positions = NULL;
    ctx->ghosts.count = 0;
    ctx->energizers.positions = NULL;
    ctx->energizers.count = 0;
    ctx->virgin_paths.positions = NULL;
    ctx->virgin_paths.count = 0;
    
    ctx->paths_to_ghosts = NULL;
    ctx->paths_to_energizers = NULL;
    ctx->paths_to_virgin_paths = NULL;
    
    ctx->decision = -1;
    
    return ctx;
}

void ai_engine_initialise(ai_engine* ctx)
{
    // The AI engine initialisation finds all needed entities and
    // allocate the proper number of path results.
    // Basically, the AI engine is ready after initialisation.
    
    vec2 pos_ghosts[4];
    find_ghosts(ctx->g.map, ctx->g.w, ctx->g.h, pos_ghosts);
    ctx->ghosts.positions = malloc(4 * sizeof(vec2));
    ctx->ghosts.count = 4;
    
    memcpy(ctx->ghosts.positions, pos_ghosts, 4 * sizeof(vec2));
    
    find_entities(ctx->g.map, ctx->g.w, ctx->g.h, ENERGY, 4, &ctx->energizers);
    find_entities(ctx->g.map, ctx->g.w, ctx->g.h, VIRGIN_PATH, 600, &ctx->virgin_paths);
    
    ctx->paths_to_ghosts = malloc(4 * sizeof(path_result));
    
    if (ctx->energizers.count > 0)
        ctx->paths_to_energizers = malloc(ctx->energizers.count * sizeof(path_result));
    
    if (ctx->virgin_paths.count > 0)
        ctx->paths_to_virgin_paths = malloc(ctx->virgin_paths.count * sizeof(path_result));
}

void ai_engine_target_nearest_ghost(ai_engine* ctx)
{
    // Get the nearest ghost from Pacman. The paths to ghosts must have been computed beforehand.
    int i = get_nearest_entity_index(ctx->paths_to_ghosts, 4);
    
    if (i != -1) // If we found one, make our decision to target it.
        ctx->decision = orientation(ctx->pacman, ctx->paths_to_ghosts[i].next_move, ctx->g.w, ctx->g.h);
}

void ai_engine_target_nearest_energizer(ai_engine* ctx)
{
    // Get the nearest energizer from Pacman. The paths to energizers must have been computed beforehand.
    int i = get_nearest_entity_index(ctx->paths_to_energizers, ctx->energizers.count);
    
    if (i != -1) // If we found one, make our decision to target it.
        ctx->decision = orientation(ctx->pacman, ctx->paths_to_energizers[i].next_move, ctx->g.w, ctx->g.h);
}

void ai_engine_target_nearest_unexplored_path(ai_engine* ctx)
{
    // Get the nearest Pacgum from Pacman. The paths to Pacgums must have been computed beforehand.
    int i = get_nearest_entity_index(ctx->paths_to_virgin_paths, ctx->virgin_paths.count);
    
    if (i != -1) // If we found one, make our decision to target it.
        ctx->decision = orientation(ctx->pacman, ctx->paths_to_virgin_paths[i].next_move, ctx->g.w, ctx->g.h);
}

void ai_engine_search_ghosts(ai_engine* ctx)
{
    // Search the shortest paths between Pacman and every ghost while avoiding energizers.
    ctx->weights.ghost = 1;
    ctx->weights.energizer = 50;
    
    // We must update the graph, as we changed some weight values.
    update_graph(ctx->g, ctx->weights);
    
    compute_shortest_paths(ctx->g, ctx->pacman, ctx->ghosts.positions, 4, ctx->paths_to_ghosts);
}

void ai_engine_search_energizers(ai_engine* ctx)
{
    // Search the shortest paths between Pacman and every energizer while avoiding ghosts.
    ctx->weights.ghost = 50;
    ctx->weights.energizer = 1;
    
    // We must update the graph, as we changed some weight values.
    update_graph(ctx->g, ctx->weights);
    
    compute_shortest_paths(ctx->g, ctx->pacman, ctx->energizers.positions, ctx->energizers.count, ctx->paths_to_energizers);
}

void ai_engine_search_unexplored_paths(ai_engine* ctx, search_settings s)
{
    // Search the shortest paths between Pacman and every Pacgum while avoiding other entities
    // according to what the user specified as flags.
    ctx->weights.energizer = s & IGNORE_ENERGIZER ? 1 : 20;
    ctx->weights.ghost = s & IGNORE_GHOST ? 1 : 50;
    
    // We must update the graph, as we changed some weight values.
    update_graph(ctx->g, ctx->weights);
    
    compute_shortest_paths(ctx->g, ctx->pacman, ctx->virgin_paths.positions, ctx->virgin_paths.count, ctx->paths_to_virgin_paths);
}

int ai_engine_get_number_ghosts_near(const ai_engine* ctx, int max_distance)
{
    int cpt = 0; // How many ghosts are there near Pacman?
    int i;
    
    for (i = 0; i < 4; i++)
    {
        // If the path exists and its raw size is less than the distance specified as limit...
        if (ctx->paths_to_ghosts[i].size != -1 && ctx->paths_to_ghosts[i].size < max_distance)
        {
            // Count it as a ghost that is near.
            cpt++;
        }
    }
    
    return cpt;
}

int ai_engine_get_number_energizers_left(const ai_engine* ctx)
{
    // Return the number of energizers left on the map.
    // Does not support entity overlap (if a ghost is covering an energizer, 
    // it will not be seen).
    return ctx->energizers.count;
}

int ai_engine_get_number_virgin_paths_left(const ai_engine* ctx)
{
    // Return the number of Pacgums left on the map.
    // Does not support entity overlap (if a ghost is covering a Pacgum, 
    // it will not be seen).
    return ctx->virgin_paths.count;
}

direction ai_engine_get_next_move(const ai_engine* ctx)
{
    direction d = ctx->decision; // We default the decision to what has been already made.
    bool tested[4] = {false};
    bool stuck = false;
    
    while (!stuck && d == -1) // Until we get a valid direction...
    {
        // Let us check if we have not already tried every direction...
        if (tested[NORTH] && tested[SOUTH] && tested[EAST] && tested[WEST])
        {
            // If yes then we are stuck. Just commit suicide.
            stuck = true;
        }
        else
        {
            // Generate a random direction.
            int dir = rand() % 4;
            
            // Get the neighbor of Pacman in this direction.
            vec2 neighbor = graph_index_to_coords(
                graph_get_neighbor_index(
                    ctx->g.w,
                    ctx->g.h,
                    coords_to_graph_index(ctx->pacman, ctx->g.w),
                                         dir),
                                         ctx->g.w);
            
            // If it is an accessible place, go for it.
            if (ctx->g.map[neighbor.y][neighbor.x] == PATH 
                || ctx->g.map[neighbor.y][neighbor.x] == VIRGIN_PATH)
            {
                d = dir;
            }
            
            // We tried this direction.
            tested[dir] = true;
        }
    }
    
    return d; // This shall be the final answer of the AI engine.
}

void ai_engine_destroy(ai_engine* ctx)
{
    // As the AI engine dynamically allocated resources, it should
    // release them as it no longer needs them.
    // This shall be performed just after getting the final decision.
    
    free(ctx->paths_to_ghosts);
    
    if (ctx->energizers.count > 0)
        free(ctx->paths_to_energizers);
    if (ctx->virgin_paths.count > 0)
        free(ctx->paths_to_virgin_paths);
    
    dispose_findings(ctx->ghosts);
    dispose_findings(ctx->energizers);
    dispose_findings(ctx->virgin_paths);
    
    dispose_graph(ctx->g);
    
    free(ctx);
}

int get_nearest_entity_index(const path_result* paths, int path_count)
{
    // Return the index of the nearest specified entity.
    // It is a simple computation of a minimum.
    
    int i;
    
    int nearest_entity_index = -1; // Initialise as to say no entity was found
    int nearest_entity_distance = 50000; // Initialise with a huge distance
    
    for (i = 0; i < path_count; i++)
    {
        // If a shorter path has been found...
        if (paths[i].distance != -1 && paths[i].distance < nearest_entity_distance)
        {
            // Save its index.
            nearest_entity_index = i;
            nearest_entity_distance = paths[i].distance;
        }
    }
    
    return nearest_entity_index; // Return the index. One could access its actual distance later on.
}

void compute_shortest_paths(const graph g, vec2 pacman, const vec2* positions, int position_count, path_result* results)
{
    // Compute the shortest paths from pacman to the positions specified. Path results are stored in the results
    // parameter, which must be a properly allocated array of size at least position_count elements.
    
    int i;
    
    for (i = 0; i < position_count; i++)
    {
        results[i] = shortest_path(g, pacman, positions[i]);
    }
}

direction orientation(vec2 pacman, vec2 target, int w, int h) 
{
    // Gives the actual direction for Pacman to take to go to the specified target.
    // The target position must be adjacent to the position of Pacman.
    direction d = -1;

    if (pacman.x == target.x - 1 || (pacman.x == w - 1 && target.x == 0)) 
    {
        d = EAST;
    }
    else if (pacman.x == target.x + 1 || (pacman.x == 0 && target.x == w - 1)) 
    {
        d = WEST;
    }
    else if (pacman.y == target.y - 1 || (pacman.y == h - 1 && target.y == 0)) 
    {
        d = SOUTH;
    }
    else if (pacman.y == target.y + 1 || (pacman.y == 0 && target.y == h - 1)) 
    {
        d = NORTH;
    }

    return d;
}
