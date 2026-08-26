#ifndef GRAPH_RECOGNITION_COCHAIN_ENUM_H
#define GRAPH_RECOGNITION_COCHAIN_ENUM_H

/**
 * @file cochain_unlabeled_enum.h
 * @brief Cochain graph (complement of chain graph) enumeration
 *
 * Enumerates all non-isomorphic cochain graphs by enumerating chain graphs
 * and computing the complement of each.
 * Since the complement operation is a bijection on isomorphism classes,
 * the number of non-isomorphic types is the same as for chain graphs (OEIS A005418).
 */

#include "chain_unlabeled_enum.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Enumerated cochain graph
 */
struct CochainUnlabeledEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int>> edges;       /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of cochain graph enumeration
 */
struct CochainUnlabeledEnumerationResult {
    std::vector<CochainUnlabeledEnumeratedGraph> graphs;   /**< array of enumerated cochain graphs */
};

/**
 * @brief Enumerates all non-isomorphic cochain graphs on n vertices
 * @param n Number of vertices
 * @return CochainUnlabeledEnumerationResult
 *
 * Enumerates chain graphs and outputs the complement of each.
 */
inline CochainUnlabeledEnumerationResult enumerate_cochain_unlabeled_graphs(int n) {
    CochainUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_cochain accepts n <= 1; emit the empty graph for n = 0.
        CochainUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    ChainUnlabeledEnumerationResult chains = enumerate_chain_unlabeled_graphs(n);

    // Build the complement of each chain graph
    for (std::size_t i = 0; i < chains.graphs.size(); ++i) {
        const ChainUnlabeledEnumeratedGraph& cg = chains.graphs[i];

        // Manage edge set with bits (fast lookup)
        std::vector<std::vector<bool> > has_edge(n + 1, std::vector<bool>(n + 1, false));
        for (std::size_t e = 0; e < cg.edges.size(); ++e) {
            int u = cg.edges[e].first;
            int v = cg.edges[e].second;
            has_edge[u][v] = true;
            has_edge[v][u] = true;
        }

        CochainUnlabeledEnumeratedGraph g;
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
