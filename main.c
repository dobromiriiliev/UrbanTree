#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <geos_c.h>
#include <string.h>

#define MAX_NODES 10

typedef struct {
    int id;
    double lat, lon;
} Node;

typedef struct {
    int from;
    int to;
    double cost;
} Edge;

Node nodes[MAX_NODES];
int node_count = 0;

Edge edges[100];
int edge_count = 0;

// Initialize GEOS
void init_geos() {
    initGEOS(NULL, NULL);
}

// Function to calculate the Euclidean distance between two nodes
double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    GEOSCoordSequence* seq1 = GEOSCoordSeq_create(1, 2);
    GEOSCoordSeq_setX(seq1, 0, lon1);
    GEOSCoordSeq_setY(seq1, 0, lat1);

    GEOSCoordSequence* seq2 = GEOSCoordSeq_create(1, 2);
    GEOSCoordSeq_setX(seq2, 0, lon2);
    GEOSCoordSeq_setY(seq2, 0, lat2);

    GEOSGeometry* pt1 = GEOSGeom_createPoint(seq1);
    GEOSGeometry* pt2 = GEOSGeom_createPoint(seq2);

    double distance;
    GEOSDistance(pt1, pt2, &distance);

    GEOSGeom_destroy(pt1);
    GEOSGeom_destroy(pt2);
    GEOSCoordSeq_destroy(seq1);
    GEOSCoordSeq_destroy(seq2);

    return distance;
}

// Heuristic: straight-line distance from a to b
double heuristic(int a, int b) {
    return calculate_distance(nodes[a].lat, nodes[a].lon, nodes[b].lat, nodes[b].lon);
}

// A* algorithm to find the shortest path
void a_star(int start, int goal) {
    double g_score[MAX_NODES]; // Cost from start to node
    double f_score[MAX_NODES]; // Estimated cost from start to goal through node
    int from_node[MAX_NODES];  // Tracks path
    int open_set[MAX_NODES];   // Nodes to be evaluated
    int open_set_count = 0;

    for (int i = 0; i < MAX_NODES; i++) {
        g_score[i] = INFINITY;
        f_score[i] = INFINITY;
        from_node[i] = -1;
    }

    g_score[start] = 0;
    f_score[start] = heuristic(start, goal);

    open_set[open_set_count++] = start;

    while (open_set_count > 0) {
        int current = open_set[0]; // The node in openSet having the lowest f_score[] value
        if (current == goal) {
            // Reconstruct path
            printf("Path: ");
            while (current != start) {
                printf("%d <- ", current);
                current = from_node[current];
            }
            printf("%d\n", start);
            return;
        }

        // Remove current from open set
        memmove(&open_set[0], &open_set[1], sizeof(int) * (--open_set_count));

        for (int i = 0; i < edge_count; i++) {
            if (edges[i].from == current) {
                int neighbor = edges[i].to;
                double tentative_g_score = g_score[current] + edges[i].cost;
                if (tentative_g_score < g_score[neighbor]) {
                    from_node[neighbor] = current;
                    g_score[neighbor] = tentative_g_score;
                    f_score[neighbor] = g_score[neighbor] + heuristic(neighbor, goal);
                    if (open_set_count == 0 || neighbor != open_set[open_set_count-1]) {
                        open_set[open_set_count++] = neighbor;
                    }
                }
            }
        }
    }
}

int main() {
    init_geos();
    // Example nodes (Add real coordinates)
    nodes[node_count++] = (Node){0, 37.7749, -122.4194}; // Node 0: San Francisco
    nodes[node_count++] = (Node){1, 34.0522, -118.2437}; // Node 1: Los Angeles
    nodes[node_count++] = (Node){2, 36.1699, -115.1398}; // Node 2: Las Vegas

    // Example edges (Add real costs based on distance)
    edges[edge_count++] = (Edge){0, 1, calculate_distance(nodes[0].lat, nodes[0].lon, nodes[1].lat, nodes[1].lon)};
    edges[edge_count++] = (Edge){0, 2, calculate_distance(nodes[0].lat, nodes[0].lon, nodes[2].lat, nodes[2].lon)};
    edges[edge_count++] = (Edge){1, 2, calculate_distance(nodes[1].lat, nodes[1].lon, nodes[2].lat, nodes[2].lon)};

    // Perform A* search from San Francisco to Las Vegas
    a_star(0, 2);

    return 0;
}
