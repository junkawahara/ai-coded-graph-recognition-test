#ifndef GRAPH_RECOGNITION_APOLLONIAN_H
#define GRAPH_RECOGNITION_APOLLONIAN_H

/**
 * @file apollonian.h
 * @brief Apollonian network (planar 3-tree) recognition
 *
 * An Apollonian network is built from a triangle by repeatedly picking a
 * (triangular) face and inserting a new vertex joined to its three
 * corners. Equivalent descriptions: planar 3-trees (n >= 4), stacked
 * triangulations / graphs of stacked polytopes, uniquely 4-colorable
 * planar graphs.
 *
 * Recognition is maximal planarity plus chordality: every Apollonian
 * network is maximal planar and chordal by induction over the insertion
 * sequence, and conversely a chordal maximal planar graph on n >= 5
 * vertices has a simplicial vertex, which must have degree exactly 3
 * (degree >= 4 with a complete neighborhood would put a K5 into a planar
 * graph); deleting it leaves a chordal maximal planar graph again
 * (m - 3 = 3(n - 1) - 6), so induction down to K4/K3 exhibits the
 * insertion sequence in reverse.
 */

#include "util/graph.h"
#include "recognizers/chordal.h"
#include "recognizers/maximal_planar.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Apollonian network recognition
 */
enum class ApollonianAlgorithm {
    MAXIMAL_PLANAR_CHORDAL /**< Maximal planarity + chordality check */
};

/**
 * @brief Result of Apollonian network recognition
 */
struct ApollonianResult {
    bool is_apollonian = false; /**< true if the graph is an Apollonian network */
};

/**
 * @brief Determines whether the graph is an Apollonian network (planar 3-tree)
 * @param g Input graph
 * @param algo Algorithm to use (default: MAXIMAL_PLANAR_CHORDAL)
 * @return ApollonianResult
 *
 * Apollonian network iff maximal planar and chordal. K3 is the seed
 * triangle of the recursive construction and is accepted; below that the
 * complete graphs K0, K1, K2 are accepted as the trivial members,
 * mirroring `check_maximal_planar` (a complete graph is chordal, so the
 * conjunction inherits exactly its small-n convention).
 */
inline ApollonianResult check_apollonian(const Graph& g,
    ApollonianAlgorithm algo = ApollonianAlgorithm::MAXIMAL_PLANAR_CHORDAL) {
    (void)algo;
    ApollonianResult res;

    /* Maximal planarity check (planar + m = 3n - 6; cheap edge-count
     * rejection happens inside) */
    if (!check_maximal_planar(g).is_maximal_planar) return res;

    /* Chordality check */
    if (!check_chordal(g).is_chordal) return res;

    res.is_apollonian = true;
    return res;
}

} // namespace graph_recognition

#endif
