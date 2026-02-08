#ifndef GRAPH_RECOGNITION_CHAIN_H
#define GRAPH_RECOGNITION_CHAIN_H

#include "bipartite.h"
#include "graph.h"
#include <vector>

namespace graph_recognition {

// Result of chain graph recognition.
struct ChainResult {
    bool is_chain;
};

namespace detail {

// Check whether neighborhoods of vertices in "side" are linearly ordered
// by inclusion with respect to vertices in "other_side".
inline bool is_nested_neighborhood_side(
    const Graph& g,
    const std::vector<int>& side,
    const std::vector<int>& other_side) {
    for (size_t i = 0; i < side.size(); ++i) {
        int u = side[i];
        for (size_t j = i + 1; j < side.size(); ++j) {
            int v = side[j];
            bool u_subset_v = true;
            bool v_subset_u = true;
            for (size_t k = 0; k < other_side.size(); ++k) {
                int w = other_side[k];
                bool uw = g.has_edge(u, w);
                bool vw = g.has_edge(v, w);
                if (uw && !vw) u_subset_v = false;
                if (vw && !uw) v_subset_u = false;
                if (!u_subset_v && !v_subset_u) return false;
            }
        }
    }
    return true;
}

} // namespace detail

// Check whether a graph is a chain graph.
// Characterization used:
//   G is chain iff G is bipartite and neighborhoods on one side are nested.
inline ChainResult check_chain(const Graph& g) {
    ChainResult res;
    res.is_chain = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    std::vector<int> left, right;
    left.reserve(g.n);
    right.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    // Checking one side is sufficient for chain graphs.
    if (!detail::is_nested_neighborhood_side(g, left, right)) return res;

    res.is_chain = true;
    return res;
}

} // namespace graph_recognition

#endif
