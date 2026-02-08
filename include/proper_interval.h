#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_H

#include "graph.h"
#include "interval.h"
#include <vector>

namespace graph_recognition {

// Result of proper interval graph recognition.
struct ProperIntervalResult {
    bool is_proper_interval;
};

namespace detail {

// Check whether graph contains an induced claw (K_{1,3}).
inline bool has_induced_claw(const Graph& g) {
    for (int c = 1; c <= g.n; ++c) {
        if (g.adj_set[c].size() < 3) continue;

        std::vector<int> nbrs;
        nbrs.reserve(g.adj_set[c].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[c].begin();
             it != g.adj_set[c].end(); ++it) {
            nbrs.push_back(*it);
        }

        for (size_t i = 0; i < nbrs.size(); ++i) {
            for (size_t j = i + 1; j < nbrs.size(); ++j) {
                if (g.has_edge(nbrs[i], nbrs[j])) continue;
                for (size_t k = j + 1; k < nbrs.size(); ++k) {
                    if (g.has_edge(nbrs[i], nbrs[k])) continue;
                    if (g.has_edge(nbrs[j], nbrs[k])) continue;
                    return true;
                }
            }
        }
    }
    return false;
}

} // namespace detail

// Check whether a graph is a proper interval graph.
// Characterization used:
//   G is proper interval iff G is interval and claw-free.
inline ProperIntervalResult check_proper_interval(const Graph& g) {
    ProperIntervalResult res;
    res.is_proper_interval = false;

    IntervalResult interval = check_interval(g);
    if (!interval.is_interval) return res;

    if (detail::has_induced_claw(g)) return res;

    res.is_proper_interval = true;
    return res;
}

} // namespace graph_recognition

#endif
