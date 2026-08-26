#ifndef GRAPH_RECOGNITION_WEAKLY_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_WEAKLY_CHORDAL_ENUM_H

/**
 * @file weakly_chordal_labeled_enum.h
 * @brief Enumeration of labeled weakly chordal graphs
 *
 * The default is Kiyomi's weakly-chordal-subgraph reverse search, applied to
 * the host graph K_n. Its root is the empty graph. Fix the lexicographic order
 * of the edges of K_n; the parent of a nonempty weakly chordal graph H removes
 * the largest edge e for which H-e is still weakly chordal. Hayward's
 * generation theorem guarantees that such an edge always exists.
 *
 * The previous generic construction is retained as
 * GENERIC_VERTEX_AUGMENTATION. It adds vertices in label order, tries every
 * possible neighborhood, and prunes non-weakly-chordal prefixes.
 *
 * Reference: M. Kiyomi, "Studies on Subgraph and Supergraph Enumeration
 * Algorithms," Ph.D. thesis, The Graduate University for Advanced Studies,
 * 2006, Section 4.1.5, Theorem 4.16.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "recognizers/weakly_chordal.h"

namespace graph_recognition {

/** @brief Algorithm selection for weakly chordal graph enumeration */
enum class WeaklyChordalLabeledEnumAlgorithm {
    KIYOMI_EDGE_ADDITION = 0, /**< Kiyomi's edge-addition reverse search */
    KIYOMI = KIYOMI_EDGE_ADDITION, /**< Short alias for the default */
    REVERSE_SEARCH = KIYOMI_EDGE_ADDITION, /**< Backward-compatible name */
    GENERIC_VERTEX_AUGMENTATION = 1 /**< Previous hereditary-class search */
};

/** @brief Result of weakly chordal graph enumeration */
struct WeaklyChordalLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of labeled weakly chordal graphs */
};

namespace detail {

/** @brief State of Kiyomi's weakly-chordal-subgraph reverse search */
struct KiyomiWeaklyChordalState {
    int total_n;
    std::size_t edge_count;
    std::vector<std::pair<int, int>> host_edges;
    std::vector<char> present;

    explicit KiyomiWeaklyChordalState(int n)
        : total_n(n), edge_count(0) {
        const std::size_t max_edges =
            static_cast<std::size_t>(n) * static_cast<std::size_t>(n - 1) / 2;
        host_edges.reserve(max_edges);
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                host_edges.push_back(std::make_pair(u, v));
            }
        }
        present.assign(host_edges.size(), 0);
    }
};

inline std::vector<std::pair<int, int>> collect_kiyomi_weakly_chordal_edges(
    const KiyomiWeaklyChordalState& state,
    std::size_t omitted = static_cast<std::size_t>(-1)) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve(state.edge_count -
                  ((omitted < state.present.size() && state.present[omitted]) ? 1 : 0));
    for (std::size_t i = 0; i < state.host_edges.size(); ++i) {
        if (state.present[i] && i != omitted) {
            edges.push_back(state.host_edges[i]);
        }
    }
    return edges;
}

inline bool kiyomi_weakly_chordal_after_omitting(
    const KiyomiWeaklyChordalState& state,
    std::size_t omitted = static_cast<std::size_t>(-1)) {
    return check_weakly_chordal(
        Graph(state.total_n,
              collect_kiyomi_weakly_chordal_edges(state, omitted)))
        .is_weakly_chordal;
}

/**
 * @brief Tests whether the newly added edge is the child's canonical parent edge
 *
 * The new edge is already known to be removable because deleting it restores
 * the current weakly chordal parent. It is canonical exactly when no larger
 * present edge can be removed while preserving weak chordality.
 */
inline bool kiyomi_weakly_chordal_is_parent_edge(
    const KiyomiWeaklyChordalState& state,
    std::size_t added_edge) {
    for (std::size_t i = state.host_edges.size(); i > added_edge + 1;) {
        --i;
        if (!state.present[i]) continue;
        if (kiyomi_weakly_chordal_after_omitting(state, i)) return false;
    }
    return true;
}

/**
 * @brief Kiyomi reverse-search traversal
 *
 * Nodes at even depth are emitted before their children and nodes at odd
 * depth after their children. This is the standard reverse-search output
 * scheduling that turns the O(m^4)-per-node bound into an O(m^4) delay bound
 * when parent and child differ by one edge.
 */
template <typename Callback>
inline void kiyomi_weakly_chordal_dfs(KiyomiWeaklyChordalState& state,
                                      Callback& cb) {
    const bool emit_before_children = (state.edge_count % 2 == 0);
    if (emit_before_children) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_kiyomi_weakly_chordal_edges(state);
        cb(graph);
    }

    for (std::size_t edge = 0; edge < state.host_edges.size(); ++edge) {
        if (state.present[edge]) continue;

        state.present[edge] = 1;
        ++state.edge_count;

        const bool weakly_chordal =
            kiyomi_weakly_chordal_after_omitting(state);
        if (weakly_chordal &&
            kiyomi_weakly_chordal_is_parent_edge(state, edge)) {
            kiyomi_weakly_chordal_dfs(state, cb);
        }

        --state.edge_count;
        state.present[edge] = 0;
    }

    if (!emit_before_children) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_kiyomi_weakly_chordal_edges(state);
        cb(graph);
    }
}

