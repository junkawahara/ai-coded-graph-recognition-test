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

namespace detail_co_chordal {

/** @brief Constructs the complement graph (internal function) */
inline Graph build_complement_graph(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

} // namespace detail_co_chordal

/**
 * @brief Determines whether a graph is a co-chordal graph
 * @param g Input graph
 * @return CoChordalResult
 *
 * G is co-chordal iff complement(G) is a chordal graph.
 */
inline CoChordalResult check_co_chordal(const Graph& g,
    CoChordalAlgorithm algo = CoChordalAlgorithm::COMPLEMENT) {
    (void)algo;
    CoChordalResult res;
    res.is_co_chordal = false;

    Graph gc = detail_co_chordal::build_complement_graph(g);
    ChordalResult cres = check_chordal(gc);
    if (!cres.is_chordal) return res;

    res.is_co_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
