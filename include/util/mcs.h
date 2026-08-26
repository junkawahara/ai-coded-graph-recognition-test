#ifndef GRAPH_RECOGNITION_MCS_H
#define GRAPH_RECOGNITION_MCS_H

/**
 * @file mcs.h
 * @brief Maximum Cardinality Search (MCS)
 *
 * Algorithms:
 *   - PQ_MCS: priority queue based MCS O(n+m log n)
 *   - BUCKET_MCS: bucket sort based MCS O(n+m) (default)
 */

#include "util/graph.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for MCS
 */
enum class MCSAlgorithm {
    PQ_MCS,     /**< Priority queue based O(n+m log n) */
    BUCKET_MCS  /**< Bucket sort based O(n+m) (default) */
};

/**
 * @brief Result of MCS
 */
struct MCSResult {
    std::vector<int> order;   /**< order[i] = vertex at position i (1-indexed) */
    std::vector<int> number;  /**< number[v] = position of vertex v (1-indexed) */
};

namespace detail {

/**
 * @brief Priority queue based MCS (original algorithm)
 */
inline MCSResult mcs_pq(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    std::vector<int> label(n + 1, 0), used(n + 1, 0);
    std::priority_queue<std::pair<int, int>> pq;
    for (int v = 1; v <= n; ++v) pq.push(std::make_pair(0, v));

    for (int i = n; i >= 1; --i) {
        while (!pq.empty()) {
            int v = pq.top().second;
            int l = pq.top().first;
            if (used[v] || l != label[v]) { pq.pop(); continue; }
            pq.pop();
            used[v] = 1;
            res.order[i] = v;
            res.number[v] = i;
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!used[u]) {
                    label[u]++;
                    pq.push(std::make_pair(label[u], u));
                }
            }
            break;
        }
    }

    return res;
}

/**
 * @brief Bucket sort based MCS O(n+m)
 *
 * Manages vertices of each label value in a doubly-linked list using bucket arrays.
 * Label update is O(1) via inter-bucket move.
 */
inline MCSResult mcs_bucket(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    std::vector<int> key(n + 1, 0);        // key[v] = number of labeled neighbors
    std::vector<unsigned char> used(n + 1, 0);

    // Doubly-linked list: prev[v], next[v]
    // bucket_head[k] = head of the list for label k (0 if empty)
    std::vector<int> prev(n + 1, 0), next(n + 1, 0);
    std::vector<int> bucket_head(n + 1, 0); // bucket_head[0..n-1]

    // Insert all vertices into bucket 0
    bucket_head[0] = 1;
    prev[1] = 0;
    for (int v = 1; v <= n; ++v) {
        next[v] = (v < n) ? v + 1 : 0;
        prev[v] = (v > 1) ? v - 1 : 0;
    }

    int max_key = 0;

    for (int i = n; i >= 1; --i) {
        // Extract a vertex from the max_key bucket
        while (max_key >= 0 && bucket_head[max_key] == 0) max_key--;
        if (max_key < 0) break;
        int v = bucket_head[max_key];

        // Remove v from the list
        bucket_head[max_key] = next[v];
        if (next[v] != 0) prev[next[v]] = 0;

        used[v] = 1;
        res.order[i] = v;
        res.number[v] = i;

        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (used[u]) continue;

            int old_key = key[u];

            // Remove u from bucket old_key
            if (prev[u] != 0) {
                next[prev[u]] = next[u];
            } else {
                bucket_head[old_key] = next[u];
            }
            if (next[u] != 0) {
                prev[next[u]] = prev[u];
            }

            key[u] = old_key + 1;

            // Insert u into bucket old_key+1 (at head)
            next[u] = bucket_head[key[u]];
            prev[u] = 0;
            if (bucket_head[key[u]] != 0) {
                prev[bucket_head[key[u]]] = u;
            }
            bucket_head[key[u]] = u;

            if (key[u] > max_key) max_key = key[u];
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief Computes the MCS ordering of a graph
 * @param g Input graph
 * @param algo Algorithm to use (default: BUCKET_MCS)
 * @return MCSResult (order and number)
 *
 * For chordal graphs, the resulting order[1..n] is a perfect elimination ordering (PEO).
 * order[1] is eliminated first and order[n] is eliminated last.
 */
inline MCSResult mcs(const Graph& g,
    MCSAlgorithm algo = MCSAlgorithm::BUCKET_MCS) {
    switch (algo) {
        case MCSAlgorithm::PQ_MCS:
            return detail::mcs_pq(g);
        case MCSAlgorithm::BUCKET_MCS:
            return detail::mcs_bucket(g);
        default:
            break;
    }
    return MCSResult();
}

} // namespace graph_recognition

#endif
