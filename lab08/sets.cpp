#include "sets.h"

/*
 * 1. Complexity Analysis:
 * MAKE_SET is O(1).
 * UNION and FIND_SET operations take nearly O(1) time on average due to
 * Path Compression and Union by Rank heuristics.
 * Kruskal's algorithm is dominated by sorting the edges, O(E log E).
 *
 * make operation grows linearly with N
 * find operation count is significantly higher than union, as Kruskal
 * performs 2 finds for every edge considered.
 * due to path compression, the cost of FIND operations remains very low
 * even as N increases.
 */

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

    Set* make_set(const int x, Operation* op) {
        Set* elem = new Set;
        elem->key = x;
        elem->parent = nullptr;
        elem->rank = 0;
        if (op) op->count(4); // 1 alloc 3 assignments
        return elem;
    }

    Set* find_set(Set* x, Operation* op) {
        if (x == nullptr) {
            return nullptr;
        }

        if (op) op->count();
        if (x->parent == nullptr) {
            return x;
        }

        Set* rep = find_set(x->parent, op);

        if (op) op->count();
        x->parent = rep; // do path compression

        return rep;
    }

    void set_union(Set* x, Set* y, Operation* op) {
        Set* x_rep = find_set(x, op);
        Set* y_rep = find_set(y, op);

        if (x_rep == nullptr || y_rep == nullptr) {
            throw std::exception();
        }

        if (x_rep == y_rep) {
            return;
        }


        if (x_rep->rank < y_rep->rank) {
            if (op) op->count(2);
            x_rep->parent = y_rep;
        } else if (x_rep->rank > y_rep->rank) {
            if (op) op->count(2);
            y_rep->parent = x_rep;
        } else {
            y_rep->parent = x_rep;
            x_rep->rank++;
            if (op) op->count(2);
        }

        if (op && x_rep->rank >= y_rep->rank) { // adjust for missed op counts when execution misses branch
            op->count();
            if (x_rep->rank <= y_rep->rank) op->count();
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

    void kruskal(int nr_vertices, Edge* edges, const int size, Edge** mst, int *out_size, Operation* make_op, Operation* union_op, Operation* find_op) {
        if (edges == nullptr) {
            return;
        }

        // the efficient version
        /*Set* vertices = new Set[nr_vertices];

        for (int i = 0; i < nr_vertices; i++) {
            vertices[i].key =  i;
            vertices[i].parent = nullptr;
            vertices[i].rank = 0;
        }*/

        // the benchmark version
        Set** vertices = new Set*[nr_vertices];
        for (int i = 0; i < nr_vertices; i++) {
            vertices[i] = make_set(i, make_op);
        }

        *out_size = 0;
        *mst = new Edge[nr_vertices - 1]; // allocate data for the list of edges, maximum N - 1

        quickSort(edges, size); // sort the list of edges

        for (int i = 0; i < size; i++) { // loop through edges
            if (*out_size == nr_vertices - 1) {
                break; // we have found the mst
            }

            const int v_f = edges[i].from;
            const int v_t = edges[i].to;

            Set* s_f = vertices[v_f];
            Set* s_t = vertices[v_t];

            if (find_set(s_f, find_op) != find_set(s_t, find_op)) { // if they are on separate subgraphs, merge them
                set_union(s_f, s_t, union_op);

                (*mst)[*out_size] = edges[i]; // add edge to solution list
                (*out_size)++;
            }
        }

        for (int i = 0; i < nr_vertices; i++) {
            delete vertices[i];
        }
        delete[] vertices;
    }

    // this function generates a list of edges for vertices 0 - N-1
    void generate_edges(int N, Edge** edges, int *out_size) {
        if (out_size == nullptr || edges == nullptr) {
            return;
        }

        #define adj(x, y) graph[N * (x) + (y)]
        int *graph = new int[N * N];
        memset(graph, 0, N * N * sizeof(int));

        int generated_count = 0;
        int limit_edges = 4 * N;
        // backbone connectivity edges
        for (int i = 0; i < N - 1; i++) {
            int w = random_number(1, 3600);
            adj(i, i + 1) = w;
            adj(i + 1, i) = w;
            generated_count++;
        }

        while (generated_count < limit_edges && generated_count < (N * (N - 1) / 2)) {
            int u = random_number(0, N - 1);
            int v = random_number(0, N - 1);

            if (u == v) continue; // no loops
            if (adj(u, v) > 0) continue; // no duplicates

            int w = random_number(1, 3600);
            adj(u, v) = w;
            adj(v, u) = w;
            generated_count++;
        }

        *out_size = generated_count;
        *edges = new Edge[generated_count];

        int k = 0;
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (adj(i, j) > 0) {
                    (*edges)[k++] = {i, j, adj(i, j)};
                }
            }
        }

        delete[] graph;
        #undef adj
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

        printf("Input Graph Edges:\n");
        for(auto & edge : edges) {
            printf("%d-%d: %d\n", edge.from, edge.to, edge.weight);
        }

        Edge* mst = nullptr;
        int size = 0;
        kruskal(5, edges, 9, &mst, &size);

        printf("\nMST is:\n");
        for (int i = 0; i < size; i++) {
            printf("%d-%d\n", mst[i].from, mst[i].to);
        }

        delete[] mst;

        Edge* edge_list = nullptr;
        int nr_edges = 0;
        generate_edges(10, &edge_list, &nr_edges);

        printf("\nGenerated graph edges:\n");
        for (int i = 0; i < nr_edges; i++) {
            printf("{%d, %d, %d}\n", edge_list[i].from, edge_list[i].to, edge_list[i].weight);
        }

        delete[] edge_list;

        for (auto & set : sets) {
            delete set;
        }
    }

    void performance(Profiler &profiler) {
        for (int repeat = 0; repeat < 5; repeat++) {
            for (int n = 100; n <= 10000; n += 100) {
                //printf("n = %d ", n);
                //fflush(stdout);
                int nr_edges = 0;
                Edge* edges = nullptr;
                generate_edges(n, &edges, &nr_edges);
                //printf("generated edges ");
                //fflush(stdout);
                Operation make_op = profiler.createOperation("make", n);
                Operation union_op = profiler.createOperation("union", n);
                Operation find_op = profiler.createOperation("find", n);

                Edge* mst = nullptr;
                int nr_mst_edges = 0;
                kruskal(n, edges, nr_edges, &mst, &nr_mst_edges, &make_op, &union_op, &find_op);
                //printf("mst done\n");
                //fflush(stdout);
                delete[] mst;
                delete[] edges;
            }
        }

        profiler.divideValues("make", 5);
        profiler.divideValues("union", 5);
        profiler.divideValues("find", 5);
        profiler.createGroup("Set operations", "make", "union", "find");
    }
}

