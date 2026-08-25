#ifndef GRAPH_RECOGNITION_PROPER_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_PROPER_CHORDAL_ENUM_H

/**
 * @file proper_chordal_enum.h
 * @brief Proper chordal graph enumeration by edge-addition reverse search
 *
 * Enumerates all labeled proper chordal graphs on {1, ..., n}.  The default
 * algorithm is a proper-chordal-specific reverse search whose root is the
 * empty graph.  The recognizer constructs a deterministic indifference
 * tree-layout T(G).  For a nonempty proper chordal graph G, let e(G) be the
 * lexicographically first graph edge of maximum distance in T(G), and define
 * parent(G) = G-e(G).  Children are obtained by adding one missing edge and
 * retaining it exactly when the added edge is the child's canonical parent
 * edge.
 *
 * The parent remains inside the class.  In any indifference tree-layout of a
 * nonempty connected component, take an edge of maximum tree distance.
 * Removing it cannot create a forbidden indifference triple: if that edge
 * were required by another edge spanning an ancestor--descendant path, the
 * latter edge would have strictly greater tree distance.  The same tree is
 * therefore an indifference tree-layout after the deletion.
 *
 * Consequently every search node is itself proper chordal; this algorithm
 * does not enumerate the larger class of chordal graphs and filter only its
 * completed leaves.  Candidate and canonical-parent tests use the existing
 * recognizer.  With a polynomial-time recognizer this gives polynomial delay
 * and polynomial space.  The recognizer in proper_chordal.h currently uses a
 * factorial nested-convex subroutine, so that implementation-level caveat
 * also applies here.
 *
 * The structural deletion argument uses the indifference tree-layout
 * characterization in Theorem 6 of:
 * C. Paul and E. Protopapas, "Tree-Layout Based Graph Classes: Proper
 * Chordal Graphs," STACS 2024, LIPIcs 289, Article 55.
 * The paper gives recognition and representation algorithms, not this
 * enumeration algorithm.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "proper_chordal.h"

namespace graph_recognition {

/** @brief Algorithm selection for proper chordal graph enumeration */
enum class ProperChordalEnumAlgorithm {
    INDIFFERENCE_EDGE_ADDITION = 0, /**< Proper-chordal-specific edge search */
    REVERSE_SEARCH = INDIFFERENCE_EDGE_ADDITION, /**< Backward-compatible name */
    LEGACY_CHORDAL_FILTER = 1 /**< Chordal vertex search plus property filtering */
};

/** @brief Result of proper chordal graph enumeration */
struct ProperChordalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Labeled proper chordal graphs */
};

namespace detail {

/** @brief Adjacency-matrix state for proper chordal edge reverse search */
struct ProperChordalEdgeState {
    int total_n;
    std::size_t edge_count;
    std::vector<std::vector<char>> adj;

    explicit ProperChordalEdgeState(int n)
        : total_n(n), edge_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

inline void proper_chordal_add_edge(ProperChordalEdgeState* state,
                                    int u,
                                    int v) {
    state->adj[u][v] = 1;
    state->adj[v][u] = 1;
    ++state->edge_count;
}

inline void proper_chordal_remove_edge(ProperChordalEdgeState* state,
                                       int u,
                                       int v) {
    state->adj[u][v] = 0;
    state->adj[v][u] = 0;
    --state->edge_count;
}

inline std::vector<std::pair<int, int>> collect_proper_chordal_edges(
    const ProperChordalEdgeState& state) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve(state.edge_count);
    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.adj[u][v]) edges.push_back(std::make_pair(u, v));
        }
    }
    return edges;
}

inline bool find_proper_chordal_edge_state_layout(
    const ProperChordalEdgeState& state,
    std::vector<int>* tree_parent) {
    const Graph graph(state.total_n, collect_proper_chordal_edges(state));
    if (!check_chordal(graph).is_chordal) return false;
    return detail_tree_layout::find_indifference_tree_layout(graph, tree_parent);
}

/**
 * @brief Selects a canonical deletable edge from a witnessed layout
 *
 * An edge of maximum tree distance can be deleted while preserving the same
 * indifference tree-layout.  Labels break ties, making the parent a function
 * of the graph and the deterministic layout returned by the recognizer.
 */
