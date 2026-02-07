#ifndef GRAPH_RECOGNITION_SPLIT_H
#define GRAPH_RECOGNITION_SPLIT_H

#include "chordal.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of split graph recognition.
struct SplitResult {
    bool is_split;
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

// Check whether a graph is a split graph.
// Characterization used:
//   G is split iff G and its complement are chordal.
inline SplitResult check_split(const Graph& g) {
    SplitResult res;
    res.is_split = false;

    ChordalResult g_chordal = check_chordal(g);
    if (!g_chordal.is_chordal) return res;

    Graph gc = detail::build_complement_graph(g);
    ChordalResult gc_chordal = check_chordal(gc);
    if (!gc_chordal.is_chordal) return res;

    res.is_split = true;
    return res;
}

} // namespace graph_recognition

#endif
