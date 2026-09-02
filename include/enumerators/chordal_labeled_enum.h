#ifndef GRAPH_RECOGNITION_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_CHORDAL_ENUM_H

/**
 * @file chordal_labeled_enum.h
 * @brief Chordal graph enumeration (Kiyomi--Uno reverse search)
 *
 * Enumerates all labeled chordal graphs on vertex set {1, ..., n}
 * using the chordal-graph-specific reverse search of Kiyomi and Uno.
 *
 * Reference: M. Kiyomi and T. Uno, "Generating Chordal Graphs Included
 * in Given Graphs," IEICE Trans. Inf. & Syst. E89-D(2), 763--770, 2006.
 * DOI: 10.1093/ietisy/e89-d.2.763
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for chordal graph enumeration
 */
enum class ChordalLabeledEnumAlgorithm {
    KIYOMI_UNO = 0, /**< Kiyomi--Uno chordal-subgraph reverse search */
    REVERSE_SEARCH = KIYOMI_UNO, /**< Backward-compatible name for KIYOMI_UNO */
    LEGACY_VERTEX_REVERSE_SEARCH = 1 /**< Previous largest-label vertex search */
};

/**
 * @brief Enumerated labeled graph
 */
struct EnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int>> edges;       /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of chordal graph enumeration
 */
struct ChordalLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs;          /**< array of enumerated chordal graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct ChordalLabeledEnumState {
    int total_n;
    int alive_count;
    std::vector<char> alive;
    std::vector<std::vector<char>> adj;

    explicit ChordalLabeledEnumState(int n)
        : total_n(n), alive_count(0), alive(n + 1, 0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

inline bool is_simplicial(const ChordalLabeledEnumState& state, int v) {
    std::vector<int> neighbors;
    for (int u = 1; u <= state.total_n; ++u) {
        if (state.alive[u] && state.adj[v][u]) neighbors.push_back(u);
    }
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        for (std::size_t j = i + 1; j < neighbors.size(); ++j) {
            int a = neighbors[i];
            int b = neighbors[j];
            if (!state.adj[a][b]) return false;
        }
    }
    return true;
}

inline int canonical_removed_vertex(const ChordalLabeledEnumState& state) {
    int best = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (!state.alive[v]) continue;
        if (!is_simplicial(state, v)) continue;
        best = v;
    }
    return best;
}

inline void remove_vertex(ChordalLabeledEnumState* state, int v) {
    if (!state->alive[v]) return;
    state->alive[v] = 0;
    --state->alive_count;
    for (int u = 1; u <= state->total_n; ++u) {
        state->adj[v][u] = 0;
        state->adj[u][v] = 0;
    }
}

inline bool parent_state(const ChordalLabeledEnumState& state, ChordalLabeledEnumState* parent) {
    if (state.alive_count == 0) return false;
    int removed = canonical_removed_vertex(state);
    if (removed == 0) return false;
    *parent = state;
    remove_vertex(parent, removed);
    return true;
}

inline bool same_state(const ChordalLabeledEnumState& a, const ChordalLabeledEnumState& b) {
    if (a.total_n != b.total_n) return false;
    if (a.alive_count != b.alive_count) return false;
    for (int v = 1; v <= a.total_n; ++v) {
        if (a.alive[v] != b.alive[v]) return false;
    }
    for (int u = 1; u <= a.total_n; ++u) {
        for (int v = u + 1; v <= a.total_n; ++v) {
            if (!a.alive[u] || !a.alive[v]) continue;
            if (a.adj[u][v] != b.adj[u][v]) return false;
        }
    }
    return true;
}

inline void enumerate_cliques_dfs(const ChordalLabeledEnumState& state,
                                  const std::vector<int>& vertices,
                                  std::size_t idx,
                                  std::vector<int>* current,
                                  std::vector<std::vector<int>>* out) {
    if (idx == vertices.size()) {
        out->push_back(*current);
        return;
    }

    enumerate_cliques_dfs(state, vertices, idx + 1, current, out);

    int v = vertices[idx];
    bool ok = true;
    for (std::size_t i = 0; i < current->size(); ++i) {
        if (!state.adj[v][(*current)[i]]) {
            ok = false;
            break;
        }
    }
    if (!ok) return;
    current->push_back(v);
    enumerate_cliques_dfs(state, vertices, idx + 1, current, out);
    current->pop_back();
}

inline std::vector<std::vector<int>> enumerate_all_cliques(const ChordalLabeledEnumState& state) {
    std::vector<int> vertices;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) vertices.push_back(v);
    }
    std::vector<std::vector<int>> cliques;
    std::vector<int> current;
    enumerate_cliques_dfs(state, vertices, 0, &current, &cliques);
    return cliques;
}

inline ChordalLabeledEnumState add_vertex_with_clique_neighborhood(const ChordalLabeledEnumState& state,
                                                            int x,
                                                            const std::vector<int>& clique) {
    ChordalLabeledEnumState child = state;
    if (child.alive[x]) return child;

    child.alive[x] = 1;
    ++child.alive_count;
    for (int u = 1; u <= child.total_n; ++u) {
        child.adj[x][u] = 0;
        child.adj[u][x] = 0;
    }
    for (std::size_t i = 0; i < clique.size(); ++i) {
        int u = clique[i];
        child.adj[x][u] = 1;
        child.adj[u][x] = 1;
    }
    return child;
}

/**
 * @brief Generates child states (for subclass enumeration)
 *
 * Since subclass enumeration (e.g., ptolemaic_labeled_enum) requires additional property checks,
 * this provides a version that explicitly generates child states.
 */
