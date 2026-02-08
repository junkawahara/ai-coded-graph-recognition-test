#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H

#include "chordal.h"
#include "cograph.h"
#include "graph.h"

namespace graph_recognition {

// Result of trivially perfect graph recognition.
struct TriviallyPerfectResult {
    bool is_trivially_perfect;
};

// Check whether a graph is trivially perfect.
// Characterization used:
//   G is trivially perfect iff G is chordal and cograph.
inline TriviallyPerfectResult check_trivially_perfect(const Graph& g) {
    TriviallyPerfectResult res;
    res.is_trivially_perfect = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    CographResult cograph = check_cograph(g);
    if (!cograph.is_cograph) return res;

    res.is_trivially_perfect = true;
    return res;
}

} // namespace graph_recognition

#endif
