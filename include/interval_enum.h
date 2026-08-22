#ifndef GRAPH_RECOGNITION_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_INTERVAL_ENUM_H

/**
 * @file interval_enum.h
 * @brief Enumeration of labeled interval graphs
 *
 * The default is the interval-graph-specific reverse search of Kiyomi,
 * Kijima, and Uno. Its root is K_n and a child is obtained by deleting one
 * edge. For every non-complete interval graph H, the parent adds an edge
 * incident with the largest-labeled non-universal vertex. An interval model
 * identifies a non-neighbor whose interval is closest to that vertex, so the
 * added edge preserves intervality.
 *
 * Reference: M. Kiyomi, S. Kijima, and T. Uno, "Listing Chordal Graphs and
 * Interval Graphs," WG 2006, LNCS 4271, 68--77, 2006.
 * DOI: 10.1007/11917496_7
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "interval.h"

namespace graph_recognition {

/** @brief Algorithm selection for interval enumeration */
enum class IntervalEnumAlgorithm {
    KIYOMI_KIJIMA_UNO = 0, /**< Interval-specific edge-deletion reverse search */
    REVERSE_SEARCH = KIYOMI_KIJIMA_UNO, /**< Backward-compatible name */
    LEGACY_CHORDAL_FILTER = 1 /**< Chordal vertex search plus interval filtering */
};

/** @brief Result of interval enumeration */
struct IntervalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of labeled interval graphs */
};

namespace detail {

/** @brief Complete-graph-rooted state used by the dedicated reverse search */
struct KiyomiKijimaUnoIntervalState {
    int total_n;
    std::size_t edge_count;
    std::vector<int> degree;
    std::vector<std::vector<char>> adj;

    explicit KiyomiKijimaUnoIntervalState(int n)
        : total_n(n),
          edge_count(static_cast<std::size_t>(n) *
                     static_cast<std::size_t>(n - 1) / 2),
          degree(n + 1, n - 1),
          adj(n + 1, std::vector<char>(n + 1, 0)) {
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                adj[u][v] = 1;
                adj[v][u] = 1;
            }
        }
    }
};

inline void interval_remove_edge(KiyomiKijimaUnoIntervalState* state,
                                 int u,
                                 int v) {
    state->adj[u][v] = 0;
    state->adj[v][u] = 0;
    --state->degree[u];
    --state->degree[v];
    --state->edge_count;
}

inline void interval_add_edge(KiyomiKijimaUnoIntervalState* state,
                              int u,
                              int v) {
    state->adj[u][v] = 1;
    state->adj[v][u] = 1;
    ++state->degree[u];
    ++state->degree[v];
    ++state->edge_count;
}

inline std::vector<std::pair<int, int>> collect_interval_edges(
    const KiyomiKijimaUnoIntervalState& state) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve(state.edge_count);
    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.adj[u][v]) edges.push_back(std::make_pair(u, v));
        }
    }
    return edges;
}

inline Graph interval_edge_state_to_graph(
    const KiyomiKijimaUnoIntervalState& state) {
    return Graph(state.total_n, collect_interval_edges(state));
}

/**
 * @brief Largest-labeled vertex that is not universal
 *
 * This is the deterministic "youngest" vertex used in the parent operation.
 * Zero denotes the complete graph.
 */
inline int interval_youngest_nonuniversal(
    const KiyomiKijimaUnoIntervalState& state) {
    for (int v = state.total_n; v >= 1; --v) {
        if (state.degree[v] < state.total_n - 1) return v;
    }
    return 0;
}

/**
 * @brief Edge added by the Kiyomi--Kijima--Uno parent operation
 *
 * The supplied interval model represents state. On the right of the pivot,
 * a non-neighbor with minimum left endpoint is closest; if the right side is
 * empty, the symmetric choice is a maximum right endpoint on the left. Ties
 * are broken by vertex label. Endpoints sharing a clique coordinate can be
 * infinitesimally perturbed in label order, so exactly the selected new
 * intersection is introduced.
 */
