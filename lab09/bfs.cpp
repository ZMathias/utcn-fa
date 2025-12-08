// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cstdlib>
#include <string>
#include "bfs.h"

#include <cmath>
#include <queue>

bool is_inside(const Grid *grid, Point p) {
    const int lim_x = grid->cols;
    const int lim_y = grid->rows;

    return p.col >= 0 && p.col <= lim_x && p.row >= 0 && p.row <= lim_y;
}

int get_neighbors(const Grid *grid, Point p, Point neighb[]) {
    int arr_it = 0;
    for (int i = 0; i < 4; i++) {
        int posx[] = {0, 0, -1, 1};
        int posy[] = {1, -1, 0, 0};
        Point translated = p;
        translated.col += posx[i];
        translated.row += posy[i];

        if (is_inside(grid, translated)) {
            if (grid->mat[translated.row][translated.col] == 0) {
                neighb[arr_it++] = translated;
            }
        }
    }

    return arr_it;
}

void grid_to_graph(const Grid *grid, Graph *graph) {
    //we need to keep the nodes in a matrix, so we can easily refer to a position in the grid
    Node *nodes[MAX_ROWS][MAX_COLS];
    int i, j;
    Point neighb[4];

    //compute how many nodes we have and allocate each node
    graph->nrNodes = 0;
    for (i = 0; i < grid->rows; ++i) {
        for (j = 0; j < grid->cols; ++j) {
            if (grid->mat[i][j] == 0) {
                nodes[i][j] = static_cast<Node *>(malloc(sizeof(Node)));
                memset(nodes[i][j], 0, sizeof(Node)); //initialize all fields with 0/NULL //-V575
                nodes[i][j]->position.row = i;
                nodes[i][j]->position.col = j;
                ++graph->nrNodes;
            } else {
                nodes[i][j] = nullptr;
            }
        }
    }
    graph->v = static_cast<Node **>(malloc(graph->nrNodes * sizeof(Node *)));
    int k = 0;
    for (i = 0; i < grid->rows; ++i) {
        for (j = 0; j < grid->cols; ++j) {
            if (nodes[i][j] != nullptr) {
                graph->v[k++] = nodes[i][j]; //-V522
            }
        }
    }

    //compute the adjacency list for each node
    for (i = 0; i < graph->nrNodes; ++i) {
        graph->v[i]->adjSize = get_neighbors(grid, graph->v[i]->position, neighb);
        if (graph->v[i]->adjSize != 0) {
            graph->v[i]->adj = static_cast<Node **>(malloc(graph->v[i]->adjSize * sizeof(Node *)));
            k = 0;
            for (j = 0; j < graph->v[i]->adjSize; ++j) {
                if (neighb[j].row >= 0 && neighb[j].row < grid->rows &&
                    neighb[j].col >= 0 && neighb[j].col < grid->cols &&
                    grid->mat[neighb[j].row][neighb[j].col] == 0) {
                    graph->v[i]->adj[k++] = nodes[neighb[j].row][neighb[j].col]; //-V522
                }
            }
            if (k < graph->v[i]->adjSize) {
                //get_neighbors returned some invalid neighbors
                graph->v[i]->adjSize = k;
                graph->v[i]->adj = static_cast<Node **>(realloc(graph->v[i]->adj, k * sizeof(Node *))); //-V701
            }
        }
    }
}

void free_graph(Graph *graph) {
    if (graph->v != nullptr) {
        for (int i = 0; i < graph->nrNodes; ++i) {
            if (graph->v[i] != nullptr) {
                if (graph->v[i]->adj != nullptr) {
                    free(graph->v[i]->adj);
                    graph->v[i]->adj = nullptr;
                }
                graph->v[i]->adjSize = 0;
                free(graph->v[i]);
                graph->v[i] = nullptr;
            }
        }
        free(graph->v);
        graph->v = nullptr;
    }
    graph->nrNodes = 0;
}

void bfs(const Graph *graph, Node *s, Operation *op) {
    /* TODO: implement the BFS algorithm on the graph, starting from the node s
    * at the end of the algorithm, every node reachable from s should have the color BLACK
    * for all the visited nodes, the minimum distance from s (dist) and the parent in the BFS tree should be set
    * for counting the number of operations, the optional op parameter is received
    */

    for (int i = 0; i < graph->nrNodes; i++) {
        NodeT* node = graph->v[i];
        node->parent = nullptr;
        node->dist = INFINITY;
        node->color = COLOR_WHITE;
    } // initialize all nodes

    std::queue<NodeT*> q;
    q.push(s);
    s->color = COLOR_GRAY;
    s->dist = 0;

    // enqueue all neighbours of s
    while (!q.empty()) {
        NodeT* curr = q.front();
        q.pop();

        for (int i = 0; i < curr->adjSize; i++) {
            if (curr->adj[i]->color == COLOR_WHITE) {
                curr->adj[i]->color = COLOR_GRAY; // enqueue turns nodes to grey
                curr->adj[i]->parent = curr;
                curr->adj[i]->dist = curr->dist + 1;
                q.push(curr->adj[i]);
            }
        }

        // after done processing turn the node black
        curr->color = COLOR_BLACK;
    }
}

