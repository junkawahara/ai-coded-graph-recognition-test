#ifndef GRAPH_RECOGNITION_COCHAIN_H
#define GRAPH_RECOGNITION_COCHAIN_H

#include "chain.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of cochain graph recognition.
struct CochainResult {
    bool is_cochain;
};

namespace detail {

inline Graph build_complement_graph(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

} // namespace detail

// Check whether a graph is a cochain graph.
// Characterization used:
//   G is cochain iff complement(G) is chain.
inline CochainResult check_cochain(const Graph& g) {
    CochainResult res;
    res.is_cochain = false;

    Graph gc = detail::build_complement_graph(g);
    ChainResult cres = check_chain(gc);
    if (!cres.is_chain) return res;

    res.is_cochain = true;
    return res;
}

} // namespace graph_recognition

#endif
