#ifndef GRAPH_RECOGNITION_CHORDAL_SUBGRAPH_ENUM_H
#define GRAPH_RECOGNITION_CHORDAL_SUBGRAPH_ENUM_H

/**
 * @file chordal_subgraph_enum.h
 * @brief Enumeration of the chordal subgraphs of a given graph
 *        (Kiyomi--Uno reverse search)
 *
 * Enumerates every spanning subgraph (V, E') with E' a subset of E(G) that is
 * chordal, for a given host graph G.  This is the problem the Kiyomi--Uno
 * reverse search was designed for; the fixed-n enumerator in chordal_labeled_enum.h is
 * its G = K_n special case, and this header reuses that search machinery.
 *
 * The parent of a nonempty chordal graph deletes the edges at its
 * minimum-degree simplicial vertex (ties broken by smallest label).  Since
 * that is an edge deletion, the subgraphs of a fixed host are closed under it,
 * so restricting the search tree to subgraphs of G leaves a tree rooted at the
 * one-edge subgraphs: filtering child generation by host adjacency is enough,
 * and no canonicality rule has to change.
 *
 * Reference: M. Kiyomi and T. Uno, "Generating Chordal Graphs Included
 * in Given Graphs," IEICE Trans. Inf. & Syst. E89-D(2), 763--770, 2006.
 * DOI: 10.1093/ietisy/e89-d.2.763
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for chordal subgraph enumeration
 */
enum class ChordalSubgraphEnumAlgorithm {
    KIYOMI_UNO = 0 /**< Kiyomi--Uno chordal-subgraph reverse search */
};

/**
 * @brief Result of chordal subgraph enumeration
 */
struct ChordalSubgraphEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< enumerated chordal subgraphs */
};

namespace detail {

/** @brief Adjacency matrix of the host graph, indexed [1..n][1..n] */
inline std::vector<std::vector<char>> chordal_subgraph_host_matrix(
    const Graph& g) {
    std::vector<std::vector<char>> host(
        g.n + 1, std::vector<char>(g.n + 1, 0));
    for (int u = 1; u <= g.n; ++u) {
        for (std::size_t i = 0; i < g.adj[u].size(); ++i) {
            host[u][g.adj[u][i]] = 1;
        }
    }
    return host;
}

template <typename Callback>
inline void chordal_subgraph_reverse_search_dfs(
    KiyomiUnoChordalState& state,
    const std::vector<std::vector<char>>& host,
    Callback& cb);

/**
 * @brief Clique callback generating the host-realizable children
 *
 * The same children as KiyomiUnoExistingCliqueChildren, except that the new
 * vertex must be host-adjacent to every clique member so that the child stays
 * a subgraph of the host.  kiyomi_uno_is_child_clique() only inspects the
 * current state, so the canonicality test is reused unchanged.
 */
template <typename Callback>
struct ChordalSubgraphCliqueChildren {
    KiyomiUnoChordalState* state;
    const KiyomiUnoNodeInfo* info;
    const std::vector<std::vector<char>>* host;
    Callback* callback;

    void operator()(const std::vector<int>& clique) {
        std::vector<char> in_clique(state->total_n + 1, 0);
        for (std::size_t i = 0; i < clique.size(); ++i) {
            in_clique[clique[i]] = 1;
        }

        for (int v = 1; v <= state->total_n; ++v) {
            if (state->alive[v]) continue;

            bool host_ok = true;
            for (std::size_t i = 0; i < clique.size(); ++i) {
                if (!(*host)[v][clique[i]]) {
                    host_ok = false;
                    break;
                }
            }
            if (!host_ok) continue;

            if (!kiyomi_uno_is_child_clique(
                    *state, *info, v, clique, in_clique)) {
                continue;
            }

            std::vector<int> lost_simplicial;
            kiyomi_uno_add_vertex(
                state, v, clique, in_clique, &lost_simplicial);
            chordal_subgraph_reverse_search_dfs(*state, *host, *callback);
            kiyomi_uno_remove_vertex(state, v, clique, lost_simplicial);
        }
    }
};

template <typename Callback>
inline void chordal_subgraph_reverse_search_dfs(
    KiyomiUnoChordalState& state,
    const std::vector<std::vector<char>>& host,
    Callback& cb) {
    EnumeratedGraph graph;
    graph.n = state.total_n;
    graph.edges = collect_kiyomi_uno_edges(state);
    cb(graph);

    const KiyomiUnoNodeInfo info = kiyomi_uno_node_info(state);

    ChordalSubgraphCliqueChildren<Callback> existing_children;
    existing_children.state = &state;
    existing_children.info = &info;
    existing_children.host = &host;
    existing_children.callback = &cb;
    enumerate_kiyomi_uno_cliques(state, info.k + 1, existing_children);

    // In a disconnected subgraph, C may also be a singleton unused vertex.
    // The ordering v < w removes the duplicate representations
    // G(v,{w}) == G(w,{v}); the new component's edge must exist in the host.
    const std::vector<int> singleton_clique(1, 0);
    const std::vector<char> empty_membership(state.total_n + 1, 0);
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) continue;
        for (int w = v + 1; w <= state.total_n; ++w) {
            if (state.alive[w]) continue;
            if (!host[v][w]) continue;
            if (!kiyomi_uno_is_child_clique(
                    state, info, v, singleton_clique, empty_membership)) {
                continue;
            }
            kiyomi_uno_add_isolated_edge(&state, v, w);
            chordal_subgraph_reverse_search_dfs(state, host, cb);
            kiyomi_uno_remove_isolated_edge(&state, v, w);
        }
    }
}

