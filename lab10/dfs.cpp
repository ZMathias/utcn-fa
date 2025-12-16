#include "dfs.h"
#include <print>
#include <random>

/*
 * DFS has a complexity of O(V + E) as seen in the generated charts
 * The code correctly tests this by fixing V and varying E then vice-versa
 */

namespace lab10
{
    int random_number(const int lo, const int hi) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> distrib(lo, hi);
        return distrib(gen);
    }

    void dfs_rec(Graph& g, int from, int c, int& time, std::list<int>& topo, Operation* op) {
        g[from].color = COLOR_GRAY;
        g[from].component = c;
        g[from].time = ++time;

        if (op) op->count(3);

        for (const auto n_idx : g[from].adj) {
            Node* neighbour = &g[n_idx];

            switch (neighbour->color) {
            case COLOR_WHITE:
                {
                    neighbour->color = COLOR_GRAY;
                    neighbour->component = c;
                    neighbour->parent = from; // set current node as parent
                    if (op) op->count(4);
                    const int idx = neighbour - g.data(); // subtract the address of the node from the vector base address gives element index in array
                    dfs_rec(g, idx, c, time, topo, op);
                    break;
                }
            case COLOR_GRAY: // on the recursion stack, not finished, direct ancestor
                {
                    // back edge, cycle detected
                    std::println("Back edge: ({} -> {})", from, n_idx);
                    if (op) op->count();
                    topo.clear(); // this is a bit slow but not necessary to keep here
                    topo.push_front(-1); // sentinel value to stop
                    break;
                }
            case COLOR_BLACK: // already finished
                {
                    if (g[from].time < neighbour->time) { // forward edge
                        std::println("Forward edge: ({} -> {})", from, n_idx);
                    } else { // cross edge
                        std::println("Cross edge: ({} -> {})", from, n_idx);
                    }
                    if (op) op->count(2);
                    break;
                }
            default:
                break;
            }
        }

        if (op) op->count();
        g[from].color = COLOR_BLACK;
        if (!topo.empty() && topo.front() != -1) {
            topo.push_front(from);
        }
    }

    int dfs(Graph& g, Operation* op) {
        reset_graph(g);
        int c = 0;
        std::list<int> topo;
        for (int i = 0; i < g.size(); i++) {
            if (op) op->count();
            if (g[i].color == COLOR_WHITE) {
                int time = 0;
                topo.emplace_back(-100); // placeholder for list to not be empty
                dfs_rec(g, i, c++, time, topo, op);
                if (!topo.empty() && topo.front() != -1) {
                    topo.pop_back();
                    std::println("Topological sort of component {}: {}", c - 1, topo);
                } else {
                    std::println("Topological sort not possible because of cycles in the graph!");
                }
                topo.clear();
            }
        }

        return c;
    }

    void strong_connect(Graph& g, int& index, Node* v, std::stack<int>& st, std::vector<int>& low_link, std::vector<bool>& on_stack) {
        const int v_idx = v - g.data();
        v->time = index;
        low_link[v_idx] = index;
        index++;
        st.push(v_idx);
        on_stack[v_idx] = true;

        // consider successors of v
        for (const auto w_idx : v->adj) {
            Node* w = &g[w_idx];
            if (w->time == -1) {
                strong_connect(g, index, w, st, low_link, on_stack);
                low_link[v_idx] = std::min(low_link[v_idx], low_link[w_idx]);
            } else if (on_stack[w_idx]) {
                low_link[v_idx] = std::min(low_link[v_idx], w->time);
            }
        }

        if (low_link[v_idx] == v->time) {
            // start a new strongly connected component
            std::print("scc: [");
            const Node* w = nullptr;
            do {
                const int w_idx = st.top();
                w = &g[w_idx];
                st.pop();
                on_stack[w_idx] = false;
                std::print(" {} ", w_idx);
            } while (w != v);
            std::println("]");
        }
    }

    void tarjan(Graph& g) {
        reset_graph(g);
        std::stack<int> st;
        int index = 0;

        std::vector low_link(g.size(), -1);
        std::vector on_stack(g.size(), false);
        for (int i = 0; i < g.size(); i++) {
            if (g[i].time == -1) {
                strong_connect(g, index, &g[i], st, low_link, on_stack);
            }
        }
    }

    Graph generate_edges(const int V, const int E) {
        const int theoretical_edges = V * (V - 1) / 2;
        if (E > theoretical_edges) {
            fprintf(stderr, "requested edges %d, maximum edges %d\n", E, theoretical_edges);
            return {};
        }

        if (E < V - 1) {
            fprintf(stderr, "E < V - 1 violates the connected property of the graph\n");
            return {};
        }

#define adj(x, y) mat[V * (x) + (y)]
        bool *mat = new bool[V * V];
        memset(mat, 0, V * V * sizeof(bool));

        int generated_count = 0;
        // backbone connectivity edges forming the cycle
        for (int i = 0; i < V - 1; i++) {
            adj(i, i + 1) = true;
            generated_count++;
        }

        adj(V - 1, 0) = true; // cycle back edge

        while (generated_count < E && generated_count < (V * (V - 1) / 2)) {
            const int u = random_number(0, V - 1);
            const int v = random_number(0, V - 1);

            if (u == v) continue; // no loops
            if (adj(u, v) > 0) continue; // no duplicates

            adj(u, v) = true; // only directed edges
            generated_count++;
        }

        Graph g(V);
        reset_graph(g);

        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (!adj(i, j))
                    continue;

                g[i].adj.push_back(j);
            }
        }

        delete[] mat;
