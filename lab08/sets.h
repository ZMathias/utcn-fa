#ifndef LAB07_SETS_H
#define LAB07_SETS_H
#include <Profiler.h>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <exception>
#include <map>
#include <tuple>
#include <random>

namespace lab08
{
    struct Set {
        Set* parent;
        int key;
        int rank;
    };

    struct Edge {
        int from, to;
        int weight;
    };

    int partition(Edge* values, int l, int r);
    void qsort(Edge* values, int l, int r);
    void quickSort(Edge* values, int n);
    Set* make_set(int x, Operation* op = nullptr);
    Set* find_set(Set* x, Operation* op = nullptr);
    void set_union(Set* x, Set* y, Operation* op = nullptr);
    void print_sets(const std::vector<Set*>& sets);
    void kruskal(int nr_vertices, Edge* edges, int size, Edge** mst, int *out_size, Operation* make_op = nullptr, Operation* union_op = nullptr, Operation* find_op = nullptr);
    void demonstrate();
    void generate_edges(int N, Edge** edges, int *out_size);
    void performance(Profiler &profiler);
}

#endif //LAB07_SETS_H