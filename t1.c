#include <stdio.h>
#include <stdlib.h>
#include "t1.h"

Node* create_node(int dest) {
    // Allocate memory for new node
    Node* new_node = (Node*)malloc(sizeof(Node));
    // Check for failure
    if (!new_node) {
        fprintf(stderr, "Error: Memory allocation failed for new node.\n");
        exit(EXIT_FAILURE);
    }
    // Set dest to arg and next to NULL
    new_node->dest = dest;
    new_node->next = NULL;
    // Return created node
    return new_node;
}

Graph* create_graph(int num_nodes) {
    // Allocate memory for the Graph structure
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    // Check for failure
    if (!graph) {
        fprintf(stderr, "Error: Memory allocation failed for graph.\n");
        exit(EXIT_FAILURE);
    }

    // Set number of nodes based on argument
    graph->num_nodes = num_nodes;

    // Memory for the adjacency list 
    graph->adjacency_list = (Node**)malloc(num_nodes * sizeof(Node*));
    // Check for failure
    if (!graph->adjacency_list) {
        fprintf(stderr, "Error: Memory allocation failed for adjacency list.\n");
        free(graph); // Free previously allocated memory before exiting
        exit(EXIT_FAILURE);
    }

    // Initialize each adjacency list to NULL (no edges yet)
    for (int i = 0; i < num_nodes; i++) {
        graph->adjacency_list[i] = NULL;
    }

    return graph;
}

// Function to add a directed edge from 'from' to 'to'
void add_edge(Graph* g, int from, int to) {
    if (from < 0 || from >= g->num_nodes || to < 0 || to >= g->num_nodes) {
        fprintf(stderr, "Error: Invalid node index.\n");
        return;
    }

    // Create a new node for the destination
    Node* new_node = create_node(to);

    // Insert the new node at the beginning of the adjacency list
    new_node->next = g->adjacency_list[from];
    g->adjacency_list[from] = new_node;
}

void bfs(Graph* g, int origin) {
    // Check graph is valid
    if (!g) {
        return;
    }
    // Check origin is valud
    if (origin < 0 || origin >= g->num_nodes) {
        printf("Error: Invalid origin node for BFS.\n");
        return;
    }

    // Array to keep track of visited nodes
    int visited[g->num_nodes];
    for (int i = 0; i < g->num_nodes; i++) {
        visited[i] = 0;
    }

    // Simple queue implementation using a fixed-size array
    int queue[g->num_nodes];
    int front = 0;
    int rear = 0;

    // Enqueue the origin node and mark it as visited
    queue[rear++] = origin;
    visited[origin] = 1;

    printf("BFS Traversal: ");

    while (front < rear) {
        // Dequeue the front node
        int current = queue[front++];

        // Print the current node
        printf("%c ", 'A' + current);

        // Traverse all adjacent nodes
        Node* temp = g->adjacency_list[current];
        while (temp) {
            int adj = temp->dest;
            if (!visited[adj]) {
                queue[rear++] = adj;
                visited[adj] = 1;
            }
            temp = temp->next;
        }
    }

    printf("\n");
}

void dfs_util(Graph* g, int vertex, int* visited) {
    // Mark the current node as visited and print it
    visited[vertex] = 1;
    printf("%c ", 'A' + vertex); // Convert index to character

    for (Node* temp = g->adjacency_list[vertex]; temp; temp = temp->next) {
        if (!visited[temp->dest]) {
            dfs_util(g, temp->dest, visited);
        }
    }
}

void dfs(Graph* g, int origin) {
    if (!g || origin < 0 || origin >= g->num_nodes) {
        printf("Error: Invalid origin node or graph for DFS.\n");
        return;
    }

    int* visited = (int*)calloc(g->num_nodes, sizeof(int));
    if (!visited) {
        printf("Error: Memory allocation failed for visited array.\n");
        exit(EXIT_FAILURE);
    }

    printf("DFS Traversal: ");
    dfs_util(g, origin, visited);
    printf("\n");

    free(visited);
}

void delete_graph(Graph* g) {
    // Check graph exists
    if (!g) {
        return;
    }

    // Free each node and free adjacency lists
    for (int i = 0; i < g->num_nodes; i++) {
        Node* current = g->adjacency_list[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(g->adjacency_list);
    free(g);
}
