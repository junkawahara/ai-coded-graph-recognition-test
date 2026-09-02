#ifndef GRAPH_RECOGNITION_STRONGLY_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_STRONGLY_CHORDAL_ENUM_H

/**
 * @file strongly_chordal_labeled_enum.h
 * @brief Strongly chordal graph enumeration by edge-addition reverse search
 *
 * Enumerates all labeled strongly chordal graphs on {1, ..., n}.  The default
 * is Kiyomi's strongly-chordal-specific subgraph enumeration with K_n as the
 * host graph.  Its root is the empty graph.  For a nonempty strongly chordal
 * graph H, compute a deterministic strong elimination ordering, take the first
 * non-isolated vertex v in that ordering and its first neighbor w, and define
 * parent(H) = H - vw.  Kiyomi's lemma proves that the same ordering remains a
 * strong elimination ordering after this deletion.
 *
 * The reverse search tries each missing edge e and descends exactly when H+e
 * is strongly chordal and its canonical parent edge is e.  Thus every search
 * node is strongly chordal; the algorithm does not enumerate and filter the
 * larger class of chordal graphs.
 *
 * Reference: M. Kiyomi, "Studies on Subgraph and Supergraph Enumeration
 * Algorithms," Ph.D. thesis, The Graduate University for Advanced Studies,
 * 2006, Section 4.1.3, Lemma 4.11 and Theorem 4.12.
 * M. Farber, "Characterizations of strongly chordal graphs," Discrete
 * Mathematics 43(2--3), 173--189, 1983.
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "recognizers/strongly_chordal.h"

namespace graph_recognition {

/** @brief Algorithm selection for strongly chordal graph enumeration */
enum class StronglyChordalLabeledEnumAlgorithm {
    KIYOMI_EDGE_ADDITION = 0, /**< Strongly-chordal-specific edge reverse search */
    REVERSE_SEARCH = KIYOMI_EDGE_ADDITION, /**< Backward-compatible name */
    LEGACY_CHORDAL_FILTER = 1 /**< Chordal vertex search plus property filtering */
};

/** @brief Result of strongly chordal graph enumeration */
struct StronglyChordalLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of labeled strongly chordal graphs */
};

namespace detail {

/** @brief Adjacency-matrix state for Kiyomi's edge-addition search */
struct KiyomiStronglyChordalState {
    int total_n;
    std::size_t edge_count;
    std::vector<int> degree;
    std::vector<std::vector<char>> adj;

