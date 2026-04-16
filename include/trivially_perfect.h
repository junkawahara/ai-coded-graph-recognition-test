#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H

/**
 * @file trivially_perfect.h
 * @brief Trivially perfect graph recognition
 *
 * Determines a graph is trivially perfect if it is both chordal and a cograph.
 */

#include "chordal.h"
#include "cograph.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for trivially perfect graph recognition
 */
enum class TriviallyPerfectAlgorithm {
    DFS /**< Recognition via DFS */
};

/**
 * @brief Result of trivially perfect graph recognition
 */
struct TriviallyPerfectResult {
    bool is_trivially_perfect = false; /**< true if the graph is trivially perfect */
};

/**
 * @brief Determines whether the graph is trivially perfect
 * @param g Input graph
 * @return TriviallyPerfectResult
 *
 * G is trivially perfect <=> G is both chordal and a cograph.
 */
inline TriviallyPerfectResult check_trivially_perfect(const Graph& g,
    TriviallyPerfectAlgorithm algo = TriviallyPerfectAlgorithm::DFS) {
    (void)algo;
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
