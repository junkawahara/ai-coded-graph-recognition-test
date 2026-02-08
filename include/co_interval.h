#ifndef GRAPH_RECOGNITION_CO_INTERVAL_H
#define GRAPH_RECOGNITION_CO_INTERVAL_H

#include "graph.h"
#include "interval.h"
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of co-interval graph recognition.
struct CoIntervalResult {
    bool is_co_interval;
};

namespace detail_co_interval {

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

} // namespace detail_co_interval

// Check whether a graph is a co-interval graph.
// Characterization used:
//   G is co-interval iff complement(G) is interval.
inline CoIntervalResult check_co_interval(const Graph& g) {
    CoIntervalResult res;
    res.is_co_interval = false;

    Graph gc = detail_co_interval::build_complement_graph(g);
    IntervalResult ires = check_interval(gc);
    if (!ires.is_interval) return res;

    res.is_co_interval = true;
    return res;
}

} // namespace graph_recognition

#endif
