#ifndef LAB08_DFS_H
#define LAB08_DFS_H
#include <list>
#include <Profiler.h>
#include <vector>

namespace lab10
{
    enum {
        COLOR_WHITE, COLOR_GRAY, COLOR_BLACK;
    };

    struct Node {
        char color;
        int parent;
        std::list<Node*> adj;
    };

    std::list<std::vector<int>> dfs(std::vector<std::list<int>>& adj);
    void dfs_rec(std::vector<std::list<int>>& adj, std::vector<bool>& visited, std::vector<int>& par_arr, int source, int parent);
    void pretty_print(const std::vector<int>& parent, int node = -1, int depth = 0);

    void performance(Profiler profiler);
    void demonstrate();
}

#endif //LAB08_DFS_H