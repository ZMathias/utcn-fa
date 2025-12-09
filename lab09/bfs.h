// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef __BFS_H__
#define __BFS_H__

#include "Profiler.h"

constexpr bool knight_eval = false;

#define MAX_ROWS 100
#define MAX_COLS 100

typedef struct {
    int rows;
    int cols;
    int mat[MAX_ROWS][MAX_COLS];
} Grid;

typedef struct {
    int row;
    int col;
} Point;

enum {
    COLOR_WHITE = 0,
    COLOR_GRAY,
    COLOR_BLACK
};

typedef struct NodeT {
    Point position;
    int adjSize;
    struct NodeT **adj;

    int color;
    int dist;
    struct NodeT *parent;
}Node;

typedef struct{
    int nrNodes;
    Node **v;
}Graph;

int get_neighbors(const Grid *grid, Point p, Point neighbours[]);
void grid_to_graph(const Grid *grid, Graph *graph);
void free_graph(Graph *graph);
void bfs_knight(const Graph *graph);
void bfs(const Graph *graph, Node *s, Operation *op=nullptr);
void print_bfs_tree(const Graph *graph);
int shortest_path(const Graph *graph, Node *start, Node *end, Node *path[]);
void performance();

#endif