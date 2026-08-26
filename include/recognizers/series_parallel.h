#ifndef GRAPH_RECOGNITION_SERIES_PARALLEL_H
#define GRAPH_RECOGNITION_SERIES_PARALLEL_H

/**
 * @file series_parallel.h
 * @brief Series-parallel graph recognition
 *
 * A graph is series-parallel iff it is K4-minor-free. Equivalently, it can
 * be reduced to an edgeless graph by repeatedly applying:
 *   - pendant removal (remove a vertex of degree 0 or 1);
 *   - series reduction (remove a vertex of degree 2 and connect its two
 *     neighbors by a new edge, unless the edge already exists);
 *   - parallel reduction (when the series step would create a duplicate
 *     edge, it is simply omitted, matching the simple-graph collapse of
 *     two parallel edges into one).
 *
 * Note: iterative removal of degree-<=2 vertices *without* the series step
 * is only a 2-degeneracy test, which is weaker (e.g., it accepts a
 * subdivision of K4). This implementation performs the series/parallel
 * reduction.
 *
 * Algorithms:
 *   - MINOR_CHECK: full-scan series-parallel reduction (O(n^2) worst case)
 *   - QUEUE_REDUCTION: queue-based series-parallel reduction (default)
 *
 * Both variants also report the reduction sequence they used. Replaying it
 * removes every vertex, which is what being series-parallel means under this
 * characterization, so the sequence is the certificate.
 */

#include "util/graph.h"
#include <queue>
#include <unordered_set>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for series-parallel graph recognition
 */
enum class SeriesParallelAlgorithm {
    MINOR_CHECK,    /**< Full-scan series-parallel reduction */
    QUEUE_REDUCTION /**< Queue-based series-parallel reduction (default) */
};

/**
 * @brief Result of series-parallel graph recognition
 */
/**
 * @brief One step of a series-parallel reduction
 */
struct SPReduction {
    int vertex = 0; /**< the vertex removed */
    int kind = 0;   /**< 0 isolated, 1 pendant, 2 series (edge u-w added), 3 parallel (u-w existed) */
    int u = 0;      /**< first neighbour, 0 for an isolated vertex */
    int w = 0;      /**< second neighbour, 0 unless kind is 2 or 3 */
};

/**
 * @brief Result of series-parallel graph recognition
 */
struct SeriesParallelResult {
    bool is_series_parallel = false; /**< true if the graph is a series-parallel graph */
    /**
     * @brief The reduction steps, in the order they were applied
     *
     * Valid only when is_series_parallel == true, and then it has one entry
     * per vertex.
     */
    std::vector<SPReduction> reductions;
};

namespace detail {

/**
 * @brief Reduces one vertex of degree <= 2, updating the mutable adjacency.
 *
 * @param v        Vertex to reduce (must be alive with degree <= 2)
 * @param adj      Mutable adjacency sets
 * @param degree   Mutable degree array
 * @param alive    Mutable alive array
 * @param touched  Vertices whose degree dropped to <= 2 after the reduction
 *                 are appended here so callers can schedule them.
 * @param step     Records which reduction was applied.
 */
inline void sp_reduce_vertex(int v,
    std::vector<std::unordered_set<int>>& adj,
    std::vector<int>& degree,
    std::vector<unsigned char>& alive,
    std::vector<int>& touched,
    SPReduction& step) {
    alive[v] = 0;
    step = SPReduction();
    step.vertex = v;

    if (degree[v] == 0) {
        step.kind = 0;
        return;
    }
    if (degree[v] == 1) {
        int u = *adj[v].begin();
        step.kind = 1;
        step.u = u;
        adj[v].clear();
        adj[u].erase(v);
        degree[v] = 0;
        --degree[u];
        if (alive[u] && degree[u] <= 2) touched.push_back(u);
        return;
    }
    /* degree 2: series / parallel reduction */
    std::unordered_set<int>::const_iterator it = adj[v].begin();
    int u = *it;
    ++it;
    int w = *it;
    step.u = u;
    step.w = w;
    adj[v].clear();
    adj[u].erase(v);
    adj[w].erase(v);
    --degree[u];
    --degree[w];
    degree[v] = 0;
    if (adj[u].find(w) == adj[u].end()) {
        step.kind = 2; /* series: the path u-v-w becomes the edge u-w */
        adj[u].insert(w);
        adj[w].insert(u);
        ++degree[u];
        ++degree[w];
    } else {
        step.kind = 3; /* parallel: u-w already there, so the two collapse */
    }
    if (alive[u] && degree[u] <= 2) touched.push_back(u);
    if (alive[w] && degree[w] <= 2) touched.push_back(w);
}

/** @brief Full-scan series-parallel reduction */
inline SeriesParallelResult check_series_parallel_scan(const Graph& g) {
    SeriesParallelResult res;

    int n = g.n;
    if (n == 0) {
        res.is_series_parallel = true;
        return res;
    }

    std::vector<std::unordered_set<int>> adj(n + 1);
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);
    for (int v = 1; v <= n; ++v) {
        adj[v] = g.adj_set[v];
        degree[v] = (int)adj[v].size();
    }

    int removed = 0;
    std::vector<int> touched;
    while (true) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (alive[v] && degree[v] <= 2) {
                pick = v;
                break;
            }
        }
        if (pick == 0) break;
        touched.clear();
        SPReduction step;
        sp_reduce_vertex(pick, adj, degree, alive, touched, step);
        res.reductions.push_back(step);
        ++removed;
    }

    if (removed != n) {
        res.reductions.clear();
        return res;
    }
    res.is_series_parallel = true;
    return res;
}

/** @brief Queue-based series-parallel reduction */
inline SeriesParallelResult check_series_parallel_queue(const Graph& g) {
    SeriesParallelResult res;

    int n = g.n;
    if (n == 0) {
        res.is_series_parallel = true;
        return res;
    }

    std::vector<std::unordered_set<int>> adj(n + 1);
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);
    for (int v = 1; v <= n; ++v) {
        adj[v] = g.adj_set[v];
        degree[v] = (int)adj[v].size();
    }

    std::queue<int> q;
    for (int v = 1; v <= n; ++v) {
        if (degree[v] <= 2) q.push(v);
    }

    int removed = 0;
    std::vector<int> touched;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (!alive[v]) continue;
        if (degree[v] > 2) continue;

        touched.clear();
        SPReduction step;
        sp_reduce_vertex(v, adj, degree, alive, touched, step);
        res.reductions.push_back(step);
        ++removed;
        for (size_t i = 0; i < touched.size(); ++i) q.push(touched[i]);
    }

    if (removed != n) {
        res.reductions.clear();
        return res;
    }
    res.is_series_parallel = true;
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
