#ifndef GRAPH_RECOGNITION_CO_INTERVAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CO_INTERVAL_UNLABELED_ENUM_H

/**
 * @file co_interval_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic co-interval graphs
 *
 * Enumerates one representative per isomorphism class of co-interval graphs
 * (complements of interval graphs) on n vertices by enumerating non-isomorphic
 * interval graphs and complementing each. Complementation commutes with vertex
 * relabeling, so it is a bijection on isomorphism classes: distinct interval
 * classes map to distinct co-interval classes, and every co-interval class is
 * hit (the same construction as `cochain_unlabeled_enum.h` and
 * `co_chordal_unlabeled_enum.h`).
 *
 * Consequently the number of non-isomorphic co-interval graphs on n vertices
 * equals the interval count, OEIS A005975(n): 1, 2, 4, 10, 27, 92, 369,
 * 1807, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (the canonical construction path behind the interval enumeration);
 *   OEIS A005975
 */

#include "enumerators/interval_unlabeled_enum.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief An enumerated co-interval graph
 */
struct CoIntervalUnlabeledEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic co-interval enumeration
 */
struct CoIntervalUnlabeledEnumerationResult {
    std::vector<CoIntervalUnlabeledEnumeratedGraph> graphs;  /**< array of enumerated co-interval graphs */
};

/**
 * @brief Enumerates all non-isomorphic co-interval graphs on n vertices
 * @param n Number of vertices
 * @return CoIntervalUnlabeledEnumerationResult
 *
 * Enumerates non-isomorphic interval graphs and outputs the complement of
 * each. n = 0 yields the single empty graph.
 */
inline CoIntervalUnlabeledEnumerationResult enumerate_co_interval_unlabeled_graphs(int n) {
    CoIntervalUnlabeledEnumerationResult result;
    if (n < 0) return result;

    IntervalUnlabeledEnumerationResult intervals = enumerate_interval_unlabeled_graphs(n);

    // Build the complement of each interval graph
    for (std::size_t i = 0; i < intervals.graphs.size(); ++i) {
        const IntervalUnlabeledEnumeratedGraph& ig = intervals.graphs[i];

        std::vector<std::vector<bool> > has_edge(n + 1, std::vector<bool>(n + 1, false));
        for (std::size_t e = 0; e < ig.edges.size(); ++e) {
            int u = ig.edges[e].first;
            int v = ig.edges[e].second;
            has_edge[u][v] = true;
            has_edge[v][u] = true;
        }

        CoIntervalUnlabeledEnumeratedGraph g;
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
