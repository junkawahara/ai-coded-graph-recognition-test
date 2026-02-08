#ifndef GRAPH_RECOGNITION_MINOR_H
#define GRAPH_RECOGNITION_MINOR_H

/**
 * @file minor.h
 * @brief 固定 forbidden minor 検出ユーティリティ
 *
 * 辺削除 / 辺縮約の再帰により、固定小グラフ minor の存在を判定する。
 */

#include "graph.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace graph_recognition {
namespace detail_minor {

/** @brief minor 判定対象の小グラフ種別 */
enum class MinorTarget {
    K4,
    K5,
    K23,
    K33,
};

/** @brief minor 探索用の 0-indexed 単純グラフ状態 */
struct MinorState {
    int n;
    int m;
    std::vector<std::vector<unsigned char>> adj;
    std::vector<int> deg;
};

/** @brief Graph から MinorState を構築する */
inline MinorState build_minor_state(const Graph& g) {
    MinorState st;
    st.n = g.n;
    st.adj.assign(st.n, std::vector<unsigned char>(st.n, 0));
    st.deg.assign(st.n, 0);
    st.m = 0;

    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u >= v) continue;
            int a = u - 1;
            int b = v - 1;
            if (st.adj[a][b]) continue;
            st.adj[a][b] = 1;
            st.adj[b][a] = 1;
            st.deg[a]++;
            st.deg[b]++;
            st.m++;
        }
    }

    return st;
}

/** @brief 辺削除した状態を返す */
inline MinorState delete_edge(const MinorState& st, int u, int v) {
    MinorState next = st;
    if (!next.adj[u][v]) return next;
    next.adj[u][v] = 0;
    next.adj[v][u] = 0;
    next.deg[u]--;
    next.deg[v]--;
    next.m--;
    return next;
}

/** @brief 辺 (u,v) を縮約した状態を返す */
inline MinorState contract_edge(const MinorState& st, int u, int v) {
    if (u > v) std::swap(u, v);

    MinorState next;
    next.n = st.n - 1;
    next.adj.assign(next.n, std::vector<unsigned char>(next.n, 0));
    next.deg.assign(next.n, 0);
    next.m = 0;

    std::vector<int> map_old(st.n, -1);
    int id = 0;
    for (int x = 0; x < st.n; ++x) {
        if (x == v) continue;
        map_old[x] = id++;
    }
    map_old[v] = map_old[u];

    for (int a = 0; a < st.n; ++a) {
        for (int b = a + 1; b < st.n; ++b) {
            if (!st.adj[a][b]) continue;
            int na = map_old[a];
            int nb = map_old[b];
            if (na == nb) continue;
            if (next.adj[na][nb]) continue;
            next.adj[na][nb] = 1;
            next.adj[nb][na] = 1;
            next.deg[na]++;
            next.deg[nb]++;
            next.m++;
        }
    }

    return next;
}

/** @brief 既存辺の中から分岐用の 1 本を選ぶ */
inline bool choose_edge(const MinorState& st, int* u, int* v) {
    int best_u = -1;
    int best_v = -1;
    int best_score = -1;

    for (int a = 0; a < st.n; ++a) {
        if (st.deg[a] == 0) continue;
        for (int b = a + 1; b < st.n; ++b) {
            if (!st.adj[a][b]) continue;
            int score = st.deg[a] + st.deg[b];
            if (score > best_score) {
                best_score = score;
                best_u = a;
                best_v = b;
            }
        }
    }

    if (best_u == -1) return false;
    *u = best_u;
    *v = best_v;
    return true;
}

/** @brief 状態のシリアライズ (メモ化キー) */
inline std::string serialize(const MinorState& st) {
    std::string key;
    key.reserve(2 + (size_t)st.n * (size_t)(st.n - 1) / 2);
    key.push_back((char)(st.n & 0xFF));
    key.push_back((char)((st.n >> 8) & 0xFF));
    for (int a = 0; a < st.n; ++a) {
        for (int b = a + 1; b < st.n; ++b) {
            key.push_back(st.adj[a][b] ? '\1' : '\0');
        }
    }
    return key;
}

