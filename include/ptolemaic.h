#ifndef GRAPH_RECOGNITION_PTOLEMAIC_H
#define GRAPH_RECOGNITION_PTOLEMAIC_H

/**
 * @file ptolemaic.h
 * @brief Ptolemaic graph recognition
 *
 * Recognizes graphs satisfying both chordality and distance-hereditary property.
 *
 * Algorithm:
 *   - DH_HASHMAP: Chordality + hashmap DH check
 *   - DH_SORTED: Chordality + sorted DH check (default)
 */

#include "chordal.h"
#include "forbidden_subgraph.h"
#include "obstruction_extract.h"
#include "distance_hereditary.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Ptolemaic graph recognition
 */
enum class PtolemaicAlgorithm {
    DH_HASHMAP, /**< Chordality + hashmap DH check */
    DH_SORTED   /**< Chordality + sorted DH check (default) */
};

/**
 * @brief Result of Ptolemaic graph recognition
 */
struct PtolemaicResult {
    bool is_ptolemaic = false; /**< true if the graph is a Ptolemaic graph */
    Obstruction obstruction; /**< NO certificate: a HOLE or a GEM. Ptolemaic graphs
                                  are the chordal gem-free graphs (Howorka 1981), the
                                  house, domino and long holes of the
                                  distance-hereditary obstruction set all being
                                  non-chordal. Valid only when is_ptolemaic == false */
};

/**
 * @brief Determines whether the graph is a Ptolemaic graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DH_SORTED)
 * @return PtolemaicResult
 *
 * G is a Ptolemaic graph <=> G is a chordal graph and a distance-hereditary graph.
 */
inline PtolemaicResult check_ptolemaic(const Graph& g,
    PtolemaicAlgorithm algo = PtolemaicAlgorithm::DH_SORTED) {
    PtolemaicResult res;
    res.is_ptolemaic = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) {
        res.obstruction = chordal.obstruction;
        return res;
    }

    DistanceHereditaryAlgorithm dh_algo;
    switch (algo) {
        case PtolemaicAlgorithm::DH_HASHMAP:
            dh_algo = DistanceHereditaryAlgorithm::HASHMAP_TWINS;
            break;
        case PtolemaicAlgorithm::DH_SORTED:
            dh_algo = DistanceHereditaryAlgorithm::SORTED_TWINS;
            break;
        default:
            dh_algo = DistanceHereditaryAlgorithm::SORTED_TWINS;
            break;
    }

    DistanceHereditaryResult dh = check_distance_hereditary(g, dh_algo);
    if (!dh.is_distance_hereditary) {
        // The graph is chordal here, so the only distance-hereditary
        // obstruction left is the gem; the twin elimination does not name it,
        // hence the separate search on this path.
        res.obstruction = make_obstruction(ObstructionKind::GEM,
                                           detail_obstruction::find_gem(g));
        return res;
    }

    res.is_ptolemaic = true;
    return res;
}

} // namespace graph_recognition

#endif
