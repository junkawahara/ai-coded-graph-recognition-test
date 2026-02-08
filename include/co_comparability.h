#ifndef GRAPH_RECOGNITION_CO_COMPARABILITY_H
#define GRAPH_RECOGNITION_CO_COMPARABILITY_H

#include "comparability.h"
#include "graph.h"
#include "permutation.h"
#include <vector>

namespace graph_recognition {

// Result of co-comparability graph recognition.
struct CoComparabilityResult {
    bool is_co_comparability;
};

// Check whether a graph is a co-comparability graph.
// Characterization used:
//   G is co-comparability iff complement(G) is comparability.
inline CoComparabilityResult check_co_comparability(const Graph& g) {
    CoComparabilityResult res;
    res.is_co_comparability = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = detail::build_complement_matrix(a);
    if (!detail::is_comparability_graph(c)) return res;

    res.is_co_comparability = true;
    return res;
}

} // namespace graph_recognition

#endif
