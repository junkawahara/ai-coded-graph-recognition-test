#ifndef GRAPH_RECOGNITION_BLOCK_H
#define GRAPH_RECOGNITION_BLOCK_H

#include "graph.h"
#include <algorithm>
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of block graph recognition.
struct BlockResult {
    bool is_block;
};

namespace detail {

class BlockChecker {
public:
    explicit BlockChecker(const Graph& graph)
        : g(graph),
          adj(g.n + 1),
          tin(g.n + 1, 0),
          low(g.n + 1, 0),
          mark(g.n + 1, 0),
          timer(0),
          mark_token(0) {
        build_simple_graph();
    }

    bool run() {
        for (int v = 1; v <= g.n; ++v) {
            if (tin[v] != 0) continue;
            if (!dfs(v, -1)) return false;
            if (!edge_stack.empty()) return false;
        }
        return true;
    }

private:
    struct UEdge {
        int u, v;
    };

    const Graph& g;
    std::vector<UEdge> edges;
    std::vector<std::vector<std::pair<int, int>>> adj;  // (to, edge id)
    std::vector<int> tin, low;
    std::vector<int> edge_stack;
    std::vector<int> mark;
    int timer;
    int mark_token;

    void build_simple_graph() {
        for (int u = 1; u <= g.n; ++u) {
            for (std::unordered_set<int>::const_iterator it = g.adj_set[u].begin();
                 it != g.adj_set[u].end(); ++it) {
                int v = *it;
                if (u >= v) continue;
                int eid = (int)edges.size();
                UEdge e;
                e.u = u;
                e.v = v;
                edges.push_back(e);
                adj[u].push_back(std::make_pair(v, eid));
                adj[v].push_back(std::make_pair(u, eid));
            }
        }
    }

    bool pop_component_and_check_clique(int stop_eid) {
        if (mark_token == INT_MAX) {
            std::fill(mark.begin(), mark.end(), 0);
            mark_token = 0;
        }
        mark_token++;

        std::vector<int> verts;
        int edge_count = 0;
        while (true) {
            if (edge_stack.empty()) return false;
            int eid = edge_stack.back();
            edge_stack.pop_back();
            edge_count++;

            int a = edges[eid].u;
            int b = edges[eid].v;
            if (mark[a] != mark_token) {
                mark[a] = mark_token;
                verts.push_back(a);
            }
            if (mark[b] != mark_token) {
                mark[b] = mark_token;
                verts.push_back(b);
            }
            if (eid == stop_eid) break;
        }

        long long k = (long long)verts.size();
        long long need = k * (k - 1) / 2;
        return (long long)edge_count == need;
    }

    bool dfs(int v, int parent_eid) {
        tin[v] = low[v] = ++timer;

        for (size_t i = 0; i < adj[v].size(); ++i) {
            int to = adj[v][i].first;
            int eid = adj[v][i].second;
            if (eid == parent_eid) continue;

            if (tin[to] == 0) {
                edge_stack.push_back(eid);
                if (!dfs(to, eid)) return false;
                low[v] = std::min(low[v], low[to]);

                if (low[to] >= tin[v]) {
                    if (!pop_component_and_check_clique(eid)) return false;
                }
            } else if (tin[to] < tin[v]) {
                edge_stack.push_back(eid);
                low[v] = std::min(low[v], tin[to]);
            }
        }

        return true;
    }
};

} // namespace detail

// Check whether a graph is a block graph.
// A graph is block iff every biconnected component is a clique.
inline BlockResult check_block(const Graph& g) {
    BlockResult res;
    detail::BlockChecker checker(g);
    res.is_block = checker.run();
    return res;
}

} // namespace graph_recognition

#endif
