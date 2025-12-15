#include "dfs.h"
#include <print>

namespace lab10
{
    void dfs_rec(Graph& g, int from) {
        for (auto n_idx : g[from].adj) {
            Node* neighb = &g[n_idx];
            if (neighb->color == COLOR_WHITE) {
                neighb->color = COLOR_GRAY;
                neighb->parent = from; // set current node as parent
                const int idx = neighb - g.data(); // subtract the address of the node from the vector base address gives element index in array
                dfs_rec(g, idx);
            }
        }

        g[from].color = COLOR_BLACK;
    }

    void dfs(Graph& g) {
        for (int i = 0; i < g.size(); i++) {
            if (g[i].color == COLOR_WHITE) {
                g[i].color = COLOR_GRAY;
                dfs_rec(g, i);
            }
        }
    }

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

    void print_graph(const Graph& g) {
        for (int i = 0; i < g.size(); i++) {
            std::print("{}: [", i);
            for (const auto to : g[i].adj) {
                if (to) {
                    std::print(" {} ", to);
                }
            }
            std::println("]");
        }
    }

    void demonstrate() {
        constexpr int numNodes = 6;
        std::vector<Node> g(numNodes, {COLOR_WHITE, -1, std::list<int>{}});
        g[0].adj = {1, 2};
        g[1].adj = {2, 3};
        g[2].adj = {3};
        g[3].adj = {4};
        g[4].adj = {5};

        print_graph(g);

        // TODO: call dfs
        dfs(g);
    }

    void performance(Profiler profiler) {
        return;
    }
}