template <typename Callback>
inline void enumerate_weakly_chordal_labeled_graphs_kiyomi_cb(int n, Callback& cb) {
    KiyomiWeaklyChordalState root(n);
    kiyomi_weakly_chordal_dfs(root, cb);
}

/** @brief State of the generic largest-label vertex augmentation */
struct WeaklyChordalVertexAugmentationState {
    int total_n;
    int alive_count; /**< Active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit WeaklyChordalVertexAugmentationState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/** @brief Previous generic hereditary-class traversal */
template <typename Callback>
inline void weakly_chordal_vertex_augmentation_dfs(
    WeaklyChordalVertexAugmentationState& state,
    Callback& cb) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u) {
            for (int v = u + 1; v <= state.total_n; ++v) {
                if (state.adj[u][v]) {
                    graph.edges.push_back(std::make_pair(u, v));
                }
            }
        }
        cb(graph);
        return;
    }

    const int x = state.alive_count + 1;
    const int k = state.alive_count;
    if (k >= 64) return;
    const unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u) {
        for (int v = u + 1; v <= k; ++v) {
            if (state.adj[u][v]) {
                base_edges.push_back(std::make_pair(u, v));
            }
        }
    }

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        for (int u = 1; u <= k; ++u) {
            const char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
        }
        state.alive_count = x;

        std::vector<std::pair<int, int>> edges = base_edges;
        for (int u = 1; u <= k; ++u) {
            if (state.adj[x][u]) edges.push_back(std::make_pair(u, x));
        }

        if (check_weakly_chordal(Graph(x, edges)).is_weakly_chordal) {
            weakly_chordal_vertex_augmentation_dfs(state, cb);
        }

        for (int u = 1; u <= k; ++u) {
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.alive_count = k;
    }
}

/** @brief Callback adapter for the materializing API */
struct WeaklyChordalAppendToVector {
    std::vector<EnumeratedGraph>* out;
    void operator()(const EnumeratedGraph& graph) { out->push_back(graph); }
};

}  // namespace detail

/**
 * @brief Enumerates all labeled weakly chordal graphs on {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selector; KIYOMI_EDGE_ADDITION is the default
 * @return Materialized enumeration result
 *
 * Kiyomi's analysis gives O(m^4) time per output with an O(m^2) recognizer.
 * This implementation reuses check_weakly_chordal, whose documented
 * worst-case bound is O(n^6); the resulting direct bound on K_n is O(n^10)
 * delay. The generic fallback retains the former largest-label vertex
 * augmentation.
 */
inline WeaklyChordalLabeledEnumerationResult
enumerate_weakly_chordal_labeled_graphs_reverse_search(
    int n,
    WeaklyChordalLabeledEnumAlgorithm algo =
        WeaklyChordalLabeledEnumAlgorithm::KIYOMI_EDGE_ADDITION) {
    WeaklyChordalLabeledEnumerationResult result;
    if (n < 0) return result;

    detail::WeaklyChordalAppendToVector cb;
    cb.out = &result.graphs;
    if (algo == WeaklyChordalLabeledEnumAlgorithm::GENERIC_VERTEX_AUGMENTATION) {
        detail::WeaklyChordalVertexAugmentationState root(n);
        detail::weakly_chordal_vertex_augmentation_dfs(root, cb);
    } else {
        detail::enumerate_weakly_chordal_labeled_graphs_kiyomi_cb(n, cb);
    }
    return result;
}

/**
 * @brief Streams all labeled weakly chordal graphs on {1, ..., n}
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const EnumeratedGraph&) for each graph
 * @param algo Algorithm selector; KIYOMI_EDGE_ADDITION is the default
 */
template <typename Callback>
inline void enumerate_weakly_chordal_labeled_graphs_reverse_search_cb(
    int n,
    Callback&& cb,
    WeaklyChordalLabeledEnumAlgorithm algo =
        WeaklyChordalLabeledEnumAlgorithm::KIYOMI_EDGE_ADDITION) {
    if (n < 0) return;
    if (algo == WeaklyChordalLabeledEnumAlgorithm::GENERIC_VERTEX_AUGMENTATION) {
        detail::WeaklyChordalVertexAugmentationState root(n);
        detail::weakly_chordal_vertex_augmentation_dfs(root, cb);
    } else {
        detail::enumerate_weakly_chordal_labeled_graphs_kiyomi_cb(n, cb);
    }
}

}  // namespace graph_recognition

#endif