#undef adj
        return g;
    }

    void pretty_print(const Graph& g, const int component, const int parent_idx, const int depth) {
        // NOLINTNEXTLINE
        constexpr int spaces_per_depth = 2;

        for (int i = 0; i < g.size(); i++) {
            if (g[i].component == component && g[i].parent == parent_idx) {
                for (int x = 0; x < depth * spaces_per_depth; x++) {
                    std::print(" ");
                }
                std::print("{}\n", i);
                pretty_print(g, component, i, depth + 1);
            }
        }
    }

    void reset_graph(Graph& g) {
        for (auto& v  : g) {
            v.component = 0;
            v.parent = -1;
            v.time = -1;
            v.color = COLOR_WHITE;
        }
    }

    void print_graph(const Graph& g) {
        for (int i = 0; i < g.size(); i++) {
            std::print("{}: [", i);
            for (const auto& to : g[i].adj) {
                std::print(" {} ", to);
            }
            std::println("]");
        }
    }

    void demonstrate() {
        // demo 1.3.1 and 1.3.2
        // comment g[5].adj for 1.3.2
        constexpr int numNodes = 11;
        std::vector<Node> g(numNodes, {COLOR_WHITE, -1, 0, -1, std::list<int>{}});
        g[0].adj = {1, 2, 7};
        g[1].adj = {3};
        g[2].adj = {4};
        g[3].adj = {5};
        g[4].adj = {3, 6, 7};
        g[8].adj = {9};
        g[9].adj = {10};
        //g[5].adj = {1};

        std::println("Graph before DFS:");
        print_graph(g);

        const int num_c = dfs(g);

        for (int i = 0; i < num_c; ++i) {
            std::println("\nTree of component {}:", i);
            pretty_print(g, i);
        }

        // demo 1.3.3
        constexpr int num_tarjan_nodes = 8;
        Graph t(num_tarjan_nodes, {COLOR_WHITE, -1, 0, -1, std::list<int>{}});

        // scc 1
        t[0].adj = {1};
        t[1].adj = {2, 3};
        t[2].adj = { 0 };

        // scc 2
        t[3].adj = {4};
        t[4].adj = {5};
        t[5].adj = {3, 6};

        // scc 3
        t[6].adj = {7};
        t[7].adj = {6};

        std::println("\nGraph before Tarjan:");
        print_graph(t);

        tarjan(t);
    }

    void performance(Profiler& profiler) {
        for (int tries = 0; tries < 5; tries++) {
            for (int E = 1000; E <= 4500; E += 100) {
                Graph g = generate_edges(100, E);

                Operation e_op = profiler.createOperation("e_op", E);
                dfs(g, &e_op);
            }

            for (int V = 100; V <= 200; V += 10) {
                Graph g = generate_edges(V, 4500);

                Operation v_op = profiler.createOperation("v_op", V);
                dfs(g, &v_op);
            }
        }

        profiler.divideValues("e_op", 5);
        profiler.divideValues("v_op", 5);

        profiler.createGroup("Varying E", "e_op");
        profiler.createGroup("Varying V", "v_op");
    }
}