inline std::pair<int, int> interval_parent_edge_from_model(
    const KiyomiKijimaUnoIntervalState& state,
    const IntervalResult& model) {
    const int pivot = interval_youngest_nonuniversal(state);
    if (pivot == 0 || !model.is_interval ||
        model.intervals.size() <= static_cast<std::size_t>(state.total_n)) {
        return std::make_pair(0, 0);
    }

    int right = 0;
    int right_left_endpoint = state.total_n + 1;
    for (int u = 1; u <= state.total_n; ++u) {
        if (u == pivot || state.adj[pivot][u]) continue;
        const int left = model.intervals[u].first;
        if (left <= model.intervals[pivot].second) continue;
        if (right == 0 || left < right_left_endpoint ||
            (left == right_left_endpoint && u < right)) {
            right = u;
            right_left_endpoint = left;
        }
    }

    int other = right;
    if (other == 0) {
        int left = 0;
        int left_right_endpoint = -1;
        for (int u = 1; u <= state.total_n; ++u) {
            if (u == pivot || state.adj[pivot][u]) continue;
            const int endpoint = model.intervals[u].second;
            if (endpoint >= model.intervals[pivot].first) continue;
            if (left == 0 || endpoint > left_right_endpoint ||
                (endpoint == left_right_endpoint && u < left)) {
                left = u;
                left_right_endpoint = endpoint;
            }
        }
        other = left;
    }

    if (other == 0) return std::make_pair(0, 0);
    return pivot < other ? std::make_pair(pivot, other)
                         : std::make_pair(other, pivot);
}

template <typename Callback>
inline void kiyomi_kijima_uno_interval_dfs(
    KiyomiKijimaUnoIntervalState& state,
    Callback& cb) {
    // End the output object's lifetime before descending. Otherwise one full
    // edge list per recursion level would remain live on the call stack.
    {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_interval_edges(state);
        cb(graph);
    }

    const int pivot = interval_youngest_nonuniversal(state);
    if (pivot == 0) {
        // K_n is the root. Every K_n-e is interval, and e is its unique
        // missing edge, hence its parent edge.
        for (int u = 1; u <= state.total_n; ++u) {
            for (int v = u + 1; v <= state.total_n; ++v) {
                interval_remove_edge(&state, u, v);
                kiyomi_kijima_uno_interval_dfs(state, cb);
                interval_add_edge(&state, u, v);
            }
        }
        return;
    }

    // Every vertex after pivot is universal. They are true twins, so for a
    // fixed other endpoint all corresponding edge deletions are isomorphic.
    // Test one representative and reuse the answer for all labeled children.
    if (pivot < state.total_n) {
        const int representative = pivot + 1;
        for (int y = 1; y <= pivot; ++y) {
            interval_remove_edge(&state, representative, y);
            const bool is_interval =
                check_interval(interval_edge_state_to_graph(state)).is_interval;
            interval_add_edge(&state, representative, y);
            if (!is_interval) continue;

            for (int x = pivot + 1; x <= state.total_n; ++x) {
                interval_remove_edge(&state, x, y);
                kiyomi_kijima_uno_interval_dfs(state, cb);
                interval_add_edge(&state, x, y);
            }
        }

        // Deleting an edge between two universal vertices has the same answer
        // for every such pair, but is not always interval when the remaining
        // graph is non-complete.
        if (pivot + 1 < state.total_n) {
            interval_remove_edge(&state, pivot + 1, pivot + 2);
            const bool is_interval =
                check_interval(interval_edge_state_to_graph(state)).is_interval;
            interval_add_edge(&state, pivot + 1, pivot + 2);
            if (is_interval) {
                for (int x = pivot + 1; x <= state.total_n; ++x) {
                    for (int y = x + 1; y <= state.total_n; ++y) {
                        interval_remove_edge(&state, x, y);
                        kiyomi_kijima_uno_interval_dfs(state, cb);
                        interval_add_edge(&state, x, y);
                    }
                }
            }
        }
    }

    // For an edge below pivot, the child's parent still adds an edge incident
    // with pivot and therefore cannot return to state. Only pivot edges need
    // recognition and an explicit parent check.
    for (int u = 1; u < pivot; ++u) {
        if (!state.adj[u][pivot]) continue;
        interval_remove_edge(&state, u, pivot);
        bool is_child = false;
        {
            const IntervalResult model =
                check_interval(interval_edge_state_to_graph(state));
            is_child = model.is_interval &&
                interval_parent_edge_from_model(state, model) ==
                    std::make_pair(u, pivot);
        }
        if (is_child) {
            kiyomi_kijima_uno_interval_dfs(state, cb);
        }
        interval_add_edge(&state, u, pivot);
    }
}

