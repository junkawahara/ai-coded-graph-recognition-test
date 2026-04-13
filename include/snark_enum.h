#ifndef GRAPH_RECOGNITION_SNARK_ENUM_H
#define GRAPH_RECOGNITION_SNARK_ENUM_H

/**
 * @file snark_enum.h
 * @brief スナークグラフの列挙 (逆探索)
 *
 * 頂点集合 {1, ..., n} 上のラベル付きスナークを全列挙する。
 *
 * スナーク: 巡回4辺連結 (cyclically 4-edge-connected) な三次グラフで
 * girth >= 5 かつ辺彩色数 (chromatic index) が 4 のもの。
 * 最小のスナークは Petersen グラフ (10 頂点)。
 * n が奇数、n < 10 の場合、スナークは存在しない。
 *
 * アルゴリズム:
 *   頂点を 1, 2, ..., n の順に追加。各頂点 x の追加時に
 *   {1,...,x-1} の中で deg < 3 の頂点から近傍を選択。
 *   次数上限・到達可能性で枝刈りし、辺追加時に girth >= 5 を
 *   BFS で増分検査。全頂点追加後に橋なし・巡回4辺連結・
 *   非3辺彩色可能を確認して出力。
 *
 * 参考文献:
 *   Brinkmann, Goedgebeur, Hägglund, Markström,
 *   "Generation and properties of snarks,"
 *   J. Combin. Theory Ser. B 103, 2013
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

enum class SnarkEnumAlgorithm {
    REVERSE_SEARCH
};

struct SnarkEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

struct SnarkEnumState {
    int total_n;
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;
    int edge_count;

    explicit SnarkEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0), edge_count(0) {}
};

/**
 * @brief 辺 (x, v) を追加すると長さ 4 以下の閉路が生じるか BFS で検査
 *
 * 追加前に呼び出す。x から v への長さ <= 3 のパスが既存辺で
 * 存在すれば、辺追加により girth < 5 の閉路が生じる。
 */
inline bool snark_has_short_cycle(const SnarkEnumState& state,
                                   int x, int v) {
    int max_v = (x > state.alive_count) ? x : state.alive_count;
    std::vector<int> dist(max_v + 1, -1);
    dist[x] = 0;
    std::vector<int> queue;
    queue.push_back(x);
    for (std::size_t qi = 0; qi < queue.size(); ++qi) {
        int u = queue[qi];
        if (dist[u] >= 3) break;
        for (int w = 1; w <= max_v; ++w) {
            if (state.adj[u][w] && dist[w] == -1) {
                dist[w] = dist[u] + 1;
                if (w == v) return true;
                if (dist[w] < 3) queue.push_back(w);
            }
        }
    }
    return false;
}

/**
 * @brief 橋なし (bridgeless) 検査 — Tarjan の橋検出
 */
inline bool snark_is_bridgeless(const SnarkEnumState& state) {
    int n = state.total_n;
    /* 隣接リスト構築 (辺ID付き) */
    std::vector<std::vector<std::pair<int,int> > > adj_list(n + 1);
    int eid = 0;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v)
            if (state.adj[u][v]) {
                adj_list[u].push_back(std::make_pair(v, eid));
                adj_list[v].push_back(std::make_pair(u, eid));
                eid++;
            }

    std::vector<int> tin(n + 1, 0), low(n + 1, 0);
    int timer = 0;

    struct Frame {
        int v;
        int parent_edge;
        std::size_t i;
    };
    std::vector<Frame> stack;
    timer++;
    tin[1] = low[1] = timer;
    Frame f0;
    f0.v = 1; f0.parent_edge = -1; f0.i = 0;
    stack.push_back(f0);

    while (!stack.empty()) {
        Frame& f = stack.back();
        if (f.i < adj_list[f.v].size()) {
            int u = adj_list[f.v][f.i].first;
            int ei = adj_list[f.v][f.i].second;
            f.i++;
            if (ei == f.parent_edge) continue;
            if (tin[u] == 0) {
                timer++;
                tin[u] = low[u] = timer;
                Frame fn;
                fn.v = u; fn.parent_edge = ei; fn.i = 0;
                stack.push_back(fn);
            } else {
                low[f.v] = (low[f.v] < tin[u]) ? low[f.v] : tin[u];
            }
        } else {
            int v = f.v;
            stack.pop_back();
            if (!stack.empty()) {
                Frame& par = stack.back();
                low[par.v] = (low[par.v] < low[v]) ? low[par.v] : low[v];
                if (low[v] > tin[par.v]) return false; /* 橋発見 */
            }
        }
    }
    /* 連結性確認 */
    for (int v = 1; v <= n; ++v)
        if (tin[v] == 0) return false;
    return true;
}

