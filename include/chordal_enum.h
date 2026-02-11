#ifndef GRAPH_RECOGNITION_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_CHORDAL_ENUM_H

/**
 * @file chordal_enum.h
 * @brief 弦グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き弦グラフを全列挙する。
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 弦グラフ列挙アルゴリズムの選択
 */
enum class ChordalEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief 列挙されたラベル付きグラフ
 */
struct EnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int>> edges;       /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief 弦グラフ列挙の結果
 */
struct ChordalEnumerationResult {
    std::vector<EnumeratedGraph> graphs;          /**< 列挙された弦グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct ChordalEnumState {
    int total_n;
    int alive_count;
    std::vector<char> alive;
    std::vector<std::vector<char>> adj;

    explicit ChordalEnumState(int n)
        : total_n(n), alive_count(0), alive(n + 1, 0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

inline bool is_simplicial(const ChordalEnumState& state, int v) {
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

inline int canonical_removed_vertex(const ChordalEnumState& state) {
    int best = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (!state.alive[v]) continue;
        if (!is_simplicial(state, v)) continue;
        best = v;
    }
    return best;
}

inline void remove_vertex(ChordalEnumState* state, int v) {
    if (!state->alive[v]) return;
    state->alive[v] = 0;
    --state->alive_count;
    for (int u = 1; u <= state->total_n; ++u) {
        state->adj[v][u] = 0;
        state->adj[u][v] = 0;
    }
}

inline bool parent_state(const ChordalEnumState& state, ChordalEnumState* parent) {
    if (state.alive_count == 0) return false;
    int removed = canonical_removed_vertex(state);
    if (removed == 0) return false;
    *parent = state;
    remove_vertex(parent, removed);
    return true;
}

inline bool same_state(const ChordalEnumState& a, const ChordalEnumState& b) {
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

inline void enumerate_cliques_dfs(const ChordalEnumState& state,
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

inline std::vector<std::vector<int>> enumerate_all_cliques(const ChordalEnumState& state) {
    std::vector<int> vertices;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) vertices.push_back(v);
    }
    std::vector<std::vector<int>> cliques;
    std::vector<int> current;
    enumerate_cliques_dfs(state, vertices, 0, &current, &cliques);
    return cliques;
}

inline ChordalEnumState add_vertex_with_clique_neighborhood(const ChordalEnumState& state,
                                                            int x,
                                                            const std::vector<int>& clique) {
    ChordalEnumState child = state;
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

inline void collect_children_reverse_search(const ChordalEnumState& state,
                                            std::vector<ChordalEnumState>* children) {
    children->clear();

    std::vector<int> missing;
    for (int x = 1; x <= state.total_n; ++x) {
        if (!state.alive[x]) missing.push_back(x);
    }

    std::vector<std::vector<int>> cliques = enumerate_all_cliques(state);
    for (std::size_t i = 0; i < missing.size(); ++i) {
        int x = missing[i];
        for (std::size_t j = 0; j < cliques.size(); ++j) {
            ChordalEnumState child = add_vertex_with_clique_neighborhood(state, x, cliques[j]);
            ChordalEnumState parent(child.total_n);
            if (!parent_state(child, &parent)) continue;
            if (same_state(parent, state)) {
                children->push_back(child);
            }
        }
    }
}

inline std::vector<std::pair<int, int>> collect_edges(const ChordalEnumState& state) {
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

inline void reverse_search_dfs(const ChordalEnumState& state,
                               std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    std::vector<ChordalEnumState> children;
    collect_children_reverse_search(state, &children);
    for (std::size_t i = 0; i < children.size(); ++i) {
        reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き弦グラフを全列挙する
 * @param n 頂点数
 * @return ChordalEnumerationResult
 *
 * 逆探索 (reverse search) を使用。parent(G) は G から最大ラベルの
 * simplicial 頂点を除去して得られる。
 */
inline ChordalEnumerationResult enumerate_chordal_graphs_reverse_search(int n,
    ChordalEnumAlgorithm algo = ChordalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    ChordalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
