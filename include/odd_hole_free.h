#ifndef GRAPH_RECOGNITION_ODD_HOLE_FREE_H
#define GRAPH_RECOGNITION_ODD_HOLE_FREE_H

/**
 * @file odd_hole_free.h
 * @brief Odd-hole-free graph recognition
 *
 * An odd-hole-free graph is a graph that does not contain an induced odd cycle
 * of length 5 or more (odd hole).
 *
 * perfect = odd-hole-free ∩ odd-antihole-free (Strong Perfect Graph Theorem)
 *
 * Algorithm:
 *   Uses has_odd_hole() from perfect.h. For each edge (u,v), detects odd holes
 *   via BFS + DFS on a restricted graph: BFS decides bipartiteness (a bipartite
 *   restriction has no even induced x-y path and is skipped), otherwise a
 *   backtracking DFS searches for an even-length induced path. The DFS is
 *   exponential in the worst case, though fast on typical inputs.
 *
 * Note: this is NOT the polynomial-time algorithm of Chudnovsky, Scott,
 * Seymour, Spirkl (JACM 67(1), 2020), which shows that odd-hole detection is
 * in P but is far more intricate than what is implemented here.
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "obstruction_extract.h"
#include "perfect.h"

namespace graph_recognition {

/**
 * @brief Result of odd-hole-free graph recognition
 */
struct OddHoleFreeResult {
    bool is_odd_hole_free = false; /**< true if the graph is odd-hole-free */
    Obstruction obstruction; /**< NO certificate: an ODD_HOLE. Valid only when
                                  is_odd_hole_free == false */
};

/**
 * @brief Determines whether the graph is odd-hole-free
 * @param g Input graph
 * @return OddHoleFreeResult
 */
inline OddHoleFreeResult check_odd_hole_free(const Graph& g) {
    OddHoleFreeResult res;
    if (g.n <= 4) {
        res.is_odd_hole_free = true;
        return res;
    }
    std::vector<int> hole = detail_perfect::find_odd_hole(g);
    res.is_odd_hole_free = hole.empty();
    if (!res.is_odd_hole_free) {
        res.obstruction =
            detail_obstruction::cycle_obstruction(hole, ObstructionKind::ODD_HOLE);
    }
    return res;
}

} // namespace graph_recognition

#endif