inline std::pair<int, int> proper_chordal_parent_edge(
    const ProperChordalEdgeState& state,
    const std::vector<int>& tree_parent) {
    if (state.edge_count == 0) return std::make_pair(0, 0);

    std::vector<int> depth(state.total_n + 1, -1);
    for (int start = 1; start <= state.total_n; ++start) {
        if (depth[start] >= 0) continue;
        std::vector<int> path;
        int v = start;
        while (v != 0 && depth[v] < 0) {
            path.push_back(v);
            v = tree_parent[v];
        }
        int d = v == 0 ? -1 : depth[v];
        for (std::size_t i = path.size(); i > 0; --i) {
            depth[path[i - 1]] = ++d;
        }
    }

    int best_distance = -1;
    std::pair<int, int> best = std::make_pair(0, 0);
    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (!state.adj[u][v]) continue;
            const int distance = depth[u] < depth[v]
                ? depth[v] - depth[u]
                : depth[u] - depth[v];
            if (distance > best_distance) {
                best_distance = distance;
                best = std::make_pair(u, v);
            }
        }
    }
    return best;
}

template <typename Callback>
inline void proper_chordal_edge_dfs(ProperChordalEdgeState& state,
                                    Callback& cb) {
    {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_proper_chordal_edges(state);
        cb(graph);
    }

    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.adj[u][v]) continue;

            proper_chordal_add_edge(&state, u, v);
            bool is_child = false;
            {
                // Release the layout witness before descending so live
                // reverse-search storage remains O(n^2), including recursion.
                std::vector<int> tree_parent;
                is_child = find_proper_chordal_edge_state_layout(
                               state, &tree_parent) &&
                           proper_chordal_parent_edge(state, tree_parent) ==
                               std::make_pair(u, v);
            }
            if (is_child) {
                proper_chordal_edge_dfs(state, cb);
            }
            proper_chordal_remove_edge(&state, u, v);
        }
    }
}

template <typename Callback>
inline void enumerate_proper_chordal_graphs_edge_cb(int n, Callback& cb) {
    ProperChordalEdgeState root(n);
    proper_chordal_edge_dfs(root, cb);
}

/** @brief Build a Graph from the legacy chordal vertex-search state */
inline Graph proper_chordal_state_to_graph(const ChordalEnumState& state) {
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

/** @brief Previous chordal-vertex-search implementation, kept for validation */
template <typename Callback>
inline void proper_chordal_legacy_dfs(const ChordalEnumState& state,
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
        const Graph graph = proper_chordal_state_to_graph(children[i]);
        if (!check_proper_chordal(graph).is_proper_chordal) continue;
        proper_chordal_legacy_dfs(children[i], cb);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled proper chordal graphs on {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selector; INDIFFERENCE_EDGE_ADDITION is the default
 */
inline ProperChordalEnumerationResult
enumerate_proper_chordal_graphs_reverse_search(
    int n,
    ProperChordalEnumAlgorithm algo =
        ProperChordalEnumAlgorithm::INDIFFERENCE_EDGE_ADDITION) {
    ProperChordalEnumerationResult result;
    if (n < 0) return result;

    detail::AppendToVector cb;
    cb.out = &result.graphs;
    if (algo == ProperChordalEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalEnumState root(n);
        detail::proper_chordal_legacy_dfs(root, cb);
    } else {
        detail::enumerate_proper_chordal_graphs_edge_cb(n, cb);
    }
    return result;
}

/**
 * @brief Streaming enumeration of labeled proper chordal graphs
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const EnumeratedGraph&) for every graph
 * @param algo Enumeration algorithm; INDIFFERENCE_EDGE_ADDITION is the default
 *
 * The callback API retains only the O(n^2) adjacency state rather than all
 * output graphs.  Canonical-parent tests rebuild complete edge lists for the
 * recognizer, so the implementation favors clarity over incremental updates.
 */
template <typename Callback>
inline void enumerate_proper_chordal_graphs_reverse_search_cb(
    int n,
    Callback&& cb,
    ProperChordalEnumAlgorithm algo =
        ProperChordalEnumAlgorithm::INDIFFERENCE_EDGE_ADDITION) {
    if (n < 0) return;
    if (algo == ProperChordalEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalEnumState root(n);
        detail::proper_chordal_legacy_dfs(root, cb);
    } else {
        detail::enumerate_proper_chordal_graphs_edge_cb(n, cb);
    }
}

}  // namespace graph_recognition

#endif
