#ifndef GRAPH_RECOGNITION_SERIES_PARALLEL_H
#define GRAPH_RECOGNITION_SERIES_PARALLEL_H

/**
 * @file series_parallel.h
 * @brief Series-parallel graph recognition
 *
 * Algorithm:
 *   - MINOR_CHECK: Iterative removal of vertices with degree <= 2 by full scan
 *   - QUEUE_REDUCTION: Queue-based 2-degeneracy test (default)
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for series-parallel graph recognition
 */
enum class SeriesParallelAlgorithm {
    MINOR_CHECK,    /**< Iterative removal by full scan */
    QUEUE_REDUCTION /**< Queue-based 2-degeneracy test (default) */
};

/**
 * @brief Result of series-parallel graph recognition
 */
struct SeriesParallelResult {
    bool is_series_parallel = false; /**< true if the graph is a series-parallel graph */
};

namespace detail {

/** @brief Iterative removal by full scan (original algorithm) */
inline SeriesParallelResult check_series_parallel_scan(const Graph& g) {
    SeriesParallelResult res;
    res.is_series_parallel = true;

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

    for (int step = 0; step < n; ++step) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] <= 2) {
                pick = v;
                break;
            }
        }

        if (pick == 0) {
            res.is_series_parallel = false;
            return res;
        }

        alive[pick] = 0;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief Queue-based 2-degeneracy test
 *
 * Manages vertices with degree <= 2 in a queue, updating neighbor degrees on removal.
 * If all vertices can be removed, the graph is 2-degenerate (= K4-minor-free = series-parallel).
 */
inline SeriesParallelResult check_series_parallel_queue(const Graph& g) {
    SeriesParallelResult res;
    res.is_series_parallel = true;

    int n = g.n;
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);

    for (int v = 1; v <= n; ++v) {
        degree[v] = (int)g.adj[v].size();
    }

    std::queue<int> q;
    for (int v = 1; v <= n; ++v) {
        if (degree[v] <= 2) q.push(v);
    }

    int removed = 0;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (!alive[v]) continue;
        if (degree[v] > 2) continue;

        alive[v] = 0;
        removed++;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (alive[u]) {
                degree[u]--;
                if (degree[u] <= 2) q.push(u);
            }
        }
    }

    if (removed != n) {
        res.is_series_parallel = false;
    }

    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a series-parallel graph
 * @param g Input graph
 * @param algo Algorithm to use (default: QUEUE_REDUCTION)
 * @return SeriesParallelResult
 */
inline SeriesParallelResult check_series_parallel(const Graph& g,
    SeriesParallelAlgorithm algo = SeriesParallelAlgorithm::QUEUE_REDUCTION) {
    switch (algo) {
        case SeriesParallelAlgorithm::MINOR_CHECK:
            return detail::check_series_parallel_scan(g);
        case SeriesParallelAlgorithm::QUEUE_REDUCTION:
            return detail::check_series_parallel_queue(g);
        default:
            break;
    }
    return SeriesParallelResult();
}

} // namespace graph_recognition

#endif
