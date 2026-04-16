#ifndef GRAPH_RECOGNITION_STRONGLY_REGULAR_H
#define GRAPH_RECOGNITION_STRONGLY_REGULAR_H

/**
 * @file strongly_regular.h
 * @brief Strongly regular graph recognition
 *
 * Verifies that a regular graph has a constant number of common neighbors lambda
 * for adjacent vertex pairs and a constant number mu for non-adjacent vertex pairs.
 * Trivial cases (complete graph, empty graph) are excluded.
 */

#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for strongly regular graph recognition
 */
enum class StronglyRegularAlgorithm {
    PARAMETER_CHECK /**< Parameter verification */
};

/**
 * @brief Result of strongly regular graph recognition
 */
struct StronglyRegularResult {
    bool is_strongly_regular = false; /**< true if the graph is a strongly regular graph */
    int k = -1;      /**< Regular degree */
    int lambda = -1;  /**< Number of common neighbors for adjacent pairs */
    int mu = -1;      /**< Number of common neighbors for non-adjacent pairs */
};

/**
 * @brief Determines whether the graph is a strongly regular graph
 * @param g Input graph
 * @param algo Algorithm to use (default: PARAMETER_CHECK)
 * @return StronglyRegularResult
 *
 * Strongly regular graph srg(n, k, lambda, mu):
 *   - k-regular (0 < k < n-1)
 *   - Any adjacent vertex pair has exactly lambda common neighbors
 *   - Any non-adjacent vertex pair has exactly mu common neighbors
 */
inline StronglyRegularResult check_strongly_regular(const Graph& g,
    StronglyRegularAlgorithm algo = StronglyRegularAlgorithm::PARAMETER_CHECK) {
    (void)algo;
    StronglyRegularResult res;

    int n = g.n;
    if (n < 2) return res;

    /* k-regular check */
    int k = (int)g.adj[1].size();
    for (int v = 2; v <= n; ++v) {
        if ((int)g.adj[v].size() != k) return res;
    }

    /* Exclude trivial cases: complete graph (k=n-1) or empty graph (k=0) */
    if (k == 0 || k == n - 1) return res;

    /* Compute common neighbor count for all vertex pairs */
    int lambda_val = -1, mu_val = -1;

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            int common = 0;
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (g.adj_set[v].count(w)) ++common;
            }
            if (g.has_edge(u, v)) {
                /* Adjacent pair */
                if (lambda_val == -1) {
                    lambda_val = common;
                } else if (lambda_val != common) {
                    return res;
                }
            } else {
                /* Non-adjacent pair */
                if (mu_val == -1) {
                    mu_val = common;
                } else if (mu_val != common) {
                    return res;
                }
            }
        }
    }

    if (lambda_val == -1) lambda_val = 0;
    if (mu_val == -1) mu_val = 0;

    res.is_strongly_regular = true;
    res.k = k;
    res.lambda = lambda_val;
    res.mu = mu_val;
    return res;
}

} // namespace graph_recognition

#endif
