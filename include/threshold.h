#ifndef GRAPH_RECOGNITION_THRESHOLD_H
#define GRAPH_RECOGNITION_THRESHOLD_H

/**
 * @file threshold.h
 * @brief Threshold graph recognition
 *
 * Algorithm:
 *   - DEGREE_SEQUENCE: Iterative removal of isolated/dominating vertices
 *   - DEGREE_SEQUENCE_FAST: Degree sequence sort + two-pointer technique (default)
 *
 * Both variants also report the creation sequence: a threshold graph is built
 * from one vertex by repeatedly adding an isolated or a dominating vertex, and
 * reversing the eliminations the recognizers perform gives exactly that. The
 * sequence is replayed against the graph before being returned, so it doubles
 * as the certificate.
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
    /**
     * @brief creation_order[i] = the i-th vertex added, for i in [1, n] (size n+1)
     *
     * Valid only when is_threshold == true.
     */
    std::vector<int> creation_order;
    /**
     * @brief creation_kind[i] = 0 if creation_order[i] was added as an isolated
     *        vertex, 1 if as a dominating vertex (size n+1)
     *
     * The first vertex is recorded as isolated. Valid only when
     * is_threshold == true.
     */
    std::vector<int> creation_kind;
};

namespace detail {

/**
 * @brief Replays a creation sequence against the graph
 * @param g Input graph
 * @param order order[i] = the i-th vertex added, for i in [1, n]
 * @param kind kind[i] = 0 for an isolated addition, 1 for a dominating one
 * @return true if every step is consistent with g
 *
 * Runs in O(n + m): each vertex's neighbours are counted once against the
 * positions of the vertices added before it.
 */
inline bool creation_sequence_is_valid(const Graph& g, const std::vector<int>& order,
                                       const std::vector<int>& kind) {
    int n = g.n;
    if ((int)order.size() != n + 1 || (int)kind.size() != n + 1) return false;
    std::vector<int> pos(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        if (v < 1 || v > n || pos[v] != 0) return false;
        pos[v] = i;
    }
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        int earlier = 0;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            if (pos[g.adj[v][j]] < i) ++earlier;
        }
        if (kind[i] == 0) {
            if (earlier != 0) return false;
        } else if (kind[i] == 1) {
            if (earlier != i - 1) return false;
        } else {
            return false;
        }
    }
    return true;
}

/**
 * @brief Turns an elimination order into the creation sequence
 *
 * The vertex removed last is added first, and a vertex isolated (or
 * dominating) at removal time is isolated (or dominating) when added back to
 * exactly the vertices that outlived it.
 */
inline void creation_sequence_from_elimination(const Graph& g,
                                               const std::vector<int>& removed,
                                               const std::vector<int>& removed_kind,
                                               ThresholdResult& res) {
    int n = g.n;
    res.creation_order.assign(n + 1, 0);
    res.creation_kind.assign(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        res.creation_order[n - i] = removed[i];
        res.creation_kind[n - i] = removed_kind[i];
    }
    if (!creation_sequence_is_valid(g, res.creation_order, res.creation_kind)) {
        res.creation_order.clear();
        res.creation_kind.clear();
        return;
    }
    res.is_threshold = true;
}

/**
 * @brief Threshold graph recognition via iterative removal (original algorithm)
 */
inline ThresholdResult check_threshold_elimination(const Graph& g) {
    ThresholdResult res;

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

    std::vector<int> removed, removed_kind;
    removed.reserve(n);
    removed_kind.reserve(n);

    int alive_count = n;
    for (int step = 0; step < n; ++step) {
        int pick = 0, kind = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] == 0) {
                pick = v;
                kind = 0;
                break;
            }
            if (degree[v] == alive_count - 1) {
                pick = v;
                kind = 1;
                break;
            }
        }
        if (pick == 0) return res;

        removed.push_back(pick);
        removed_kind.push_back(kind);
        alive[pick] = 0;
        alive_count--;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    creation_sequence_from_elimination(g, removed, removed_kind, res);
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

    int n = g.n;
    if (n == 0) {
        res.creation_order.assign(1, 0);
        res.creation_kind.assign(1, 0);
        res.is_threshold = true;
        return res;
    }

    // Counting sort of the vertices by decreasing degree; `order` keeps the
    // vertex behind each entry so the eliminations can be recorded.
    std::vector<int> count(n, 0);
    for (int v = 1; v <= n; ++v) count[(int)g.adj[v].size()]++;
    std::vector<int> start(n + 1, 0);
    int pos = 0;
    for (int k = n - 1; k >= 0; --k) {
        start[k] = pos;
        pos += count[k];
    }
    std::vector<int> order(n, 0), d(n, 0);
    for (int v = 1; v <= n; ++v) {
        int slot = start[(int)g.adj[v].size()]++;
        order[slot] = v;
        d[slot] = (int)g.adj[v].size();
    }

    // Two-pointer + lazy offset
    std::vector<int> removed, removed_kind;
    removed.reserve(n);
    removed_kind.reserve(n);
    int lo = 0, hi = n - 1;
    int remaining = n;
    int offset = 0;

    while (lo <= hi) {
        int actual_hi = d[hi] - offset;
        int actual_lo = d[lo] - offset;

        if (actual_hi == 0) {
            // Remove isolated vertex
            removed.push_back(order[hi]);
            removed_kind.push_back(0);
            hi--;
            remaining--;
        } else if (actual_lo == remaining - 1) {
            // Remove dominating vertex
            removed.push_back(order[lo]);
            removed_kind.push_back(1);
            lo++;
            remaining--;
            offset++;
        } else {
            return res;
        }
    }

    creation_sequence_from_elimination(g, removed, removed_kind, res);
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
