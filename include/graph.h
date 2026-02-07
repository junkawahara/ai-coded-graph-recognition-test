#ifndef GRAPH_RECOGNITION_GRAPH_H
#define GRAPH_RECOGNITION_GRAPH_H

#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

// 1-indexed undirected graph with adjacency list and adjacency set.
struct Graph {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<std::unordered_set<int>> adj_set;

    Graph() : n(0), adj(1), adj_set(1) {}

    Graph(int n, const std::vector<std::pair<int, int>>& edges)
        : n(n), adj(n + 1), adj_set(n + 1) {
        for (size_t i = 0; i < edges.size(); ++i) {
            int u = edges[i].first, v = edges[i].second;
            if (u == v) continue;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        for (int v = 1; v <= n; ++v) {
            adj_set[v].reserve(adj[v].size() * 2 + 1);
            for (size_t i = 0; i < adj[v].size(); ++i) {
                adj_set[v].insert(adj[v][i]);
            }
        }
    }

    bool has_edge(int u, int v) const {
        return adj_set[u].count(v) > 0;
    }

    // Read a graph from input stream.  Format: n m, then m lines of u v.
    static Graph read(std::istream& in) {
        int n, m;
        if (!(in >> n >> m)) return Graph();
        std::vector<std::pair<int, int>> edges;
        edges.reserve(m);
        for (int i = 0; i < m; ++i) {
            int u, v;
            in >> u >> v;
            edges.push_back(std::make_pair(u, v));
        }
        return Graph(n, edges);
    }
};

} // namespace graph_recognition

#endif
