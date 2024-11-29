#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include "t3.h"

Stops *Vertexs[MAX_GRAPH];
Graph *g;

int next_field(FILE *f, char *buf, int max) {
    int i = 0, quoted = 0;
    int c;

    while (1) {
        c = fgetc(f);

        // Check if char is EOF
        // If this is first entry in buffer then
        // Return fail, otherwise break to return
        if (c == EOF) {
            if (i == 0) {
                return NEXT_FIELD_FAIL;
            }
            // i > 0, EOF reached
            break;
        }

        // Enter or exit quotes
        if (c == '"') {
            quoted = !quoted;
            // We don't actually add quotes to buffer
            continue;
        }


        // We have reached seperator and not in quotes!
        if (c == ',' && !quoted) {
            // Hmmm first character was a seperator
            // This should not happen, return fail
            if (i == 0) {
                return NEXT_FIELD_FAIL;
            }
            // Otherwize, we have just reached EOF
            break;
        }
        // Same as seperator, '\n' indicates EOF
        if (c == '\n') {
            break;
        }
        if (i < max - 1) {
            buf[i++] = c;
        }
    }

    buf[i] = '\0';
    return 0;
}

// Function to parse a stop from the CSV file
Stops *createStop(FILE *f) {
    char buf[MAX_STRING_SIZE];
    Stops *temp_stop = malloc(sizeof(Stops));
    if (!temp_stop) {
        printf("Memory allocation failed for Stops\n");
        return NULL;
    }

    // Read stop_no
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
        free(temp_stop);
        return NULL;
    }
    temp_stop->stop_no = atoi(buf);

    // Read Name
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
        free(temp_stop);
        return NULL;
    }
    strncpy(temp_stop->Name, buf, MAX_STRING_SIZE);
    temp_stop->Name[MAX_STRING_SIZE - 1] = '\0'; // Ensure null-termination

    // Read Latitude
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
        free(temp_stop);
        return NULL;
    }
    temp_stop->Latitude = atof(buf);

    // Read Longitude
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
        free(temp_stop);
        return NULL;
    }
    temp_stop->Longitude = atof(buf);

    return temp_stop;
}

// Function to parse an edge from the CSV file
Edge *createEdge(FILE *f) {
    char buf[MAX_STRING_SIZE];
    Edge *temp_edge = malloc(sizeof(Edge));
    if (!temp_edge) {
        printf("Memory allocation failed for Edge\n");
        return NULL;
    }

    // Read from
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
        free(temp_edge);
        return NULL;
    }
    temp_edge->from = atoi(buf);

    // Read to
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
        free(temp_edge);
        return NULL;
    }
    temp_edge->to = atoi(buf);

    // Read weight
    if (next_field(f, buf, MAX_STRING_SIZE) == NEXT_FIELD_FAIL) {
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
        printf("Memory allocation failed for Graph\n");
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
        printf("Edge nodes %d-%d out of bounds\n", from, to);
        return;
    }
    // Esentially links two stops, provides path between them
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
        Edge *temp = createEdge(f);
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
        Stops *temp = createStop(f);
        if (!temp) {
            break; // End of file or error
        }
        if (temp->stop_no >= MAX_GRAPH || temp->stop_no < 0) {
            printf("Stop number %d out of bounds\n", temp->stop_no);
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
        // Skip finallized nodes and make sure node is reachable
        if (!shortestpath[i] && distance[i] < min) {
            min = distance[i];
            min_index = i;
        }
    }
    return min_index;
}

// Implement Dijkstra's algorithm
void dijkstra(int start, int end) {
    // Array to keep track of distance
    int distance[MAX_GRAPH];
    // Array to keep track of shortest path
    bool shortestpath[MAX_GRAPH];
    // Array to keep track of nodes traversed
    int prev[MAX_GRAPH];

    // Initialize distances and shortestpath set
    for (int i = 0; i < MAX_GRAPH; i++) {
        // Set all disatances to INT_MAX aka unknown
        distance[i] = INT_MAX;
        // Shortest path is not found for app
        shortestpath[i] = false;
        // -1 is essentially the origin, when reconstructing
        // The path we can use it to signify the end of path
        prev[i] = -1;
    }

    // Distance to origin from origin is zero
    distance[start] = 0;

    for (int count = 0; count < MAX_GRAPH; count++) {
        int u = min_distance(distance, shortestpath);
        if (u == -1) {
            break; // No more reachable vertices
        }

        shortestpath[u] = true;

        // Update distance value of adjacent vertices
        for (int v = 0; v < MAX_GRAPH; v++) {
            // Skip finallized nodes and make sure edge exists
            if (g->graph_ar[u][v] && !shortestpath[v] &&
                // Distance must be less than INT_MAX (unreachable)
                distance[u] != INT_MAX &&
                // If the distance we can travel is less, better path is found
                distance[u] + g->graph_ar[u][v] < distance[v]) {
                distance[v] = distance[u] + g->graph_ar[u][v];
                prev[v] = u;
            }
        }

        // Early exit if we reached the destination node
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
    // Start at the end
    int crawl = end;
    // Add end node
    path[path_length++] = crawl;
    // Keep adding prev nodes until we get back to irigin
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