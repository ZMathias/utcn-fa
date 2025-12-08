/*
 * The get_neighbours function procedurally generates the neighbours if they are inside the boundaries
 * bfs correctly enqueues and colors nodes while traversing the graph. It is of complexity O(V + E)
 * shortest_path runs bfs and populates the path array going backwards through the parent pointers
 * performance uses generate edges which correctly generates a connected graph with the specified
 * amount of vertices and edges
 */

#include <cstdlib>
#include <string>
#include "bfs.h"

#include <climits>
#include <cmath>
#include <queue>
#include <random>

bool is_inside(const Grid *grid, const Point p) {
    const int lim_x = grid->cols;
    const int lim_y = grid->rows;

    return p.col >= 0 && p.col <= lim_x && p.row >= 0 && p.row <= lim_y;
}

int get_neighbors(const Grid *grid, Point p, Point neighbours[]) {
    int arr_it = 0;
    for (int i = 0; i < 4; i++) {
        constexpr int pos_x[] = {0, 0, -1, 1};
        constexpr int pos_y[] = {1, -1, 0, 0};
        Point translated = p;
        translated.col += pos_x[i];
        translated.row += pos_y[i];

        if (is_inside(grid, translated)) {
            if (grid->mat[translated.row][translated.col] == 0) {
                neighbours[arr_it++] = translated;
            }
        }
    }

    return arr_it;
}

void grid_to_graph(const Grid *grid, Graph *graph) {
    //we need to keep the nodes in a matrix, so we can easily refer to a position in the grid
    Node *nodes[MAX_ROWS][MAX_COLS];
    int i, j;
    Point neighbours[4];

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
        graph->v[i]->adjSize = get_neighbors(grid, graph->v[i]->position, neighbours);
        if (graph->v[i]->adjSize != 0) {
            graph->v[i]->adj = static_cast<Node **>(malloc(graph->v[i]->adjSize * sizeof(Node *)));
            k = 0;
            for (j = 0; j < graph->v[i]->adjSize; ++j) {
                if (neighbours[j].row >= 0 && neighbours[j].row < grid->rows &&
                    neighbours[j].col >= 0 && neighbours[j].col < grid->cols &&
                    grid->mat[neighbours[j].row][neighbours[j].col] == 0) {
                    graph->v[i]->adj[k++] = nodes[neighbours[j].row][neighbours[j].col]; //-V522
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
    for (int i = 0; i < graph->nrNodes; i++) {
        NodeT *node = graph->v[i];
        node->parent = nullptr;
        node->dist = INT_MAX;
        node->color = COLOR_WHITE;
        if (op) op->count(4);
    } // initialize all nodes

    std::queue<NodeT *> q;
    q.push(s);
    s->color = COLOR_GRAY;
    s->dist = 0;
    if (op) op->count(3);

    // enqueue all neighbours of s
    while (!q.empty()) {
        NodeT *curr = q.front();
        q.pop();

        if (op) op->count(2);

        for (int i = 0; i < curr->adjSize; i++) {
            if (op) op->count();
            if (curr->adj[i]->color == COLOR_WHITE) {
                curr->adj[i]->color = COLOR_GRAY; // enqueue turns nodes to grey
                curr->adj[i]->parent = curr;
                curr->adj[i]->dist = curr->dist + 1;
                q.push(curr->adj[i]);
                if (op) op->count(4);
            }
        }

        // after done processing turn the node black
        curr->color = COLOR_BLACK;
        if (op) op->count();
    }
}

void pretty_print_r1(int *parent, Point *repr, int size, int node, int depth) {
    if (parent == nullptr)
        return;

    for (int i = 0; i < size; i++) {
        if (parent[i] == node) {
            constexpr int spaces_per_depth = 8;
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
    int *trans = static_cast<int *>(malloc(graph->nrNodes * sizeof(int)));
    for (int i = 0; i < graph->nrNodes; ++i) {
        if (graph->v[i]->color == COLOR_BLACK) {
            trans[i] = n; //-V522
            ++n;
        } else {
            //-V522
            trans[i] = -1;
        }
    }
    if (n == 0) {
        //no BFS tree
        free(trans);
        return;
    }

    int err = 0;
    p = static_cast<int *>(malloc(n * sizeof(int)));
    repr = new Point[n];
    for (int i = 0; i < graph->nrNodes && !err; ++i) {
        if (graph->v[i]->color == COLOR_BLACK) {
            if (trans[i] < 0 || trans[i] >= n) {
                err = 1;
            } else {
                repr[trans[i]] = graph->v[i]->position;
                if (graph->v[i]->parent == nullptr) {
                    p[trans[i]] = -1; //-V522
                } else {
                    err = 1;
                    for (int j = 0; j < graph->nrNodes; ++j) {
                        if (graph->v[i]->parent == graph->v[j]) {
                            if (trans[j] >= 0 && trans[j] < n) {
                                p[trans[i]] = trans[j]; //-V595
                                err = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    free(trans);
    // NOLINTNEXTLINE
    trans = nullptr;

    if (!err) {
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

int shortest_path(const Graph *graph, Node *start, Node *end, Node *path[]) {
    bfs(graph, start);

    if (end->dist == INT_MAX) { // Check if end was reached
        return -1;
    }

    int length = 0;
    Node *curr = end;

    // Trace back from End to Start
    while (curr != nullptr) {
        path[curr->dist] = curr; // Place node at its correct distance index
        if (curr == start) break;
        curr = curr->parent;
    }

    return end->dist + 1; // Return number of nodes in the path
}

int random_number(const int lo, const int hi) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> distrib(lo, hi);
    return distrib(gen);
}

// this function generates a list of edges for vertices 0 - N-1
void generate_edges(const int V, const int E, NodeT** nodes) {
    if (nodes == nullptr) {
        return;
    }

    const int theoretical_edges = V * (V - 1) / 2;
    if (E > theoretical_edges) {
        fprintf(stderr, "requested edges %d, maximum edges %d\n", E, theoretical_edges);
        return;
    }

    if (E < V - 1) {
        fprintf(stderr, "E < V - 1 violates the connected property of the graph\n");
        return;
    }

    #define adj(x, y) graph[V * (x) + (y)]
    bool *graph = new bool[V * V];
    memset(graph, 0, V * V * sizeof(bool));

    int generated_count = 0;
    // backbone connectivity edges
    for (int i = 0; i < V - 1; i++) {
        adj(i, i + 1) = true;
        adj(i + 1, i) = true;
        generated_count++;
    }

    while (generated_count < E && generated_count < (V * (V - 1) / 2)) {
        const int u = random_number(0, V - 1);
        const int v = random_number(0, V - 1);

        if (u == v) continue; // no loops
        if (adj(u, v) > 0) continue; // no duplicates

        adj(u, v) = true;
        adj(v, u) = true;
        generated_count++;
    }

    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (!adj(i, j))
                continue;

            if (nodes[i]->adj == nullptr) {
                nodes[i]->adj = (NodeT**)malloc((V - 1) * sizeof(NodeT*)); // every node can have at most V - 1 outgoing edges with no loops
            }
            nodes[i]->adj[nodes[i]->adjSize++] = nodes[j]; // add node pointer to its adjacency list
        }
    }

    delete[] graph;
    #undef adj
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

        generate_edges(100, n, graph.v);

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
        generate_edges(n, 4500, graph.v);

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    p.showReport();
}

