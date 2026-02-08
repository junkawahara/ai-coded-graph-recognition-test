#ifndef GRAPH_RECOGNITION_AT_FREE_H
#define GRAPH_RECOGNITION_AT_FREE_H

#include "graph.h"
#include "interval_v2.h"

namespace graph_recognition {

// Result of AT-free graph recognition.
struct ATFreeResult {
    bool is_at_free;
};

// Check whether a graph is AT-free (asteroidal-triple free).
// Reuses the AT detector from interval_v2 recognition.
inline ATFreeResult check_at_free(const Graph& g) {
    ATFreeResult res;
    res.is_at_free = !detail_v2::has_asteroidal_triple(g);
    return res;
}

} // namespace graph_recognition

#endif
