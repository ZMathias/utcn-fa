#include "dfs.h"
#include <print>

namespace lab10
{
    void pretty_print(const std::vector<int>& parent, const int node, const int depth) {
        constexpr int spaces_per_depth = 2;

        for (int i = 0; i < parent.size(); i++) {
            if (parent[i] == node) {
                for (int x = 0; x < depth * spaces_per_depth; x++) {
                    std::print(" ");
                }
                std::print("{}\n", i);
                pretty_print(parent, i, depth + 1);
            }
        }
    }

    void demonstrate() {
        constexpr int numNodes = 6;
        std::vector<Node> adj(numNodes, {COLOR_WHITE, -1, std::list<Node*>{}});
        adj[0].adj = {&adj[1], &adj[2]};
        adj[1].adj = {&adj[2], &adj[3]};
        adj[2].adj = {&adj[3]};
        adj[3].adj = {&adj[4]};
        adj[4].adj = {&adj[5]};
    }

    void performance(Profiler profiler) {
        return;
    }
}