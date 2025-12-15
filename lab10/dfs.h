#ifndef LAB08_DFS_H
#define LAB08_DFS_H
#include <list>
#include <variant>
#include <vector>

namespace lab10
{
    void dfs(std::vector<std::list<int>>& adj) {
        std::vector<bool> visited(adj.size(), false);
        std::vector<int> par_arr;
    }

    void dfs_rec(std::vector<std::list<int>>& adj, std::vector<bool>& visited, std::vector<int>& par_arr, int source, int parent) {
        if (!visited[source]) {
            visited[source] = true;
            // do something if needed
            if (source > par_arr.size()) {
                par_arr.resize(source + 1);
            }
            par_arr[source] = parent;

            for (auto edge : adj[source]) {
                if (!visited[edge]) {
                    dfs_rec(adj, visited, par_arr, edge, source);
                }
            }
        }
    }

    void demonstrate() {
        constexpr int numNodes = 6;
        std::vector<std::list<int>> adj(numNodes);

        adj[0].push_back(1);
        adj[0].push_back(2);
        adj[1].push_back(3);
        adj[1].push_back(4);
        adj[2].push_back(5);
        adj[5].push_back(0);


    }
}

#endif //LAB08_DFS_H