#ifndef GRAPH_RECOGNITION_COMPARABILITY_H
#define GRAPH_RECOGNITION_COMPARABILITY_H

#include "graph.h"
#include "permutation.h"
#include <vector>

namespace graph_recognition {

// Result of comparability graph recognition.
struct ComparabilityResult {
    bool is_comparability;
};

// Check whether a graph is a comparability graph.
// This reuses the transitive-orientation solver used by permutation recognition.
inline ComparabilityResult check_comparability(const Graph& g) {
    ComparabilityResult res;
    res.is_comparability = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    if (!detail::is_comparability_graph(a)) return res;

    res.is_comparability = true;
    return res;
}

} // namespace graph_recognition

#endif
