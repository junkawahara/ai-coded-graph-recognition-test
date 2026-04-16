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
 *   via BFS + DFS on a restricted graph.
 *
 * References:
 *   - Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020
 */

#include "graph.h"
#include "perfect.h"

namespace graph_recognition {

/**
 * @brief Result of odd-hole-free graph recognition
 */
struct OddHoleFreeResult {
    bool is_odd_hole_free = false; /**< true if the graph is odd-hole-free */
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
    res.is_odd_hole_free = !detail_perfect::has_odd_hole(g);
    return res;
}

} // namespace graph_recognition

#endif
