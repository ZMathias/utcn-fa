#ifndef LAB08_DFS_H
#define LAB08_DFS_H
#include <list>
#include <Profiler.h>
#include <vector>

namespace lab10
{
    enum {
        COLOR_WHITE, COLOR_GRAY, COLOR_BLACK
    };

    struct Node {
        char color;
        int parent;
        std::list<int> adj;
    };

    using Graph = std::vector<Node>;

    void performance(Profiler profiler);
    void demonstrate();
}

#endif //LAB08_DFS_H