/**
 * @brief 巡回4辺連結 (cyclically 4-edge-connected) 検査
 *
 * 全頂点部分集合 S (2 <= |S| <= n-2) について交差辺数を計算。
 * 交差辺数 < 4 の S が存在すれば非巡回4辺連結。
 * 三次グラフでは交差辺 = 3|S| - 2|E(G[S])|。
 */
inline bool snark_is_cyc4ec(const SnarkEnumState& state) {
    int n = state.total_n;
    int full = (1 << n) - 1;
    for (int mask = 1; mask <= full; ++mask) {
        int sz = 0;
        { int tmp = mask; while (tmp) { sz += tmp & 1; tmp >>= 1; } }
        if (sz < 2 || sz > n - 2) continue;
        /* 対称性で |S| <= n/2 のみ検査 */
        if (sz > n / 2) continue;

        int internal_edges = 0;
        for (int u = 1; u <= n; ++u) {
            if (!((mask >> (u - 1)) & 1)) continue;
            for (int v = u + 1; v <= n; ++v) {
                if (!((mask >> (v - 1)) & 1)) continue;
                if (state.adj[u][v]) internal_edges++;
            }
        }
        int cross = 3 * sz - 2 * internal_edges;
        if (cross < 4) return false;
    }
    return true;
}

/**
 * @brief 辺3彩色のバックトラッキング
 */
inline bool snark_try_3_edge_color(
    const std::vector<std::pair<int,int> >& edges,
    std::vector<int>& vc,
    int idx, int m) {
    if (idx == m) return true;
    int u = edges[idx].first;
    int v = edges[idx].second;
    for (int c = 0; c < 3; ++c) {
        if ((vc[u] & (1 << c)) || (vc[v] & (1 << c))) continue;
        vc[u] |= (1 << c);
        vc[v] |= (1 << c);
        if (snark_try_3_edge_color(edges, vc, idx + 1, m)) return true;
        vc[u] &= ~(1 << c);
        vc[v] &= ~(1 << c);
    }
    return false;
}

/**
 * @brief 3辺彩色可能か検査
 * @return true なら彩色可能 (スナークではない)
 */
inline bool snark_is_3_edge_colorable(const SnarkEnumState& state) {
    int n = state.total_n;
    std::vector<std::pair<int,int> > edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v)
            if (state.adj[u][v])
                edges.push_back(std::make_pair(u, v));
    int m = (int)edges.size();
    std::vector<int> vc(n + 1, 0);
    return snark_try_3_edge_color(edges, vc, 0, m);
}

/**
 * @brief 部分グラフの連結成分数を計算
 */
inline int snark_count_components(const SnarkEnumState& state, int n_verts) {
    std::vector<int> comp(n_verts + 1, -1);
    int num_comp = 0;
    for (int s = 1; s <= n_verts; ++s) {
        if (comp[s] != -1) continue;
        comp[s] = num_comp;
        std::vector<int> queue;
        queue.push_back(s);
        for (std::size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (int w = 1; w <= n_verts; ++w) {
                if (state.adj[u][w] && comp[w] == -1) {
                    comp[w] = num_comp;
                    queue.push_back(w);
                }
            }
        }
        num_comp++;
    }
    return num_comp;
}