inline void collect_children_reverse_search(const ChordalLabeledEnumState& state,
                                            std::vector<ChordalLabeledEnumState>* children) {
    children->clear();

    std::vector<int> missing;
    for (int x = 1; x <= state.total_n; ++x) {
        if (!state.alive[x]) missing.push_back(x);
    }

    std::vector<std::vector<int>> cliques = enumerate_all_cliques(state);
    for (std::size_t i = 0; i < missing.size(); ++i) {
        int x = missing[i];
        for (std::size_t j = 0; j < cliques.size(); ++j) {
            ChordalLabeledEnumState child = add_vertex_with_clique_neighborhood(state, x, cliques[j]);
            // Optimized check: canonical_removed_vertex(child) == x
            // is equivalent to parent_state(child) == state
            int best = canonical_removed_vertex(child);
            if (best == x) {
                children->push_back(child);
            }
        }
    }
}

inline std::vector<std::pair<int, int>> collect_edges(const ChordalLabeledEnumState& state) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= state.total_n; ++u) {
        if (!state.alive[u]) continue;
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (!state.alive[v]) continue;
            if (state.adj[u][v]) {
                edges.push_back(std::make_pair(u, v));
            }
        }
    }
    return edges;
}

/**
 * @brief In-place reverse search DFS (avoids O(n^2) copies), streaming version
 *
 * Modifies the state in-place during recursion and restores it on backtrack.
 * Only recurses when canonical_removed_vertex(child) == x
 * (equivalent to parent(child) == state).
 * Each complete graph is handed to the callback and never stored.
 */
