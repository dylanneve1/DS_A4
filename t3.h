#ifndef T3_H_
#define T3_H_

#define MAX_GRAPH 10000
#define MAX_STRING_SIZE 100

typedef struct Graph {
    int graph_ar[MAX_GRAPH][MAX_GRAPH];
} Graph;

typedef struct Stops {
    int stop_no;
    char Name[MAX_STRING_SIZE];
    float Latitude;
    float Longitude;
} Stops;

typedef struct Edge {
    int from;
    int to;
    int weight;
} Edge;

int load_edges ( char *fname ); //loads the edges from the CSV file of name fname
int load_vertices ( char *fname );  //loads the vertices from the CSV file of name fname
void shortest_path(int startNode, int endNode); // prints the shortest path between startNode and endNode, if there is any
void free_memory ( void ) ; // frees any memory that was used

#endif
