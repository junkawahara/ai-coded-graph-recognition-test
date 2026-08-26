#ifndef GRAPH_RECOGNITION_SPLIT_ENUM_H
#define GRAPH_RECOGNITION_SPLIT_ENUM_H

/**
 * @file split_labeled_enum.h
 * @brief Enumeration of labeled split graphs by canonical KS-partitions
 *
 * The default algorithm enumerates S-max partitions V = K union S directly.
 * K induces a clique, S induces a stable set, and every vertex of K is given
 * a nonempty neighborhood in S.  The last condition is exactly S-maximality.
 * Thus every generated candidate is a split graph and no recognition test is
 * needed.
 *
 * An underlying graph can have several S-max partitions only when its swing
 * vertices form a clique A.  In such a partition exactly one a in A lies in
 * S; it is universal to K, while every vertex in A - {a} has cross
 * neighborhood {a}.  The algorithm emits only the partition in which a is
 * the smallest-labeled vertex of A.  This gives every labeled split graph
 * exactly once.
 *
 * References:
 * C. Cheng, K. L. Collins, and A. N. Trenk, "Split graphs and
 * Nordhaus--Gaddum graphs," Discrete Mathematics 339(9), 2345--2356, 2016.
 * DOI: 10.1016/j.disc.2016.04.001
 *
 * J. M. Troyka, "Split graphs: combinatorial species and asymptotics,"
 * Electronic Journal of Combinatorics 26(2), P2.42, 2019.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "recognizers/split.h"

namespace graph_recognition {

/** @brief Algorithm selection for split graph enumeration */
enum class SplitLabeledEnumAlgorithm {
    KS_PARTITION_CANONICAL = 0, /**< Split-specific canonical KS-partitions */
    REVERSE_SEARCH = KS_PARTITION_CANONICAL, /**< Backward-compatible name */
    LEGACY_CHORDAL_FILTER = 1 /**< Chordal vertex search plus split filtering */
};

/** @brief Result of split graph enumeration */
struct SplitLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of labeled split graphs */
};

namespace detail {

/** @brief State for one fixed S-max partition V = K union S */
struct CanonicalSplitKSState {
    int total_n;
    std::size_t edge_count;
    std::vector<int> clique;
    std::vector<int> stable;
    std::vector<int> clique_cross_degree;
    std::vector<int> stable_cross_degree;
    std::vector<std::vector<char>> adj;

    CanonicalSplitKSState(int n,
                          const std::vector<int>& clique_vertices,
                          const std::vector<int>& stable_vertices)
        : total_n(n),
          edge_count(0),
          clique(clique_vertices),
          stable(stable_vertices),
          clique_cross_degree(n + 1, 0),
          stable_cross_degree(n + 1, 0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {
        for (std::size_t i = 0; i < clique.size(); ++i) {
            for (std::size_t j = i + 1; j < clique.size(); ++j) {
                const int u = clique[i];
                const int v = clique[j];
                adj[u][v] = 1;
                adj[v][u] = 1;
                ++edge_count;
            }
        }
    }
};

inline void split_add_cross_edge(CanonicalSplitKSState* state,
                                 int clique_vertex,
                                 int stable_vertex) {
    state->adj[clique_vertex][stable_vertex] = 1;
    state->adj[stable_vertex][clique_vertex] = 1;
    ++state->clique_cross_degree[clique_vertex];
    ++state->stable_cross_degree[stable_vertex];
    ++state->edge_count;
}

inline void split_remove_cross_edge(CanonicalSplitKSState* state,
                                    int clique_vertex,
                                    int stable_vertex) {
    state->adj[clique_vertex][stable_vertex] = 0;
    state->adj[stable_vertex][clique_vertex] = 0;
    --state->clique_cross_degree[clique_vertex];
    --state->stable_cross_degree[stable_vertex];
    --state->edge_count;
}

inline std::vector<std::pair<int, int>> collect_canonical_split_edges(
    const CanonicalSplitKSState& state) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve(state.edge_count);
    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.adj[u][v]) edges.push_back(std::make_pair(u, v));
        }
    }
    return edges;
}

/**
 * @brief Whether this S-max partition is the canonical one for its graph
 *
 * Let a be stable and universal to K.  If some k in K has N_S(k) = {a},
 * then a together with all such k is precisely the clique A of swing
 * vertices.  There is one S-max partition for each choice of a in A, so only
 * the partition with min(A) = a is retained.  If no such pair exists, the
 * graph has a unique S-max partition.
 */
inline bool is_canonical_split_s_max_partition(
    const CanonicalSplitKSState& state) {
    const int clique_size = static_cast<int>(state.clique.size());
    for (std::size_t si = 0; si < state.stable.size(); ++si) {
        const int a = state.stable[si];
        if (state.stable_cross_degree[a] != clique_size) continue;

        bool has_other_swing = false;
        int minimum_swing_label = a;
        for (std::size_t ki = 0; ki < state.clique.size(); ++ki) {
            const int k = state.clique[ki];
            if (state.clique_cross_degree[k] == 1 && state.adj[k][a]) {
                has_other_swing = true;
                if (k < minimum_swing_label) minimum_swing_label = k;
            }
        }
        if (has_other_swing) return a == minimum_swing_label;
    }
    return true;
}

