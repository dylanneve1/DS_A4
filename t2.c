#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "t2.h"

Graph* create_graph(int num_nodes) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    g->num_nodes = num_nodes;

    for (int i = 0; i < MAX_VERTICES; i++) {
        for (int j = 0; j < MAX_VERTICES; j++) {
            g->adj_matrix[i][j] = (i < num_nodes && j < num_nodes) ? 0 : INT_MAX;
        }
    }
    return g;
}

void add_edge(Graph* g, int from, int to, int weight) {
    if (from < 0 || from >= g->num_nodes || to < 0 || to >= g->num_nodes) {
        fprintf(stderr, "Invalid edge endpoints\n");
        return;
    }
    g->adj_matrix[from][to] = weight;
    g->adj_matrix[to][from] = weight; // Undirected graph
}

void print_solution(int dist[], int perm_order[], int num_nodes) {
    printf("Order of nodes becoming permanent:\n");
    for (int i = 0; i < num_nodes; i++) {
        printf("%c ", 'A' + perm_order[i]);
    }
    printf("\n");
    printf("Shortest paths from the source:\n");
    for (int i = 0; i < num_nodes; i++) {
        printf("%c: %d\n", 'A' + i, dist[i]);
    }
}

void dijkstra(Graph* g, int origin) {
    int num_nodes = g->num_nodes;
    int dist[MAX_VERTICES];
    bool sptSet[MAX_VERTICES];
    int perm_order[MAX_VERTICES];
    int perm_count = 0;

    for (int i = 0; i < num_nodes; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }
    dist[origin] = 0;

    for (int count = 0; count < num_nodes; count++) {
        int min_dist = INT_MAX, u = -1;
        for (int v = 0; v < num_nodes; v++) {
            if (!sptSet[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }

        if (u == -1) break;
        sptSet[u] = true;
        perm_order[perm_count++] = u;

        for (int v = 0; v < num_nodes; v++) {
            if (!sptSet[v] && g->adj_matrix[u][v] != 0 && g->adj_matrix[u][v] != INT_MAX &&
                dist[u] + g->adj_matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + g->adj_matrix[u][v];
            }
        }
    }
    print_solution(dist, perm_order, num_nodes);
}

void delete_graph(Graph* g) {
    free(g);
}
