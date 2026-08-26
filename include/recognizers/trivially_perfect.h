#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H

/**
 * @file trivially_perfect.h
 * @brief Trivially perfect graph recognition
 *
 * Determines a graph is trivially perfect if it is both chordal and a cograph.
 */

#include "recognizers/chordal.h"
#include "recognizers/cograph.h"
#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "certificates/obstruction_extract.h"

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
    Obstruction obstruction; /**< NO certificate: a C4 or a P4, the two patterns
                                  trivially perfect graphs forbid. Valid only when
                                  is_trivially_perfect == false */
};

/**
 * @brief Determines whether the graph is trivially perfect
 * @param g Input graph
 * @param algo Algorithm selector (currently only DFS is implemented)
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
    if (!chordal.is_chordal) {
        // A raw hole is not a trivially perfect obstruction; project it onto
        // the C4 or the P4 it contains.
        res.obstruction =
            detail_obstruction::tp_obstruction_from_hole(chordal.obstruction.vertices);
        return res;
    }

    CographResult cograph = check_cograph(g);
    if (!cograph.is_cograph) {
        res.obstruction = cograph.obstruction;
        return res;
    }

    res.is_trivially_perfect = true;
    return res;
}

} // namespace graph_recognition

#endif
