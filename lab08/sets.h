#ifndef LAB07_SETS_H
#define LAB07_SETS_H
#include <algorithm>
#include <commandline.h>
#include <Profiler.h>
#include <random>

namespace lab08
{
    struct Node {
        Node* p;
        int key;
        int rank;
    };

    void delete_set(Node** root);
    void pretty_print(const Node* root, int depth = 0);
    void demonstrate(int l, int r);
    void performance(Profiler &profiler);

}

#endif //LAB07_SETS_H