template <typename Callback>
inline void reverse_search_dfs_cb(ChordalLabeledEnumState& state, Callback& cb) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        cb(graph);
        return;
    }

    std::vector<int> missing;
    for (int x = 1; x <= state.total_n; ++x) {
        if (!state.alive[x]) missing.push_back(x);
    }

    std::vector<std::vector<int>> cliques = enumerate_all_cliques(state);

    for (std::size_t i = 0; i < missing.size(); ++i) {
        int x = missing[i];
        for (std::size_t j = 0; j < cliques.size(); ++j) {
            const std::vector<int>& clique = cliques[j];

            // Add x in-place
            state.alive[x] = 1;
            ++state.alive_count;
            for (std::size_t ci = 0; ci < clique.size(); ++ci) {
                state.adj[x][clique[ci]] = 1;
                state.adj[clique[ci]][x] = 1;
            }

            // Check if x is the canonical removed vertex (equivalent to parent == state)
            int best = canonical_removed_vertex(state);
            if (best == x) {
                reverse_search_dfs_cb(state, cb);
            }

            // Undo: remove x
            for (std::size_t ci = 0; ci < clique.size(); ++ci) {
                state.adj[x][clique[ci]] = 0;
                state.adj[clique[ci]][x] = 0;
            }
            state.alive[x] = 0;
            --state.alive_count;
        }
    }
}

/**
 * @brief State of the Kiyomi--Uno chordal-subgraph reverse search
 *
 * Only vertices incident with a current edge are marked alive.  The other
 * vertices are the isolated vertices of the fixed vertex set {1, ..., n} and
 * are represented implicitly.  This is the representation used by the
 * Kiyomi--Uno search tree: its roots are the one-edge graphs.
 */
struct KiyomiUnoChordalState {
    int total_n;
    int alive_count;
    int edge_count;
    std::vector<char> alive;
    std::vector<char> simplicial;
    std::vector<int> degree;
    std::vector<int> peo;
    std::vector<std::vector<char>> adj;

    explicit KiyomiUnoChordalState(int n)
        : total_n(n), alive_count(0), edge_count(0), alive(n + 1, 0),
          simplicial(n + 1, 0), degree(n + 1, 0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/** @brief Information about the minimum-degree simplicial vertices of a node */
struct KiyomiUnoNodeInfo {
    int k;
    int min_s_star;
    int min_s_k_plus_one;
    std::vector<char> in_s_star;

    explicit KiyomiUnoNodeInfo(int n)
        : k(n + 1), min_s_star(n + 1), min_s_k_plus_one(n + 1),
          in_s_star(n + 1, 0) {}
};

inline KiyomiUnoNodeInfo kiyomi_uno_node_info(
    const KiyomiUnoChordalState& state) {
    KiyomiUnoNodeInfo info(state.total_n);
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v] && state.simplicial[v] &&
            state.degree[v] < info.k) {
            info.k = state.degree[v];
        }
    }
    for (int v = 1; v <= state.total_n; ++v) {
        if (!state.alive[v] || !state.simplicial[v]) continue;
        if (state.degree[v] == info.k) {
            info.in_s_star[v] = 1;
            if (v < info.min_s_star) info.min_s_star = v;
        } else if (state.degree[v] == info.k + 1 &&
                   v < info.min_s_k_plus_one) {
            info.min_s_k_plus_one = v;
        }
    }
    return info;
}

inline std::vector<std::pair<int, int>> collect_kiyomi_uno_edges(
    const KiyomiUnoChordalState& state) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(state.edge_count));
    for (int u = 1; u <= state.total_n; ++u) {
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (state.adj[u][v]) edges.push_back(std::make_pair(u, v));
        }
    }
    return edges;
}

inline bool kiyomi_uno_is_child_clique(
    const KiyomiUnoChordalState& state,
    const KiyomiUnoNodeInfo& info,
    int new_vertex,
    const std::vector<int>& clique,
    const std::vector<char>& in_clique) {
    const int size = static_cast<int>(clique.size());
    if (size < info.k) return true;

    if (size == info.k) {
        int min_not_in_clique = state.total_n + 1;
        for (int u = 1; u <= state.total_n; ++u) {
            if (info.in_s_star[u] && !in_clique[u] &&
                u < min_not_in_clique) {
                min_not_in_clique = u;
            }
        }
        return new_vertex < min_not_in_clique;
    }

    if (size != info.k + 1) return false;
    for (int u = 1; u <= state.total_n; ++u) {
        if (info.in_s_star[u] && !in_clique[u]) return false;
    }
    int min_simplicial = info.min_s_star;
    if (info.min_s_k_plus_one < min_simplicial) {
        min_simplicial = info.min_s_k_plus_one;
    }
    return new_vertex < min_simplicial;
}