/**
 * @brief 連結性チェック (全頂点追加後)
 */
inline bool snark_is_connected(const SnarkEnumState& state) {
    int n = state.total_n;
    std::vector<char> visited(n + 1, 0);
    std::vector<int> queue;
    queue.push_back(1);
    visited[1] = 1;
    int cnt = 1;
    for (std::size_t qi = 0; qi < queue.size(); ++qi) {
        int u = queue[qi];
        for (int w = 1; w <= n; ++w) {
            if (state.adj[u][w] && !visited[w]) {
                visited[w] = 1;
                queue.push_back(w);
                cnt++;
            }
        }
    }
    return cnt == n;
}

/* forward declaration */
inline void snark_enum_choose(SnarkEnumState& state,
                               const std::vector<int>& available,
                               std::size_t start,
                               int chosen_count,
                               int min_size, int max_size,
                               std::vector<EnumeratedGraph>* out);

inline void snark_enum_dfs(SnarkEnumState& state,
                            std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        /* 全次数 == 3 確認 */
        for (int v = 1; v <= state.total_n; ++v)
            if (state.deg[v] != 3) return;

        /* 連結性検査 (安価、先に実行) */
        if (!snark_is_connected(state)) return;

        /* 橋なし検査 */
        if (!snark_is_bridgeless(state)) return;

        /* 巡回4辺連結検査 */
        if (!snark_is_cyc4ec(state)) return;

        /* 非3辺彩色検査 (彩色可能ならスナークでない) */
        if (snark_is_3_edge_colorable(state)) return;

        /* Accept */
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int remaining = state.total_n - x;

    std::vector<int> available;
    for (int v = 1; v < x; ++v)
        if (state.deg[v] < 3)
            available.push_back(v);

    int min_neighbors = 3 - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    int max_neighbors = 3;
    if (max_neighbors > (int)available.size())
        max_neighbors = (int)available.size();

    if (max_neighbors < min_neighbors) return;

    /* 辺数実現可能性枝刈り */
    int target_edges = 3 * state.total_n / 2;
    int max_future_edges = 3 * (state.total_n - x);
    if (state.edge_count + max_neighbors + max_future_edges < target_edges)
        return;

    /* 連結性枝刈り: 連結成分数 > 残り頂点数 + 1 なら接続不可能 */
    if (x >= 4) {
        int comp = snark_count_components(state, x - 1);
        if (comp > remaining + 1) return;
    }

    snark_enum_choose(state, available, 0, 0,
                       min_neighbors, max_neighbors, out);
}

inline void snark_enum_choose(SnarkEnumState& state,
                               const std::vector<int>& available,
                               std::size_t start,
                               int chosen_count,
                               int min_size, int max_size,
                               std::vector<EnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_n - x;

    if (chosen_count >= min_size) {
        state.deg[x] = chosen_count;
        state.alive_count = x;

        bool feasible = true;
        for (int v = 1; v <= x; ++v) {
            if (state.deg[v] + remaining_after_x < 3) {
                feasible = false;
                break;
            }
        }

        if (feasible) {
            snark_enum_dfs(state, out);
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        if (state.deg[v] >= 3) continue;

        /* Girth >= 5 枝刈り: 辺 (x, v) が長さ <= 4 の閉路を生じるか */
        if (snark_has_short_cycle(state, x, v)) continue;

        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;
        state.edge_count++;

        snark_enum_choose(state, available, i + 1, chosen_count + 1,
                           min_size, max_size, out);

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
        state.edge_count--;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付きスナークを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return SnarkEnumerationResult
 */
inline SnarkEnumerationResult
enumerate_snark_graphs(int n,
    SnarkEnumAlgorithm algo = SnarkEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    SnarkEnumerationResult result;
    if (n <= 0) return result;
    if (n % 2 != 0) return result;
    if (n < 10) return result;

    detail::SnarkEnumState root(n);
    detail::snark_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
