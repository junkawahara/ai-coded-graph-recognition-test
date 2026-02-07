#ifndef GRAPH_RECOGNITION_CHORDAL_H
#define GRAPH_RECOGNITION_CHORDAL_H

#include "graph.h"
#include "mcs.h"
#include <vector>

namespace graph_recognition {

// Result of chordal graph recognition.
struct ChordalResult {
    bool is_chordal;
    MCSResult mcs_result;
    std::vector<int> parent;              // parent[v] in PEO (0 if none)
    std::vector<std::vector<int>> later;  // later[v] = neighbors with higher MCS number
};

// Check whether a graph is chordal using MCS + PEO verification.
// If chordal, also computes the PEO, parent, and later-neighbor structure.
inline ChordalResult check_chordal(const Graph& g) {
    ChordalResult res;
    int n = g.n;
    res.mcs_result = mcs(g);
    const std::vector<int>& number = res.mcs_result.number;

    // Build later-neighbor lists.
    res.later.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (number[v] < number[u]) res.later[v].push_back(u);
        }
    }

    // Chordal check: for each vertex v, all later neighbors must be adjacent
    // to v's parent (the later neighbor with the smallest MCS number).
    res.parent.resize(n + 1, 0);
    res.is_chordal = true;
    for (int v = 1; v <= n; ++v) {
        int best = n + 1;
        for (size_t i = 0; i < res.later[v].size(); ++i) {
            int u = res.later[v][i];
            if (number[u] < best) {
                best = number[u];
                res.parent[v] = u;
            }
        }
        if (res.parent[v] == 0) continue;
        for (size_t i = 0; i < res.later[v].size(); ++i) {
            int u = res.later[v][i];
            if (u == res.parent[v]) continue;
            if (!g.has_edge(res.parent[v], u)) {
                res.is_chordal = false;
                return res;
            }
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