template <typename Callback>
inline void emit_canonical_split_candidate(CanonicalSplitKSState& state,
                                           Callback& cb) {
    if (!is_canonical_split_s_max_partition(state)) return;
    EnumeratedGraph graph;
    graph.n = state.total_n;
    graph.edges = collect_canonical_split_edges(state);
    cb(graph);
}

template <typename Callback>
inline void enumerate_split_cross_neighborhoods(
    CanonicalSplitKSState& state,
    std::size_t clique_index,
    std::size_t stable_index,
    bool neighborhood_nonempty,
    Callback& cb) {
    if (clique_index == state.clique.size()) {
        emit_canonical_split_candidate(state, cb);
        return;
    }

    if (stable_index == state.stable.size()) {
        // Every K vertex must see S; otherwise it could move to S and the
        // chosen partition would not be S-max.
        if (neighborhood_nonempty) {
            enumerate_split_cross_neighborhoods(
                state, clique_index + 1, 0, false, cb);
        }
        return;
    }

    // Do not connect the current pair.
    enumerate_split_cross_neighborhoods(
        state, clique_index, stable_index + 1,
        neighborhood_nonempty, cb);

    // Connect the current K--S pair and restore it after the subtree.
    const int k = state.clique[clique_index];
    const int s = state.stable[stable_index];
    split_add_cross_edge(&state, k, s);
    enumerate_split_cross_neighborhoods(
        state, clique_index, stable_index + 1, true, cb);
    split_remove_cross_edge(&state, k, s);
}

template <typename Callback>
inline void enumerate_split_fixed_partition(
    int n,
    const std::vector<int>& clique,
    const std::vector<int>& stable,
    Callback& cb) {
    CanonicalSplitKSState state(n, clique, stable);
    enumerate_split_cross_neighborhoods(state, 0, 0, false, cb);
}

template <typename Callback>
inline void enumerate_split_partitions(
    int n,
    int next_vertex,
    std::vector<int>* clique,
    std::vector<int>* stable,
    Callback& cb) {
    if (next_vertex > n) {
        enumerate_split_fixed_partition(n, *clique, *stable, cb);
        return;
    }

    stable->push_back(next_vertex);
    enumerate_split_partitions(
        n, next_vertex + 1, clique, stable, cb);
    stable->pop_back();

    clique->push_back(next_vertex);
    enumerate_split_partitions(
        n, next_vertex + 1, clique, stable, cb);
    clique->pop_back();
}

template <typename Callback>
inline void enumerate_split_labeled_graphs_canonical_ks_cb(int n, Callback& cb) {
    std::vector<int> clique;
    std::vector<int> stable;
    clique.reserve(static_cast<std::size_t>(n));
    stable.reserve(static_cast<std::size_t>(n));
    enumerate_split_partitions(n, 1, &clique, &stable, cb);
}

/** @brief Build a Graph from the legacy chordal search state */
inline Graph split_state_to_graph(const ChordalLabeledEnumState& state) {
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
inline void split_legacy_reverse_search_dfs(const ChordalLabeledEnumState& state,
                                            Callback& cb) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        cb(graph);
        return;
    }

    std::vector<ChordalLabeledEnumState> children;
    collect_children_reverse_search(state, &children);
    for (std::size_t i = 0; i < children.size(); ++i) {
        const Graph graph = split_state_to_graph(children[i]);
        if (!check_split(graph).is_split) continue;
        split_legacy_reverse_search_dfs(children[i], cb);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled split graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selector; KS_PARTITION_CANONICAL is the default
 * @return Materialized enumeration result
 *
 * The default directly enumerates canonical S-max KS-partitions.  The legacy
 * chordal search remains available for compatibility and differential tests.
 */
inline SplitLabeledEnumerationResult enumerate_split_labeled_graphs_reverse_search(
    int n,
    SplitLabeledEnumAlgorithm algo =
        SplitLabeledEnumAlgorithm::KS_PARTITION_CANONICAL) {
    SplitLabeledEnumerationResult result;
    if (n < 0) return result;

    detail::AppendToVector cb;
    cb.out = &result.graphs;
    if (algo == SplitLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalLabeledEnumState root(n);
        detail::split_legacy_reverse_search_dfs(root, cb);
    } else {
        detail::enumerate_split_labeled_graphs_canonical_ks_cb(n, cb);
    }
    return result;
}

/**
 * @brief Streams all labeled split graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const EnumeratedGraph&) for every graph
 * @param algo Enumeration algorithm; KS_PARTITION_CANONICAL is the default
 *
 * The default algorithm keeps O(n^2) adjacency and degree state.  Each
 * callback still receives a complete edge list.  The materializing wrapper
 * additionally retains all output graphs.
 */
template <typename Callback>
inline void enumerate_split_labeled_graphs_reverse_search_cb(
    int n,
    Callback&& cb,
    SplitLabeledEnumAlgorithm algo =
        SplitLabeledEnumAlgorithm::KS_PARTITION_CANONICAL) {
    if (n < 0) return;
    if (algo == SplitLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalLabeledEnumState root(n);
        detail::split_legacy_reverse_search_dfs(root, cb);
    } else {
        detail::enumerate_split_labeled_graphs_canonical_ks_cb(n, cb);
    }
}

}  // namespace graph_recognition

#endif
