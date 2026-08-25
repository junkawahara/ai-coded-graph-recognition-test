#ifndef GRAPH_RECOGNITION_CO_CHORDAL_H
#define GRAPH_RECOGNITION_CO_CHORDAL_H

/**
 * @file co_chordal.h
 * @brief Co-chordal graph recognition
 *
 * Determines the graph is co-chordal if the complement graph is chordal.
 */

#include "chordal.h"
#include "graph.h"
#include "graph_utils.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for co-chordal graph recognition
 */
enum class CoChordalAlgorithm {
    COMPLEMENT /**< chordality test on the complement graph */
};

/**
 * @brief Result of co-chordal graph recognition
 */
struct CoChordalResult {
    bool is_co_chordal = false; /**< true if the graph is co-chordal */
};

/**
 * @brief Determines whether a graph is a co-chordal graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only COMPLEMENT is implemented)
 * @return CoChordalResult
 *
 * G is co-chordal iff complement(G) is a chordal graph.
 */
inline CoChordalResult check_co_chordal(const Graph& g,
    CoChordalAlgorithm algo = CoChordalAlgorithm::COMPLEMENT) {
    (void)algo;
    CoChordalResult res;
    res.is_co_chordal = false;

    Graph gc = build_complement(g);
    ChordalResult cres = check_chordal(gc);
    if (!cres.is_chordal) return res;

    res.is_co_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
