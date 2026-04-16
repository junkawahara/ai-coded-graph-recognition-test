#ifndef GRAPH_RECOGNITION_THRESHOLD_H
#define GRAPH_RECOGNITION_THRESHOLD_H

/**
 * @file threshold.h
 * @brief Threshold graph recognition
 *
 * Algorithm:
 *   - DEGREE_SEQUENCE: Iterative removal of isolated/dominating vertices
 *   - DEGREE_SEQUENCE_FAST: Degree sequence sort + two-pointer technique (default)
 */

#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for threshold graph recognition
 */
enum class ThresholdAlgorithm {
    DEGREE_SEQUENCE,      /**< Iterative removal */
    DEGREE_SEQUENCE_FAST  /**< Degree sequence sort + two-pointer technique (default) */
};

/**
 * @brief Result of threshold graph recognition
 */
struct ThresholdResult {
    bool is_threshold = false; /**< true if the graph is a threshold graph */
};

namespace detail {

/**
 * @brief Threshold graph recognition via iterative removal (original algorithm)
 */
inline ThresholdResult check_threshold_elimination(const Graph& g) {
    ThresholdResult res;
    res.is_threshold = true;

    int n = g.n;
    std::vector<std::vector<int>> neighbors(n + 1);
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);

    for (int v = 1; v <= n; ++v) {
        neighbors[v].reserve(g.adj_set[v].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[v].begin();
             it != g.adj_set[v].end(); ++it) {
            neighbors[v].push_back(*it);
        }
        degree[v] = (int)neighbors[v].size();
    }

    int alive_count = n;
    for (int step = 0; step < n; ++step) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] == 0 || degree[v] == alive_count - 1) {
                pick = v;
                break;
            }
        }
        if (pick == 0) {
            res.is_threshold = false;
            return res;
        }

        alive[pick] = 0;
        alive_count--;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief Threshold graph recognition via degree sequence sort + two-pointer technique
 *
 * Threshold graphs are uniquely determined by their degree sequence (unigraph).
 * Sort degree sequence in descending order and simulate removal of
 * isolated/dominating vertices from both ends in O(n) using lazy offset.
 */
inline ThresholdResult check_threshold_fast(const Graph& g) {
    ThresholdResult res;
    res.is_threshold = true;

    int n = g.n;
    if (n <= 1) return res;

    // Compute degrees
    std::vector<int> deg(n);
    for (int v = 1; v <= n; ++v) {
        deg[v - 1] = (int)g.adj[v].size();
    }

    // Counting sort (descending)
    std::vector<int> count(n, 0);
    for (int i = 0; i < n; ++i) count[deg[i]]++;
    std::vector<int> d(n);
    // Descending: place from highest degree
    int pos = 0;
    for (int k = n - 1; k >= 0; --k) {
        for (int c = 0; c < count[k]; ++c) {
            d[pos++] = k;
        }
    }

    // Two-pointer + lazy offset
    int lo = 0, hi = n - 1;
    int remaining = n;
    int offset = 0;

    while (lo <= hi) {
        int actual_hi = d[hi] - offset;
        int actual_lo = d[lo] - offset;

        if (actual_hi == 0) {
            // Remove isolated vertex
            hi--;
            remaining--;
        } else if (actual_lo == remaining - 1) {
            // Remove dominating vertex
            lo++;
            remaining--;
            offset++;
        } else {
            res.is_threshold = false;
            return res;
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a threshold graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DEGREE_SEQUENCE_FAST)
 * @return ThresholdResult
 */
inline ThresholdResult check_threshold(const Graph& g,
    ThresholdAlgorithm algo = ThresholdAlgorithm::DEGREE_SEQUENCE_FAST) {
    switch (algo) {
        case ThresholdAlgorithm::DEGREE_SEQUENCE:
            return detail::check_threshold_elimination(g);
        case ThresholdAlgorithm::DEGREE_SEQUENCE_FAST:
            return detail::check_threshold_fast(g);
        default:
            break;
    }
    return ThresholdResult();
}

} // namespace graph_recognition

#endif