void pretty_print_r1(int *parent, Point *repr, int size, int node, int depth) {
    if (parent == nullptr)
        return;

    constexpr int spaces_per_depth = 8;

    for (int i = 0; i < size; i++) {
        if (parent[i] == node) {
            for (int x = 0; x < depth * spaces_per_depth; x++) {
                putchar(' ');
            }
            printf("(%d, %d)\n", repr[i].row, repr[i].col);
            pretty_print_r1(parent, repr, size, i, depth + 1);
        }
    }
}

void print_bfs_tree(const Graph *graph) {
    //first, we will represent the BFS tree as a parent array
    int n = 0; //the number of nodes
    int *p = nullptr; //the parent array
    Point *repr = nullptr; //the representation for each element in p

    //some of the nodes in graph->v may not have been reached by BFS
    //p and repr will contain only the reachable nodes
    int *transf = static_cast<int *>(malloc(graph->nrNodes * sizeof(int)));
    for (int i = 0; i < graph->nrNodes; ++i) {
        if (graph->v[i]->color == COLOR_BLACK) {
            transf[i] = n; //-V522
            ++n;
        } else {
            //-V522
            transf[i] = -1;
        }
    }
    if (n == 0) {
        //no BFS tree
        free(transf);
        return;
    }

    int err = 0;
    p = static_cast<int *>(malloc(n * sizeof(int)));
    repr = new Point[n];
    for (int i = 0; i < graph->nrNodes && !err; ++i) {
        if (graph->v[i]->color == COLOR_BLACK) {
            if (transf[i] < 0 || transf[i] >= n) {
                err = 1;
            } else {
                repr[transf[i]] = graph->v[i]->position;
                if (graph->v[i]->parent == nullptr) {
                    p[transf[i]] = -1; //-V522
                } else {
                    err = 1;
                    for (int j = 0; j < graph->nrNodes; ++j) {
                        if (graph->v[i]->parent == graph->v[j]) {
                            if (transf[j] >= 0 && transf[j] < n) {
                                p[transf[i]] = transf[j]; //-V595
                                err = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    free(transf);
    // NOLINTNEXTLINE
    transf = nullptr;

    if (!err) {
        // TODO: pretty print the BFS tree
        // the parent array is p (p[k] is the parent for node k or -1 if k is the root)
        // when printing the node k, print repr[k] (it contains the row and column for that point)
        // you can adapt the code for transforming and printing multi-way trees from the previous labs
        pretty_print_r1(p, repr, n, -1, 0);
    }

    // NOLINTNEXTLINE
    if (p != nullptr) {
        free(p);
        // NOLINTNEXTLINE
        p = nullptr;
    }

    delete[] repr;
}

int shortest_path(Graph *graph, Node *start, Node *end, Node *path[]) {
    // TODO: compute the shortest path between the nodes start and end in the given graph
    // the nodes from the path, should be filled, in order, in the array path
    // the number of nodes filled in the path array should be returned
    // if end is not reachable from start, return -1
    // note: the size of the array path is guaranteed to be at least 1000
    return -1;
}


void performance() {
    int n, i;
    Profiler p("bfs");

    // vary the number of edges
    for (n = 1000; n <= 4500; n += 100) {
        Operation op = p.createOperation("bfs-edges", n);
        Graph graph;
        graph.nrNodes = 100;
        //initialize the nodes of the graph
        graph.v = static_cast<Node **>(malloc(graph.nrNodes * sizeof(Node *)));
        for (i = 0; i < graph.nrNodes; ++i) {
            graph.v[i] = static_cast<Node *>(malloc(sizeof(Node))); //-V522
            memset(graph.v[i], 0, sizeof(Node)); //-V575
        }
        // TODO: generate n random edges
        // make sure the generated graph is connected

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    // vary the number of vertices
    for (n = 100; n <= 200; n += 10) {
        Operation op = p.createOperation("bfs-vertices", n);
        Graph graph;
        graph.nrNodes = n;
        //initialize the nodes of the graph
        graph.v = static_cast<Node **>(malloc(graph.nrNodes * sizeof(Node *)));
        for (i = 0; i < graph.nrNodes; ++i) {
            graph.v[i] = static_cast<Node *>(malloc(sizeof(Node))); //-V522
            memset(graph.v[i], 0, sizeof(Node)); //-V575
        }
        // TODO: generate 4500 random edges
        // make sure the generated graph is connected

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    p.showReport();
}
