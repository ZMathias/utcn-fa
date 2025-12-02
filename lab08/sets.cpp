#include "sets.h"

#include <set>

namespace lab08
{
    int random_number(int lo, int hi) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> distrib(lo, hi);
        return distrib(gen);
    }

    int partition(Edge* values, const int l, const int r) {
        const int rand_idx = random_number(l, r);
        std::swap(values[r], values[rand_idx]);

        const int pivot = values[r].weight; // always pick the last element as pivot
        int i = l;
        for (int j = l; j < r; j++) {
            if (values[j].weight <= pivot) {
                if (i != j) {
                    std::swap(values[i], values[j]);
                }
                i++;
            }
        }
        std::swap(values[r], values[i]);
        return i;
    }

    void qsort(Edge* values, const int l, const int r) {
        if (l >= r) {
            return;
        }

        const int pivot = partition(values, l, r);

        qsort(values, l, pivot - 1);
        qsort(values, pivot + 1, r);
    }

    void quickSort(Edge* values, const int n)
    {
        qsort(values, 0, n - 1);
    }

    Set* make_set(const int x) {
        Set* elem = new Set;
        elem->key = x;
        elem->parent = nullptr;
        elem->rank = 0;
        return elem;
    }

    Set* find_set(Set* x) {
        if (x == nullptr) {
            return nullptr;
        }

        if (x->parent == nullptr) {
            return x;
        }

        Set* rep = find_set(x->parent);

        x->parent = rep; // do path compression

        return rep;
    }

    void set_union(Set* x, Set* y) {
        Set* x_rep = find_set(x);
        Set* y_rep = find_set(y);

        if (x_rep == nullptr || y_rep == nullptr) {
            throw std::exception();
        }

        if (x_rep == y_rep) {
            return;
        }

        if (x_rep->rank < y_rep->rank) {
            x_rep->parent = y_rep;
        } else if (x_rep->rank > y_rep->rank) {
            y_rep->parent = x_rep;
        } else {
            y_rep->parent = x_rep;
            x_rep->rank++;
        }
    }

    void print_sets(const std::vector<Set*>& sets) {
        std::map<int, std::vector<Set*>> unique_sets;
        for (auto elem : sets) {
            unique_sets[find_set(elem)->key].emplace_back(elem);
        }

        for (auto set : unique_sets) {
            printf("(%d) {", find_set(set.second[0])->key);
            for (const auto elem : set.second) {
                printf(" %d ", elem->key);
            }
            printf("}\n");
        }
    }

    void kruskal(int N, Edge* edges, const int size, Edge** mst, int *out_size) {
        if (edges == nullptr) {
            return;
        }

        Set* vertices = new Set[N];

        for (int i = 0; i < N; i++) {
            vertices[i].key =  i;
            vertices[i].parent = nullptr;
            vertices[i].rank = 0;
        }

        *out_size = 0;
        *mst = new Edge[N - 1]; // allocate data for the list of edges, maximum N - 1

        quickSort(edges, size); // sort the list of edges

        for (int i = 0; i < size; i++) { // loop through edges
            if (*out_size == N - 1) {
                break; // we have found an mst
            }

            const int v_f = edges[i].from;
            const int v_t = edges[i].to;

            Set s_f = vertices[v_f];
            Set s_t = vertices[v_t];

            if (find_set(&s_f) != find_set(&s_t)) { // if they are on separate subgraphs, merge them
                set_union(&s_f, &s_t);

                (*mst)[*out_size] = edges[i]; // add edge to solution list
                (*out_size)++;
            }
        }

        delete[] vertices;
    }

    void demonstrate() {
        // demo 1.3.1
        std::vector<Set*> sets;
        for (int i = 1; i <= 10; i++) {
            sets.emplace_back(make_set(i));
        }
        printf("After making the sets:\n");
        print_sets(sets);

        set_union(sets[0], sets[1]);
        set_union(sets[1], sets[2]);
        set_union(sets[2], sets[3]);
        set_union(sets[3], sets[4]);
        set_union(sets[9], sets[5]);

        printf("After unions:\n");
        print_sets(sets);

        printf("representative of (%d): %d\n", 4, find_set(sets[3])->key);
        printf("representative of (%d): %d\n", 6, find_set(sets[5])->key);
        printf("representative of (%d): %d\n", 10, find_set(sets[9])->key);

        // demo 1.3.2
        Edge edges[9];
        edges[0] = {0, 1, 3};
        edges[1] = {0, 3, 1};
        edges[2] = {1, 2, 7};
        edges[3] = {1, 4, 8};
        edges[4] = {1, 3, 3};
        edges[5] = {2, 3, 5};
        edges[6] = {2, 4, 6};
        edges[7] = {3, 4, 15};
        edges[8] = {0, 4, 10};

        Edge* mst = nullptr;
        int size = 0;
        kruskal(5, edges, 9, &mst, &size);

        putchar('\n');
        for (int i = 0; i < size; i++) {
            printf("%d-%d\n", mst[i].from, mst[i].to);
        }

        delete[] mst;

        for (auto & set : sets) {
            delete set;
        }
    }

    // this function generates a list of edges for vertices 0 - N-1
    void generateEdges(int N, Edge** edges, int *out_size) {
        // collect into a list of edges
        if (out_size == nullptr) {
            fprintf(stderr, "out_size can not be nullptr!");
            return;
        }

        if (edges == nullptr) {
            fprintf(stderr, "edges can not be nullptr!");
            return;
        }

        #define adj(x, y) graph[N * x + y]
        int *graph = new int[N * N];
        int nr_edges = 0;

        memset(graph, 0, N * N * sizeof(int)); // zero the adjacency matrix
        for (int i = 0; i < N - 1; i++) {
            adj(i, i + 1) = adj(i + 1, i) = random_number(1, 50); // connect 0->1, 1->2 to generate a connected graph
            nr_edges++;
        }


        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (adj(i, j) > 0) continue; // only single edges, no more than one edge per vertex pair
                const int put_chance = random_number(1, 10);
                if (put_chance >= 5) { // put an edge 50% of the time
                    adj(i, j) = adj(j, i) = random_number(1, 50);
                    nr_edges++;
                }
            }
        }

        *out_size = 0;
        *edges = new Edge[nr_edges];
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (adj(i, j) > 0) {
                    (*edges)[(*out_size)] = {i, j, adj(i, j)};
                    (*out_size)++;
                }
            }
        }

        delete[] graph;
    }

    void performance(Profiler &profiler) {

    }
}

