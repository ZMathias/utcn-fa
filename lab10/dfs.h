#ifndef LAB10_DFS_H
#define LAB10_DFS_H
#include <list>
#include <Profiler.h>
#include <vector>
#include <stack>

namespace lab10
{
    enum {
        COLOR_WHITE, COLOR_GRAY, COLOR_BLACK
    };

    struct Node {
        char color;
        int parent;
        int component;
        int time;
        std::list<int> adj;
    };

    using Graph = std::vector<Node>;

    void dfs_rec(Graph& g, int from, int c, int& time, std::list<int>& topo, Operation* op);
    int dfs(Graph& g, Operation* op = nullptr);
    void pretty_print(const Graph& g, int component, int parent_idx = -1, int depth = 0);
    void strong_connect(Graph& g, int& index, Node* v, std::stack<int>& st, std::vector<int>& low_link, std::vector<bool>& on_stack);
    void tarjan(Graph& g);
    void reset_graph(Graph& g);

    void performance(Profiler& profiler);
    void demonstrate();
}

#endif //LAB10_DFS_H