/**
 * @brief Enumerates nonempty cliques up to max_size from a maintained PEO
 *
 * Every clique has a unique first vertex in a PEO.  Its remaining vertices
 * form a subset of that vertex's later neighbors, which are themselves a
 * clique.  The callback therefore sees each eligible clique exactly once.
 */
template <typename CliqueCallback>
inline void kiyomi_uno_subset_dfs(const std::vector<int>& later_neighbors,
                                  std::size_t index,
                                  int max_size,
                                  std::vector<int>* clique,
                                  CliqueCallback& cb) {
    if (index == later_neighbors.size()) {
        cb(*clique);
        return;
    }

    kiyomi_uno_subset_dfs(later_neighbors, index + 1, max_size, clique, cb);
    if (static_cast<int>(clique->size()) >= max_size) return;
    clique->push_back(later_neighbors[index]);
    kiyomi_uno_subset_dfs(later_neighbors, index + 1, max_size, clique, cb);
    clique->pop_back();
}

template <typename CliqueCallback>
inline void enumerate_kiyomi_uno_cliques(const KiyomiUnoChordalState& state,
                                         int max_size,
                                         CliqueCallback& cb) {
    for (std::size_t i = 0; i < state.peo.size(); ++i) {
        const int first = state.peo[i];
        std::vector<int> later_neighbors;
        for (std::size_t j = i + 1; j < state.peo.size(); ++j) {
            const int u = state.peo[j];
            if (state.adj[first][u]) later_neighbors.push_back(u);
        }
        std::vector<int> clique(1, first);
        kiyomi_uno_subset_dfs(later_neighbors, 0, max_size, &clique, cb);
    }
}

inline void kiyomi_uno_add_vertex(KiyomiUnoChordalState* state,
                                  int v,
                                  const std::vector<int>& clique,
                                  const std::vector<char>& in_clique,
                                  std::vector<int>* lost_simplicial) {
    lost_simplicial->clear();
    for (std::size_t i = 0; i < clique.size(); ++i) {
        const int u = clique[i];
        if (!state->simplicial[u]) continue;
        for (int w = 1; w <= state->total_n; ++w) {
            if (state->alive[w] && state->adj[u][w] && !in_clique[w]) {
                state->simplicial[u] = 0;
                lost_simplicial->push_back(u);
                break;
            }
        }
    }

    state->alive[v] = 1;
    state->simplicial[v] = 1;
    state->degree[v] = static_cast<int>(clique.size());
    ++state->alive_count;
    for (std::size_t i = 0; i < clique.size(); ++i) {
        const int u = clique[i];
        state->adj[v][u] = 1;
        state->adj[u][v] = 1;
        ++state->degree[u];
        ++state->edge_count;
    }
    state->peo.insert(state->peo.begin(), v);
}

inline void kiyomi_uno_remove_vertex(KiyomiUnoChordalState* state,
                                     int v,
                                     const std::vector<int>& clique,
                                     const std::vector<int>& lost_simplicial) {
    state->peo.erase(state->peo.begin());
    for (std::size_t i = 0; i < clique.size(); ++i) {
        const int u = clique[i];
        state->adj[v][u] = 0;
        state->adj[u][v] = 0;
        --state->degree[u];
        --state->edge_count;
    }
    state->alive[v] = 0;
    state->simplicial[v] = 0;
    state->degree[v] = 0;
    --state->alive_count;
    for (std::size_t i = 0; i < lost_simplicial.size(); ++i) {
        state->simplicial[lost_simplicial[i]] = 1;
    }
}

