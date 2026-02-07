#ifndef GRAPH_RECOGNITION_PTOLEMAIC_H
#define GRAPH_RECOGNITION_PTOLEMAIC_H

#include "chordal.h"
#include "distance_hereditary.h"
#include "graph.h"

namespace graph_recognition {

// Result of ptolemaic graph recognition.
struct PtolemaicResult {
    bool is_ptolemaic;
};

// Check whether a graph is ptolemaic.
// Characterization used:
//   G is ptolemaic iff G is chordal and distance-hereditary.
inline PtolemaicResult check_ptolemaic(const Graph& g) {
    PtolemaicResult res;
    res.is_ptolemaic = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    DistanceHereditaryResult dh = check_distance_hereditary(g);
    if (!dh.is_distance_hereditary) return res;

    res.is_ptolemaic = true;
    return res;
}

} // namespace graph_recognition

#endif
