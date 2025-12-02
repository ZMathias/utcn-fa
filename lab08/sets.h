#ifndef LAB07_SETS_H
#define LAB07_SETS_H
#include <Profiler.h>
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

    Set* make_set(int x);
    void set_union(Set* x, Set* y);

    void kruskal(int N, Edge* edges, const int size, Edge** mst, int *out_size);

    void demonstrate();
    void performance(Profiler &profiler);

}

#endif //LAB07_SETS_H