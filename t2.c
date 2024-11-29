#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "t2.h"

Graph* create_graph(int num_nodes) {
    // Allocate memory for graph
    Graph* g = (Graph*)malloc(sizeof(Graph));
    // Check for failure
    if (!g) {
        printf("Memory allocation for graph failed\n");
        exit(EXIT_FAILURE);
    }
    // Set number of nodes
    g->num_nodes = num_nodes;

    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            g->adj_matrix[i][j] = 0;
        }
    }
    return g;
}

void add_edge(Graph* g, int from, int to, int weight) {
    if (from < 0 || from >= g->num_nodes || to < 0 || to >= g->num_nodes) {
        printf("Invalid edge endpoints\n");
        return;
    }
    // Both directions
    g->adj_matrix[from][to] = weight;
    g->adj_matrix[to][from] = weight;
}

void print_solution(int dist[], int perm_order[], int num_nodes, int origin) {
    printf("Nodes in Graph: ");
    for (int i = 0; i < num_nodes; i++) {
        printf("%c ", 'A' + perm_order[i]);
    }
    printf("\n");
    printf("Dijkstra's Algorithm Finds: \n");
    for (int i = 0; i < num_nodes; i++) {
        printf("- Shortest path %c to %c: %d\n", 'A' + origin, 'A' + i, dist[i]);
    }
}

void dijkstra(Graph* g, int origin) {
    int num_nodes = g->num_nodes;
    // Set arrays to keep track of finalization
    // And distance to node relative to given origin
    int dist[MAX_VERTICES];
    bool sptSet[MAX_VERTICES];
    // Order of finalized nodes
    int perm_order[MAX_VERTICES];
    // We have finalized none to start
    int perm_count = 0;

    for (int i = 0; i < num_nodes; i++) {
        // Infinite distance
        dist[i] = INT_MAX;
        // Not finalized
        sptSet[i] = false;
    }
    // Distance to origin is zero
    dist[origin] = 0;

    for (int count = 0; count < num_nodes; count++) {
        int min_dist = INT_MAX, u = -1;
        // We must find the closest unfinalized node
        for (int v = 0; v < num_nodes; v++) {
            if (!sptSet[v] && dist[v] < min_dist) {
                // Get distance to node
                min_dist = dist[v];
                u = v;
            }
        }

        // If NO node found that is not finalized
        // Then we should break since nothing more can be done
        if (u == -1) { 
            break;
        }
        // Set as finalized
        sptSet[u] = true;
        // Add to perm array
        perm_order[perm_count++] = u;

        for (int v = 0; v < num_nodes; v++) {
            // Check node isnt finalized and edge exists
            if (!sptSet[v] && g->adj_matrix[u][v] != 0 && 
                // Check node is actually reachable
                g->adj_matrix[u][v] != INT_MAX &&
                // If the distance through that path is smaller than our
                // Current shortest path then update it
                dist[u] + g->adj_matrix[u][v] < dist[v]) {
                // Set new distance
                dist[v] = dist[u] + g->adj_matrix[u][v];
            }
        }
    }
    // Print final solution
    print_solution(dist, perm_order, num_nodes, origin);
}

void delete_graph(Graph* g) {
    free(g);
}