inline void kiyomi_uno_add_isolated_edge(KiyomiUnoChordalState* state,
                                         int v,
                                         int w) {
    state->alive[v] = 1;
    state->alive[w] = 1;
    state->simplicial[v] = 1;
    state->simplicial[w] = 1;
    state->degree[v] = 1;
    state->degree[w] = 1;
    state->adj[v][w] = 1;
    state->adj[w][v] = 1;
    state->alive_count += 2;
    ++state->edge_count;
    state->peo.insert(state->peo.begin(), w);
    state->peo.insert(state->peo.begin(), v);
}

inline void kiyomi_uno_remove_isolated_edge(KiyomiUnoChordalState* state,
                                            int v,
                                            int w) {
    state->peo.erase(state->peo.begin(), state->peo.begin() + 2);
    state->alive[v] = 0;
    state->alive[w] = 0;
    state->simplicial[v] = 0;
    state->simplicial[w] = 0;
    state->degree[v] = 0;
    state->degree[w] = 0;
    state->adj[v][w] = 0;
    state->adj[w][v] = 0;
    state->alive_count -= 2;
    --state->edge_count;
}

template <typename Callback>
inline void kiyomi_uno_reverse_search_dfs(KiyomiUnoChordalState& state,
                                          Callback& cb);

template <typename Callback>
struct KiyomiUnoExistingCliqueChildren {
    KiyomiUnoChordalState* state;
    const KiyomiUnoNodeInfo* info;
    Callback* callback;

    void operator()(const std::vector<int>& clique) {
        std::vector<char> in_clique(state->total_n + 1, 0);
        for (std::size_t i = 0; i < clique.size(); ++i) {
            in_clique[clique[i]] = 1;
        }

        for (int v = 1; v <= state->total_n; ++v) {
            if (state->alive[v]) continue;
            if (!kiyomi_uno_is_child_clique(
                    *state, *info, v, clique, in_clique)) {
                continue;
            }

            std::vector<int> lost_simplicial;
            kiyomi_uno_add_vertex(
                state, v, clique, in_clique, &lost_simplicial);
            kiyomi_uno_reverse_search_dfs(*state, *callback);
            kiyomi_uno_remove_vertex(
                state, v, clique, lost_simplicial);
        }
    }
};

template <typename Callback>
inline void kiyomi_uno_reverse_search_dfs(KiyomiUnoChordalState& state,
                                          Callback& cb) {
    EnumeratedGraph graph;
    graph.n = state.total_n;
    graph.edges = collect_kiyomi_uno_edges(state);
    cb(graph);

    const KiyomiUnoNodeInfo info = kiyomi_uno_node_info(state);

    KiyomiUnoExistingCliqueChildren<Callback> existing_children;
    existing_children.state = &state;
    existing_children.info = &info;
    existing_children.callback = &cb;
    enumerate_kiyomi_uno_cliques(state, info.k + 1, existing_children);

    // In a disconnected graph, C may also be a singleton unused vertex.
    // The ordering v < w removes the duplicate representations
    // G(v,{w}) == G(w,{v}).
    const std::vector<int> singleton_clique(1, 0);
    const std::vector<char> empty_membership(state.total_n + 1, 0);
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) continue;
        for (int w = v + 1; w <= state.total_n; ++w) {
            if (state.alive[w]) continue;
            if (!kiyomi_uno_is_child_clique(
                    state, info, v, singleton_clique, empty_membership)) {
                continue;
            }
            kiyomi_uno_add_isolated_edge(&state, v, w);
            kiyomi_uno_reverse_search_dfs(state, cb);
            kiyomi_uno_remove_isolated_edge(&state, v, w);
        }
    }
}