inline bool clique_dfs(
    const MinorState& st,
    int k,
    int start,
    std::vector<int>* chosen) {
    if ((int)chosen->size() == k) return true;

    int need = k - (int)chosen->size();
    for (int v = start; v <= st.n - need; ++v) {
        if (st.deg[v] < k - 1) continue;

        bool ok = true;
        for (size_t i = 0; i < chosen->size(); ++i) {
            if (!st.adj[v][(*chosen)[i]]) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        chosen->push_back(v);
        if (clique_dfs(st, k, v + 1, chosen)) return true;
        chosen->pop_back();
    }

    return false;
}

/** @brief K_k を部分グラフとして含むか (非誘導) */
inline bool has_clique_k(const MinorState& st, int k) {
    if (st.n < k) return false;
    std::vector<int> chosen;
    chosen.reserve(k);
    return clique_dfs(st, k, 0, &chosen);
}

inline bool bipartite_complete_dfs(
    const MinorState& st,
    int a_size,
    int b_size,
    int start,
    std::vector<int>* a_set,
    std::vector<unsigned char>* in_a) {
    if ((int)a_set->size() == a_size) {
        int cnt = 0;
        for (int v = 0; v < st.n; ++v) {
            if ((*in_a)[v]) continue;
            bool ok = true;
            for (size_t i = 0; i < a_set->size(); ++i) {
                if (!st.adj[v][(*a_set)[i]]) {
                    ok = false;
                    break;
                }
            }
            if (ok) cnt++;
            if (cnt >= b_size) return true;
        }
        return false;
    }

    int need = a_size - (int)a_set->size();
    for (int v = start; v <= st.n - need; ++v) {
        if (st.deg[v] < b_size) continue;
        (*a_set).push_back(v);
        (*in_a)[v] = 1;
        if (bipartite_complete_dfs(st, a_size, b_size, v + 1, a_set, in_a)) {
            return true;
        }
        (*in_a)[v] = 0;
        (*a_set).pop_back();
    }

    return false;
}

/** @brief K_{a,b} を部分グラフとして含むか (非誘導) */
inline bool has_complete_bipartite(const MinorState& st, int a_size, int b_size) {
    if (st.n < a_size + b_size) return false;

    std::vector<int> a_set;
    a_set.reserve(a_size);
    std::vector<unsigned char> in_a(st.n, 0);

    return bipartite_complete_dfs(st, a_size, b_size, 0, &a_set, &in_a);
}

/** @brief 固定小グラフ minor 判定器 */
class MinorChecker {
public:
    explicit MinorChecker(MinorTarget target) : target_(target) {}

    bool has_minor(const MinorState& st) {
        return dfs(st);
    }

private:
    MinorTarget target_;
    std::unordered_map<std::string, unsigned char> memo_;

    int min_vertices() const {
        if (target_ == MinorTarget::K4) return 4;
        if (target_ == MinorTarget::K5) return 5;
        if (target_ == MinorTarget::K23) return 5;
        return 6; // K33
    }

    int min_edges() const {
        if (target_ == MinorTarget::K4) return 6;
        if (target_ == MinorTarget::K5) return 10;
        if (target_ == MinorTarget::K23) return 6;
        return 9; // K33
    }

    bool contains_target_subgraph(const MinorState& st) const {
        if (target_ == MinorTarget::K4) return has_clique_k(st, 4);
        if (target_ == MinorTarget::K5) return has_clique_k(st, 5);
        if (target_ == MinorTarget::K23) return has_complete_bipartite(st, 2, 3);
        return has_complete_bipartite(st, 3, 3);
    }

    bool dfs(const MinorState& st) {
        if (st.n < min_vertices()) return false;
        if (st.m < min_edges()) return false;

        if (contains_target_subgraph(st)) return true;

        int u = -1, v = -1;
        if (!choose_edge(st, &u, &v)) return false;

        std::string key = serialize(st);
        std::unordered_map<std::string, unsigned char>::const_iterator it =
            memo_.find(key);
        if (it != memo_.end()) return it->second != 0;

        MinorState contracted = contract_edge(st, u, v);
        if (dfs(contracted)) {
            memo_[key] = 1;
            return true;
        }

        MinorState deleted = delete_edge(st, u, v);
        if (dfs(deleted)) {
            memo_[key] = 1;
            return true;
        }

        memo_[key] = 0;
        return false;
    }
};

} // namespace detail_minor
} // namespace graph_recognition

#endif
