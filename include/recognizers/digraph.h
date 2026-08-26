#ifndef GRAPH_RECOGNITION_DIGRAPH_H
#define GRAPH_RECOGNITION_DIGRAPH_H

/**
 * @file digraph.h
 * @brief Directed graph (digraph) recognition
 *
 * Validates a simple directed graph: no self-loops, no duplicate arcs in the same direction.
 * Always YES if the input is a valid directed graph.
 *
 * Input format: n m (number of vertices, number of arcs) followed by m lines of arcs u v (u->v)
 */

#include <iostream>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for digraph recognition
 */
enum class DigraphAlgorithm {
    VALIDITY_CHECK /**< Input validity check */
};

/**
 * @brief Result of digraph recognition
 */
struct DigraphResult {
    bool is_digraph = false; /**< true if the input is a valid digraph */
};

/**
 * @brief Determines whether the input is a valid directed graph
 * @param n Number of vertices
 * @param arcs Arc list (u, v) = u->v
 * @param algo Algorithm to use
 * @return DigraphResult
 *
 * Digraph iff no self-loops and no duplicate arcs in the same direction.
 */
inline DigraphResult check_digraph(int n,
    const std::vector<std::pair<int, int>>& arcs,
    DigraphAlgorithm algo = DigraphAlgorithm::VALIDITY_CHECK) {
    (void)algo;
    DigraphResult res;

    if (n < 0) return res;
    if (n == 0) {
        res.is_digraph = arcs.empty();
        return res;
    }

    std::set<std::pair<int, int>> seen;
    for (size_t i = 0; i < arcs.size(); ++i) {
        int u = arcs[i].first, v = arcs[i].second;
        if (u < 1 || u > n || v < 1 || v > n) return res;
        if (u == v) return res; /* self-loop */
        if (!seen.insert(std::make_pair(u, v)).second) return res; /* duplicate arc */
    }

    res.is_digraph = true;
    return res;
}

} // namespace graph_recognition

#endif
