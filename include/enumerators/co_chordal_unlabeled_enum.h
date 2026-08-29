#ifndef GRAPH_RECOGNITION_CO_CHORDAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CO_CHORDAL_UNLABELED_ENUM_H

/**
 * @file co_chordal_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic co-chordal graphs
 *
 * Enumerates one representative per isomorphism class of co-chordal graphs
 * (complements of chordal graphs) on n vertices by enumerating non-isomorphic
 * chordal graphs and complementing each. Complementation commutes with vertex
 * relabeling, so it is a bijection on isomorphism classes: distinct chordal
 * classes map to distinct co-chordal classes, and every co-chordal class is
 * hit (the same construction as `cochain_unlabeled_enum.h`).
 *
 * Consequently the number of non-isomorphic co-chordal graphs on n vertices
 * equals the chordal count, OEIS A048193(n): 1, 2, 4, 10, 27, 94, 393,
 * 2119, 14524, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (the canonical construction path behind the chordal enumeration);
 *   OEIS A048193
 */

#include "enumerators/chordal_unlabeled_enum.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief An enumerated co-chordal graph
 */
struct CoChordalUnlabeledEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic co-chordal enumeration
 */
struct CoChordalUnlabeledEnumerationResult {
    std::vector<CoChordalUnlabeledEnumeratedGraph> graphs;  /**< array of enumerated co-chordal graphs */
};

/**
 * @brief Enumerates all non-isomorphic co-chordal graphs on n vertices
 * @param n Number of vertices
 * @return CoChordalUnlabeledEnumerationResult
 *
 * Enumerates non-isomorphic chordal graphs and outputs the complement of
 * each. n = 0 yields the single empty graph.
 */
inline CoChordalUnlabeledEnumerationResult enumerate_co_chordal_unlabeled_graphs(int n) {
    CoChordalUnlabeledEnumerationResult result;
    if (n < 0) return result;

    ChordalUnlabeledEnumerationResult chordals = enumerate_chordal_unlabeled_graphs(n);

    // Build the complement of each chordal graph
    for (std::size_t i = 0; i < chordals.graphs.size(); ++i) {
        const ChordalUnlabeledEnumeratedGraph& cg = chordals.graphs[i];

        std::vector<std::vector<bool> > has_edge(n + 1, std::vector<bool>(n + 1, false));
        for (std::size_t e = 0; e < cg.edges.size(); ++e) {
            int u = cg.edges[e].first;
            int v = cg.edges[e].second;
            has_edge[u][v] = true;
            has_edge[v][u] = true;
        }

        CoChordalUnlabeledEnumeratedGraph g;
        g.n = n;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                if (!has_edge[u][v]) {
                    g.edges.push_back(std::make_pair(u, v));
                }
            }
        }
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