template <typename Callback>
inline void enumerate_interval_graphs_kiyomi_kijima_uno_cb(int n,
                                                            Callback& cb) {
    KiyomiKijimaUnoIntervalState root(n);
    kiyomi_kijima_uno_interval_dfs(root, cb);
}

/** @brief Constructs a compact Graph from the legacy vertex-search state */
inline Graph interval_legacy_state_to_graph(const ChordalEnumState& state) {
    std::vector<int> remap(state.total_n + 1, 0);
    int count = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) remap[v] = ++count;
    }
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= state.total_n; ++u) {
        if (!state.alive[u]) continue;
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.alive[v] && state.adj[u][v]) {
                edges.push_back(std::make_pair(remap[u], remap[v]));
            }
        }
    }
    return Graph(count, edges);
}

/** @brief Previous chordal-tree filtering implementation */
template <typename Callback>
inline void interval_legacy_reverse_search_dfs(const ChordalEnumState& state,
                                                Callback& cb) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        cb(graph);
        return;
    }

    std::vector<ChordalEnumState> children;
    collect_children_reverse_search(state, &children);
    for (std::size_t i = 0; i < children.size(); ++i) {
        if (!check_interval(interval_legacy_state_to_graph(children[i])).is_interval) {
            continue;
        }
        interval_legacy_reverse_search_dfs(children[i], cb);
    }
}

/** @brief Callback adapter for the materializing API */
struct IntervalAppendToVector {
    std::vector<EnumeratedGraph>* out;
    void operator()(const EnumeratedGraph& graph) { out->push_back(graph); }
};

}  // namespace detail

/**
 * @brief Enumerates all labeled interval graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selector; KIYOMI_KIJIMA_UNO is the default
 * @return Materialized enumeration result
 *
 * The default starts at K_n and follows the Kiyomi--Kijima--Uno family tree
 * by edge deletion. LEGACY_CHORDAL_FILTER retains the former vertex-addition
 * chordal search for compatibility and differential testing.
 */
inline IntervalEnumerationResult enumerate_interval_graphs_reverse_search(
    int n,
    IntervalEnumAlgorithm algo = IntervalEnumAlgorithm::KIYOMI_KIJIMA_UNO) {
    IntervalEnumerationResult result;
    if (n < 0) return result;
    detail::IntervalAppendToVector cb;
    cb.out = &result.graphs;
    if (algo == IntervalEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalEnumState root(n);
        detail::interval_legacy_reverse_search_dfs(root, cb);
    } else {
        detail::enumerate_interval_graphs_kiyomi_kijima_uno_cb(n, cb);
    }
    return result;
}

/**
 * @brief Streams all labeled interval graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const EnumeratedGraph&) for each graph
 * @param algo Enumeration algorithm; KIYOMI_KIJIMA_UNO is the default
 *
 * With the default algorithm, unlike the materializing API, this keeps only
 * O(n^2) reverse-search state including the recursion stack. Each callback
 * still receives a complete edge list. The legacy algorithm may retain its
 * materialized child-state vectors at active recursion levels.
 */
template <typename Callback>
inline void enumerate_interval_graphs_reverse_search_cb(
    int n,
    Callback&& cb,
    IntervalEnumAlgorithm algo = IntervalEnumAlgorithm::KIYOMI_KIJIMA_UNO) {
    if (n < 0) return;
    if (algo == IntervalEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalEnumState root(n);
        detail::interval_legacy_reverse_search_dfs(root, cb);
    } else {
        detail::enumerate_interval_graphs_kiyomi_kijima_uno_cb(n, cb);
    }
}

}  // namespace graph_recognition

#endif