/**
 * @brief Callback translating core-graph output back to host labels
 *
 * The search runs on the vertices that carry a host edge, so the graphs it
 * emits are numbered in that core.  label[] maps them back; it is increasing,
 * so the u < v order of each edge survives the translation.
 */
template <typename Callback>
struct ChordalSubgraphRelabel {
    const std::vector<int>* label;
    int host_n;
    Callback* callback;

    void operator()(const EnumeratedGraph& g) {
        EnumeratedGraph out;
        out.n = host_n;
        out.edges.reserve(g.edges.size());
        for (std::size_t i = 0; i < g.edges.size(); ++i) {
            out.edges.push_back(
                std::make_pair((*label)[g.edges[i].first],
                               (*label)[g.edges[i].second]));
        }
        (*callback)(out);
    }
};

template <typename Callback>
inline void enumerate_chordal_subgraphs_kiyomi_uno_cb(const Graph& g,
                                                      Callback& cb) {
    EnumeratedGraph empty;
    empty.n = g.n;
    cb(empty);
    if (g.n < 2) return;

    /* Only a vertex with a host edge can appear in a subgraph's edge set, and
       both the host matrix and the search state are Theta(k^2) in the number k
       of search vertices.  Restricting the search to those vertices keeps a
       sparse host (10^4 isolated vertices and a handful of edges is a valid
       input the m guard lets through) from costing quadratic memory; k <= 2m
       bounds it by the edge count instead. */
    std::vector<int> label(1, 0);  // core vertex -> host vertex, 1-indexed
    std::vector<int> core_of(g.n + 1, 0);
    for (int v = 1; v <= g.n; ++v) {
        if (g.adj[v].empty()) continue;
        label.push_back(v);
        core_of[v] = static_cast<int>(label.size()) - 1;
    }
    const int core_n = static_cast<int>(label.size()) - 1;
    if (core_n < 2) return;

    std::vector<std::pair<int, int>> core_edges;
    for (int v = 1; v <= g.n; ++v) {
        for (std::size_t i = 0; i < g.adj[v].size(); ++i) {
            const int w = g.adj[v][i];
            if (w > v) core_edges.push_back(
                std::make_pair(core_of[v], core_of[w]));
        }
    }
    const Graph core(core_n, core_edges);
    const std::vector<std::vector<char>> host =
        chordal_subgraph_host_matrix(core);

    ChordalSubgraphRelabel<Callback> relabel;
    relabel.label = &label;
    relabel.host_n = g.n;
    relabel.callback = &cb;

    KiyomiUnoChordalState root(core_n);
    for (int u = 1; u <= core_n; ++u) {
        for (int v = u + 1; v <= core_n; ++v) {
            if (!host[u][v]) continue;
            kiyomi_uno_add_isolated_edge(&root, u, v);
            chordal_subgraph_reverse_search_dfs(root, host, relabel);
            kiyomi_uno_remove_isolated_edge(&root, u, v);
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates every chordal subgraph of a given graph
 * @param g Host graph (1-indexed, vertex set {1, ..., n})
 * @param algo Algorithm selector; KIYOMI_UNO is the only algorithm
 * @return ChordalSubgraphEnumerationResult
 *
 * A chordal subgraph here is a spanning subgraph (V, E') with E' a subset of
 * E(g) that is chordal.  The vertex set is fixed and isolated vertices are
 * kept, so subgraphs are in bijection with the chordal edge subsets and the
 * empty edge set is always among them.  For g = K_n the output is exactly the
 * labeled chordal graphs on n vertices (chordal_labeled_enum.h).
 *
 * The count can reach 2^m (every subgraph of a forest is chordal), so prefer
 * enumerate_chordal_subgraphs_cb() when only aggregation is needed.
 */
inline ChordalSubgraphEnumerationResult enumerate_chordal_subgraphs(
    const Graph& g,
    ChordalSubgraphEnumAlgorithm algo =
        ChordalSubgraphEnumAlgorithm::KIYOMI_UNO) {
    (void)algo;
    ChordalSubgraphEnumerationResult result;
    detail::AppendToVector cb;
    cb.out = &result.graphs;
    detail::enumerate_chordal_subgraphs_kiyomi_uno_cb(g, cb);
    return result;
}

/**
 * @brief Streaming enumeration of every chordal subgraph of a given graph
 * @param g Host graph (1-indexed, vertex set {1, ..., n})
 * @param cb Callback invoked as cb(const EnumeratedGraph&) per subgraph
 * @param algo Algorithm selector; KIYOMI_UNO is the only algorithm
 *
 * Memory-friendly alternative to enumerate_chordal_subgraphs(): each subgraph
 * is handed to the callback as it is generated and never stored, so memory
 * stays O(min(n, 2m)^2) instead of O(number of subgraphs * n^2) -- the search
 * skips the vertices with no host edge, so an isolated-vertex-heavy host costs
 * no more than its edges do.  The paper's O(1) amortized/delay bounds use a
 * difference-output implementation; this API builds a complete edge list per
 * subgraph and favors simpler quadratic state.
 */
template <typename Callback>
inline void enumerate_chordal_subgraphs_cb(
    const Graph& g,
    Callback&& cb,
    ChordalSubgraphEnumAlgorithm algo =
        ChordalSubgraphEnumAlgorithm::KIYOMI_UNO) {
    (void)algo;
    detail::enumerate_chordal_subgraphs_kiyomi_uno_cb(g, cb);
}

}  // namespace graph_recognition

#endif
