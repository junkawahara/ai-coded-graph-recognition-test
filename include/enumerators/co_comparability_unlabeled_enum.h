#ifndef GRAPH_RECOGNITION_CO_COMPARABILITY_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CO_COMPARABILITY_UNLABELED_ENUM_H

/**
 * @file co_comparability_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic co-comparability graphs
 *
 * Enumerates one representative per isomorphism class of co-comparability
 * graphs (complements of comparability graphs) on n vertices by
 * enumerating non-isomorphic comparability graphs and complementing each.
 * Complementation commutes with vertex relabeling, so it is a bijection on
 * isomorphism classes: distinct comparability classes map to distinct
 * co-comparability classes, and every co-comparability class is hit (the
 * same construction as `cochain_unlabeled_enum.h` and
 * `co_chordal_unlabeled_enum.h`).
 *
 * Consequently the number of non-isomorphic co-comparability graphs on n
 * vertices equals the comparability count, OEIS A123416(n): 1, 2, 4, 11,
 * 33, 144, 824, 6793, 75400, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (the canonical construction path behind the comparability enumeration);
 *   Möhring, "Almost all comparability graphs are UPO," Discrete Math. 50,
 *   1984;
 *   OEIS A123416
 */

#include "enumerators/comparability_unlabeled_enum.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief An enumerated co-comparability graph
 */
struct CoComparabilityUnlabeledEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic co-comparability enumeration
 */
struct CoComparabilityUnlabeledEnumerationResult {
    std::vector<CoComparabilityUnlabeledEnumeratedGraph> graphs;  /**< array of enumerated co-comparability graphs */
};

/**
 * @brief Enumerates all non-isomorphic co-comparability graphs on n vertices
 * @param n Number of vertices
 * @return CoComparabilityUnlabeledEnumerationResult
 *
 * Enumerates non-isomorphic comparability graphs and outputs the complement
 * of each. n = 0 yields the single empty graph.
 */
inline CoComparabilityUnlabeledEnumerationResult enumerate_co_comparability_unlabeled_graphs(int n) {
    CoComparabilityUnlabeledEnumerationResult result;
    if (n < 0) return result;

    ComparabilityUnlabeledEnumerationResult comparabilities =
        enumerate_comparability_unlabeled_graphs(n);

    // Build the complement of each comparability graph
    for (std::size_t i = 0; i < comparabilities.graphs.size(); ++i) {
        const ComparabilityUnlabeledEnumeratedGraph& cg = comparabilities.graphs[i];

        std::vector<std::vector<bool> > has_edge(n + 1, std::vector<bool>(n + 1, false));
        for (std::size_t e = 0; e < cg.edges.size(); ++e) {
            int u = cg.edges[e].first;
            int v = cg.edges[e].second;
            has_edge[u][v] = true;
            has_edge[v][u] = true;
        }

        CoComparabilityUnlabeledEnumeratedGraph g;
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