template <typename Callback>
inline void enumerate_chordal_labeled_graphs_kiyomi_uno_cb(int n, Callback& cb) {
    EnumeratedGraph empty;
    empty.n = n;
    cb(empty);
    if (n < 2) return;

    KiyomiUnoChordalState root(n);
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            root.alive[u] = 1;
            root.alive[v] = 1;
            root.simplicial[u] = 1;
            root.simplicial[v] = 1;
            root.degree[u] = 1;
            root.degree[v] = 1;
            root.adj[u][v] = 1;
            root.adj[v][u] = 1;
            root.alive_count = 2;
            root.edge_count = 1;
            root.peo.push_back(u);
            root.peo.push_back(v);
            kiyomi_uno_reverse_search_dfs(root, cb);

            root.alive[u] = 0;
            root.alive[v] = 0;
            root.simplicial[u] = 0;
            root.simplicial[v] = 0;
            root.degree[u] = 0;
            root.degree[v] = 0;
            root.adj[u][v] = 0;
            root.adj[v][u] = 0;
            root.alive_count = 0;
            root.edge_count = 0;
            root.peo.clear();
        }
    }
}

/** @brief Callback that appends every graph to a vector (materializing API) */
struct AppendToVector {
    std::vector<EnumeratedGraph>* out;
    void operator()(const EnumeratedGraph& g) { out->push_back(g); }
};

/** @brief In-place reverse search DFS, materializing version */
inline void reverse_search_dfs(ChordalLabeledEnumState& state,
                               std::vector<EnumeratedGraph>* out) {
    AppendToVector cb;
    cb.out = out;
    reverse_search_dfs_cb(state, cb);
}

}  // namespace detail

/**
 * @brief Enumerates all labeled chordal graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selector; KIYOMI_UNO is the default
 * @return ChordalLabeledEnumerationResult
 *
 * The default follows Kiyomi and Uno: parent(G) removes a minimum-degree
 * simplicial vertex, breaking ties by the smallest label.  The legacy
 * largest-label vertex search remains selectable for compatibility with
 * enumerators that use its internal state.
 */
inline ChordalLabeledEnumerationResult enumerate_chordal_labeled_graphs_reverse_search(int n,
    ChordalLabeledEnumAlgorithm algo = ChordalLabeledEnumAlgorithm::KIYOMI_UNO) {
    ChordalLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::AppendToVector cb;
    cb.out = &result.graphs;
    if (algo == ChordalLabeledEnumAlgorithm::LEGACY_VERTEX_REVERSE_SEARCH) {
        detail::ChordalLabeledEnumState root(n);
        detail::reverse_search_dfs_cb(root, cb);
    } else {
        detail::enumerate_chordal_labeled_graphs_kiyomi_uno_cb(n, cb);
    }
    return result;
}

/**
 * @brief Streaming enumeration of all labeled chordal graphs on {1, ..., n}
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const EnumeratedGraph&) for each graph
 * @param algo Enumeration algorithm; KIYOMI_UNO is the default
 *
 * Memory-friendly alternative to enumerate_chordal_labeled_graphs_reverse_search():
 * each graph is handed to the callback as it is generated and never stored,
 * so memory stays O(n^2) instead of O(number of graphs * n^2). The number of labeled
 * chordal graphs grows super-exponentially (n = 7: 617675, n = 8: about
 * 3.1e7), so prefer this API when only aggregation (counting, filtering,
 * writing to a stream) is needed.  The paper's O(1) amortized/delay bounds use
 * a difference-output implementation; this API constructs a complete edge
 * list for every graph and this implementation favors simpler O(n^2) state.
 */
template <typename Callback>
inline void enumerate_chordal_labeled_graphs_reverse_search_cb(
    int n,
    Callback&& cb,
    ChordalLabeledEnumAlgorithm algo = ChordalLabeledEnumAlgorithm::KIYOMI_UNO) {
    if (n < 0) return;
    if (algo == ChordalLabeledEnumAlgorithm::LEGACY_VERTEX_REVERSE_SEARCH) {
        detail::ChordalLabeledEnumState root(n);
        detail::reverse_search_dfs_cb(root, cb);
    } else {
        detail::enumerate_chordal_labeled_graphs_kiyomi_uno_cb(n, cb);
    }
}

}  // namespace graph_recognition

#endif