    /* n + 1 in std::size_t: the public entry points take an int, and
       n = INT_MAX would overflow the addition before the allocation ever
       fails. */
    explicit KiyomiStronglyChordalState(int n)
        : total_n(n), edge_count(0),
          degree(static_cast<std::size_t>(n) + 1, 0),
          adj(static_cast<std::size_t>(n) + 1,
              std::vector<char>(static_cast<std::size_t>(n) + 1, 0)) {}
};

inline void strongly_chordal_add_edge(KiyomiStronglyChordalState* state,
                                      int u,
                                      int v) {
    state->adj[u][v] = 1;
    state->adj[v][u] = 1;
    ++state->degree[u];
    ++state->degree[v];
    ++state->edge_count;
}

inline void strongly_chordal_remove_edge(KiyomiStronglyChordalState* state,
                                         int u,
                                         int v) {
    state->adj[u][v] = 0;
    state->adj[v][u] = 0;
    --state->degree[u];
    --state->degree[v];
    --state->edge_count;
}

inline std::vector<std::pair<int, int>> collect_strongly_chordal_edges(
    const KiyomiStronglyChordalState& state) {
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
 * @brief Tests whether v is simple in the subgraph induced by alive vertices
 *
 * A vertex is simple iff the closed neighborhoods of the vertices in N[v]
 * are linearly ordered by inclusion.  Simpliciality makes N[v] a subset of
 * every neighbor's closed neighborhood, so it remains to sort the neighbors
 * by alive degree and test inclusion for consecutive pairs.
 */
inline bool is_simple_vertex(const KiyomiStronglyChordalState& state,
                             int v,
                             const std::vector<char>& alive,
                             const std::vector<int>& alive_degree,
                             std::vector<int>* neighbors) {
    neighbors->clear();
    for (int u = 1; u <= state.total_n; ++u) {
        if (alive[u] && state.adj[v][u]) neighbors->push_back(u);
    }

    for (std::size_t i = 0; i < neighbors->size(); ++i) {
        for (std::size_t j = i + 1; j < neighbors->size(); ++j) {
            if (!state.adj[(*neighbors)[i]][(*neighbors)[j]]) return false;
        }
    }

    std::sort(neighbors->begin(), neighbors->end(),
              [&](int a, int b) {
                  if (alive_degree[a] != alive_degree[b]) {
                      return alive_degree[a] < alive_degree[b];
                  }
                  return a < b;
              });

    for (std::size_t i = 0; i + 1 < neighbors->size(); ++i) {
        const int smaller = (*neighbors)[i];
        const int larger = (*neighbors)[i + 1];
        for (int w = 1; w <= state.total_n; ++w) {
            if (!alive[w] || w == larger) continue;
            if ((w == smaller || state.adj[smaller][w]) &&
                !state.adj[larger][w]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Tests strict closed-neighborhood inclusion in the alive subgraph
 */
inline bool is_strict_closed_neighborhood_subset(
    const KiyomiStronglyChordalState& state,
    int smaller,
    int larger,
    const std::vector<char>& alive,
    const std::vector<int>& alive_degree) {
    if (alive_degree[smaller] >= alive_degree[larger]) return false;
    for (int w = 1; w <= state.total_n; ++w) {
        if (!alive[w]) continue;
        const bool in_smaller =
            w == smaller || state.adj[smaller][w];
        const bool in_larger =
            w == larger || state.adj[larger][w];
        if (in_smaller && !in_larger) return false;
    }
    return true;
}

/**
 * @brief Computes a canonical strong elimination ordering (Farber)
 *
 * Farber's construction accumulates a partial order: at elimination step i,
 * x < y when that relation was already present or N_i[x] is a strict subset
 * of N_i[y].  A simple vertex that is minimal in this partial order is
 * removed; labels break any remaining tie.  Merely removing an arbitrary
 * simple vertex recognizes the class, but does not necessarily produce the
 * strong ordering required by Kiyomi's canonical parent lemma.
 */
inline bool canonical_strong_elimination_order(
    const KiyomiStronglyChordalState& state,
    std::vector<int>* order) {
    order->clear();
    order->reserve(static_cast<std::size_t>(state.total_n));

    std::vector<char> alive(state.total_n + 1, 1);
    std::vector<int> alive_degree = state.degree;
    std::vector<std::vector<char>> less(
        state.total_n + 1, std::vector<char>(state.total_n + 1, 0));
    std::vector<int> neighbors;
    neighbors.reserve(static_cast<std::size_t>(state.total_n));

    for (int remaining = state.total_n; remaining > 0; --remaining) {
        for (int x = 1; x <= state.total_n; ++x) {
            if (!alive[x]) continue;
            for (int y = 1; y <= state.total_n; ++y) {
                if (!alive[y] || x == y) continue;
                if (is_strict_closed_neighborhood_subset(
                        state, x, y, alive, alive_degree)) {
                    less[x][y] = 1;
                }
            }
        }

        // Keep the strict relation transitively closed. Farber proves that
        // adding the current neighborhood inclusions preserves a partial
        // order; the closure makes the implementation independent of how
        // those relations happen to be discovered.
        for (int k = 1; k <= state.total_n; ++k) {
            if (!alive[k]) continue;
            for (int x = 1; x <= state.total_n; ++x) {
                if (!alive[x] || !less[x][k]) continue;
                for (int y = 1; y <= state.total_n; ++y) {
                    if (alive[y] && less[k][y]) less[x][y] = 1;
                }
            }
        }

        int pick = 0;
        for (int v = 1; v <= state.total_n; ++v) {
            if (!alive[v]) continue;
            bool minimal = true;
            for (int u = 1; u <= state.total_n; ++u) {
                if (alive[u] && less[u][v]) {
                    minimal = false;
                    break;
                }
            }
            if (!minimal) continue;
            if (is_simple_vertex(state, v, alive, alive_degree, &neighbors)) {
                pick = v;
                break;
            }
        }
        if (pick == 0) {
            order->clear();
            return false;
        }

        order->push_back(pick);
        alive[pick] = 0;
        for (int u = 1; u <= state.total_n; ++u) {
            if (alive[u] && state.adj[pick][u]) --alive_degree[u];
        }
    }
    return true;
}

/**
 * @brief Returns the edge removed by the canonical Kiyomi parent operation
 *
 * (0,0) denotes either the empty root or a graph that is not strongly
 * chordal.  For a valid nonempty state the returned edge always exists and
 * deleting it preserves the computed strong elimination ordering.
 */
inline std::pair<int, int> strongly_chordal_parent_edge(
    const KiyomiStronglyChordalState& state) {
    if (state.edge_count == 0) return std::make_pair(0, 0);

    std::vector<int> order;
    if (!canonical_strong_elimination_order(state, &order)) {
        return std::make_pair(0, 0);
    }

    int first = 0;
    for (std::size_t i = 0; i < order.size(); ++i) {
        if (state.degree[order[i]] > 0) {
            first = order[i];
            break;
        }
    }
    if (first == 0) return std::make_pair(0, 0);

    int neighbor = 0;
    for (std::size_t i = 0; i < order.size(); ++i) {
        if (state.adj[first][order[i]]) {
            neighbor = order[i];
            break;
        }
    }
    if (neighbor == 0) return std::make_pair(0, 0);
    return first < neighbor ? std::make_pair(first, neighbor)
                            : std::make_pair(neighbor, first);
}

template <typename Callback>
inline void kiyomi_strongly_chordal_dfs(KiyomiStronglyChordalState& state,
                                        Callback& cb) {
    {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_strongly_chordal_edges(state);
        cb(graph);
    }

    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.adj[u][v]) continue;

            strongly_chordal_add_edge(&state, u, v);
            if (strongly_chordal_parent_edge(state) ==
                std::make_pair(u, v)) {
                kiyomi_strongly_chordal_dfs(state, cb);
            }
            strongly_chordal_remove_edge(&state, u, v);
        }
    }
}

template <typename Callback>
inline void enumerate_strongly_chordal_labeled_graphs_kiyomi_cb(int n,
                                                        Callback& cb) {
    KiyomiStronglyChordalState root(n);
    kiyomi_strongly_chordal_dfs(root, cb);
}

/** @brief Build a Graph from the legacy chordal search state */
inline Graph strongly_chordal_state_to_graph(const ChordalLabeledEnumState& state) {
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
inline void strongly_chordal_legacy_dfs(const ChordalLabeledEnumState& state,
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
        const Graph graph = strongly_chordal_state_to_graph(children[i]);
        if (!check_strongly_chordal(graph).is_strongly_chordal) continue;
        strongly_chordal_legacy_dfs(children[i], cb);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled strongly chordal graphs on {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selector; KIYOMI_EDGE_ADDITION is the default
 */
inline StronglyChordalLabeledEnumerationResult
enumerate_strongly_chordal_labeled_graphs_reverse_search(
    int n,
    StronglyChordalLabeledEnumAlgorithm algo =
        StronglyChordalLabeledEnumAlgorithm::KIYOMI_EDGE_ADDITION) {
    StronglyChordalLabeledEnumerationResult result;
    if (n < 0) return result;

    detail::AppendToVector cb;
    cb.out = &result.graphs;
    if (algo == StronglyChordalLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalLabeledEnumState root(n);
        detail::strongly_chordal_legacy_dfs(root, cb);
    } else {
        detail::enumerate_strongly_chordal_labeled_graphs_kiyomi_cb(n, cb);
    }
    return result;
}

/**
 * @brief Streaming enumeration of labeled strongly chordal graphs
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const EnumeratedGraph&) for every graph
 * @param algo Enumeration algorithm; KIYOMI_EDGE_ADDITION is the default
 *
 * With the default KIYOMI_EDGE_ADDITION the callback API keeps only the
 * O(n^2) search state instead of retaining all output graphs.  This
 * implementation recomputes the straightforward O(n^4) Farber partial-order
 * construction for each candidate edge; Kiyomi's sharper bound assumes the
 * O(min(m log n, n^2)) strong-ordering algorithm cited in the thesis.
 *
 * LEGACY_CHORDAL_FILTER streams its output through the same callback but does
 * not keep to that space bound: every recursion level materializes the whole
 * child list of the chordal vertex-addition search (an adjacency matrix per
 * child, and up to 2^k cliques of a k-vertex node), so it is exponential in
 * temporary space.  It exists for differential testing, not for large n.
 */
template <typename Callback>
inline void enumerate_strongly_chordal_labeled_graphs_reverse_search_cb(
    int n,
    Callback&& cb,
    StronglyChordalLabeledEnumAlgorithm algo =
        StronglyChordalLabeledEnumAlgorithm::KIYOMI_EDGE_ADDITION) {
    if (n < 0) return;
    if (algo == StronglyChordalLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER) {
        detail::ChordalLabeledEnumState root(n);
        detail::strongly_chordal_legacy_dfs(root, cb);
    } else {
        detail::enumerate_strongly_chordal_labeled_graphs_kiyomi_cb(n, cb);
    }
}

}  // namespace graph_recognition

#endif
