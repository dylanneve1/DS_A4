#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include "t3.h"

Stops *Vertexs[MAX_GRAPH];
Graph *g;

// Function to read the next field in a CSV, handling quoted fields
int next_field(FILE *f, char *buf, int max) {
    int i = 0, end = 0, quoted = 0;
    int c;

    while ((c = fgetc(f)) != EOF && !end) {
        if (c == '"') {
            quoted = !quoted;
            continue; // Skip the quote character
        }
        if (c == ',' && !quoted) {
            break;
        }
        if (c == '\n') {
            end = 1;
            break;
        }
        if (i < max - 1) {
            buf[i++] = c;
        }
    }

    buf[i] = '\0';
    return end;
}

// Function to parse a stop from the CSV file
Stops *fetch_stop(FILE *f) {
    char buf[MAX_STRING_SIZE];
    Stops *temp_stop = malloc(sizeof(Stops));
    if (!temp_stop) {
        fprintf(stderr, "Memory allocation failed for Stops\n");
        return NULL;
    }

    // Read stop_no
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_stop);
        return NULL;
    }
    temp_stop->stop_no = atoi(buf);

    // Read Name
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_stop);
        return NULL;
    }
    strncpy(temp_stop->Name, buf, MAX_STRING_SIZE);
    temp_stop->Name[MAX_STRING_SIZE - 1] = '\0'; // Ensure null-termination

    // Read Latitude
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_stop);
        return NULL;
    }
    temp_stop->Latitude = atof(buf);

    // Read Longitude
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_stop);
        return NULL;
    }
    temp_stop->Longitude = atof(buf);

    return temp_stop;
}

// Function to parse an edge from the CSV file
Edge *fetch_edge(FILE *f) {
    char buf[MAX_STRING_SIZE];
    Edge *temp_edge = malloc(sizeof(Edge));
    if (!temp_edge) {
        fprintf(stderr, "Memory allocation failed for Edge\n");
        return NULL;
    }

    // Read from
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_edge);
        return NULL;
    }
    temp_edge->from = atoi(buf);

    // Read to
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_edge);
        return NULL;
    }
    temp_edge->to = atoi(buf);

    // Read weight
    if (next_field(f, buf, MAX_STRING_SIZE) == EOF) {
        free(temp_edge);
        return NULL;
    }
    temp_edge->weight = atoi(buf);

    return temp_edge;
}

// Initialize the graph with no edges
void init_graph() {
    g = malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Memory allocation failed for Graph\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_GRAPH; i++) {
        for (int j = 0; j < MAX_GRAPH; j++) {
            g->graph_ar[i][j] = 0; // No edge initially
        }
    }
}

// Add an undirected edge to the graph
void add_edge(Graph *g, int from, int to, int weight) {
    if (from >= MAX_GRAPH || to >= MAX_GRAPH || from < 0 || to < 0) {
        fprintf(stderr, "Edge nodes %d-%d out of bounds\n", from, to);
        return;
    }
    g->graph_ar[from][to] = weight;
    g->graph_ar[to][from] = weight;
}

// Load edges from a CSV file
int load_edges(char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        printf("Unable to open %s\n", fname);
        return 0;
    }

    // Skip header
    char header[MAX_STRING_SIZE];
    fgets(header, sizeof(header), f);

    init_graph();

    int num_edges = 0;
    while (!feof(f)) {
        Edge *temp = fetch_edge(f);
        if (!temp) {
            break; // End of file or error
        }
        add_edge(g, temp->from, temp->to, temp->weight);
        num_edges++;
        free(temp);
    }

    fclose(f);
    printf("Loaded %d edges\n", num_edges);
    return 1;
}

// Load vertices from a CSV file
int load_vertices(char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        printf("Unable to open %s\n", fname);
        return 0;
    }

    // Skip header
    char header[MAX_STRING_SIZE];
    fgets(header, sizeof(header), f);

    int num_vertices = 0;
    while (!feof(f)) {
        Stops *temp = fetch_stop(f);
        if (!temp) {
            break; // End of file or error
        }
        if (temp->stop_no >= MAX_GRAPH || temp->stop_no < 0) {
            fprintf(stderr, "Stop number %d out of bounds\n", temp->stop_no);
            free(temp);
            continue;
        }
        Vertexs[temp->stop_no] = temp;
        num_vertices++;
    }

    fclose(f);
    printf("Loaded %d vertices\n", num_vertices);
    return 1;
}

// Find the vertex with the minimum distance value
int min_distance(int distance[MAX_GRAPH], bool shortestpath[MAX_GRAPH]) {
    int min = INT_MAX;
    int min_index = -1;
    for (int i = 0; i < MAX_GRAPH; i++) {
        if (!shortestpath[i] && distance[i] < min) {
            min = distance[i];
            min_index = i;
        }
    }
    return min_index;
}

// Implement Dijkstra's algorithm
void dijkstra(int start, int end) {
    int distance[MAX_GRAPH];
    bool shortestpath[MAX_GRAPH];
    int prev[MAX_GRAPH];

    // Initialize distances and shortestpath set
    for (int i = 0; i < MAX_GRAPH; i++) {
        distance[i] = INT_MAX;
        shortestpath[i] = false;
        prev[i] = -1;
    }

    distance[start] = 0;

    for (int count = 0; count < MAX_GRAPH; count++) {
        int u = min_distance(distance, shortestpath);
        if (u == -1) {
            break; // No more reachable vertices
        }

        shortestpath[u] = true;

        // Update distance value of adjacent vertices
        for (int v = 0; v < MAX_GRAPH; v++) {
            if (g->graph_ar[u][v] && !shortestpath[v] &&
                distance[u] != INT_MAX &&
                distance[u] + g->graph_ar[u][v] < distance[v]) {
                distance[v] = distance[u] + g->graph_ar[u][v];
                prev[v] = u;
            }
        }

        // Early exit if we reached the destination
        if (u == end) {
            break;
        }
    }

    // Check if there is a path
    if (distance[end] == INT_MAX) {
        printf("No path exists between %d and %d\n", start, end);
        return;
    }

    // Reconstruct the path
    int path[MAX_GRAPH];
    int path_length = 0;
    int crawl = end;
    path[path_length++] = crawl;
    while (prev[crawl] != -1) {
        crawl = prev[crawl];
        path[path_length++] = crawl;
    }

    // Print the path in reverse order
    printf("Shortest path from %d (%s) to %d (%s):\n",
           start, Vertexs[start]->Name,
           end, Vertexs[end]->Name);
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%-10d %-30s %-12.8f %-12.8f\n",
        Vertexs[path[i]]->stop_no,
        Vertexs[path[i]]->Name,
        Vertexs[path[i]]->Latitude,
        Vertexs[path[i]]->Longitude);
    }
    printf("Total distance: %d\n", distance[end]);
}

// Function to find and print the shortest path
void shortest_path(int startNode, int endNode) {
    if (!Vertexs[startNode]) {
        printf("Start node %d does not exist.\n", startNode);
        return;
    }
    if (!Vertexs[endNode]) {
        printf("End node %d does not exist.\n", endNode);
        return;
    }

    dijkstra(startNode, endNode);
}

// Free all allocated memory
void free_memory(void) {
    if (g) {
        free(g);
    }
    for (int i = 0; i < MAX_GRAPH; i++) {
        if (Vertexs[i]) {
            free(Vertexs[i]);
            Vertexs[i] = NULL;
        }
    }
}