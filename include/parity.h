#ifndef GRAPH_RECOGNITION_PARITY_H
#define GRAPH_RECOGNITION_PARITY_H

/**
 * @file parity.h
 * @brief パリティグラフ認識
 *
 * パリティグラフとは、任意の 2 頂点間の全ての誘導パスが同じ偶奇性
 * (全て偶数長 or 全て奇数長) を持つグラフである。
 *
 * Distance-hereditary グラフの上位クラス。
 * 同値な特性化: split decomposition の全 prime 成分が完全グラフ
 * または完全二部グラフ。
 *
 * アルゴリズム:
 *   - DIRECT_CHECK: 全頂点ペアの BFS 距離と誘導パス偶奇性を
 *     DFS バックトラッキングで検証。小さい n に実用的。
 *
 * 参考文献:
 *   - Burlet, Uhry, "Parity graphs," Annals of Discrete Math., 1984
 *   - Bouchet, "Reducing prime graphs and recognizing circle graphs,"
 *     Combinatorica, 1987
 */

#include "graph.h"

#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief パリティグラフ認識アルゴリズムの選択
 */
enum class ParityAlgorithm {
    DIRECT_CHECK /**< 直接定義検査 (BFS + バックトラッキング) */
};

/**
 * @brief パリティグラフ認識の結果
 */
struct ParityResult {
    bool is_parity = false; /**< パリティグラフであれば true */
};

namespace detail_parity {

/**
 * @brief 誘導パス探索の内部状態
 *
 * blocked[w] = パス上の頂点のうち w に隣接する頂点の数。
 * 頂点 w をパス末尾に追加可能な条件: blocked[w] == 1
 * (現在の端点のみに隣接)。
 */
struct ParityCheckState {
    const Graph& g;
    std::vector<int> blocked;
    std::vector<unsigned char> in_path;
    int target_v;
    int target_parity;
    bool found;

    ParityCheckState(const Graph& g_, int n, int v, int tp)
        : g(g_), blocked(n + 1, 0), in_path(n + 1, 0),
          target_v(v), target_parity(tp), found(false) {}
};

/**
 * @brief 誘導パスの DFS バックトラッキング
 *
 * cur から target_v への誘導パスを探索し、target_parity と
 * 異なる偶奇性のパスが見つかれば found = true にする。
 */
inline void parity_dfs(ParityCheckState& state, int cur, int depth) {
    if (state.found) return;
    for (size_t i = 0; i < state.g.adj[cur].size(); ++i) {
        if (state.found) return;
        int w = state.g.adj[cur][i];
        if (state.in_path[w]) continue;

        if (w == state.target_v) {
            if (state.blocked[w] == 1) {
                if ((depth + 1) % 2 != state.target_parity) {
                    state.found = true;
                    return;
                }
            }
            continue;
        }

        if (state.blocked[w] != 1) continue;

        state.in_path[w] = 1;
        for (size_t j = 0; j < state.g.adj[w].size(); ++j) {
            state.blocked[state.g.adj[w][j]]++;
        }

        parity_dfs(state, w, depth + 1);

        for (size_t j = 0; j < state.g.adj[w].size(); ++j) {
            state.blocked[state.g.adj[w][j]]--;
        }
        state.in_path[w] = 0;
    }
}

/**
 * @brief u から v への、target_parity と異なる偶奇性の誘導パスが存在するか
 */
inline bool has_induced_path_diff_parity(const Graph& g, int u, int v,
                                         int target_parity) {
    ParityCheckState state(g, g.n, v, target_parity);
    state.in_path[u] = 1;
    for (size_t i = 0; i < g.adj[u].size(); ++i) {
        state.blocked[g.adj[u][i]]++;
    }
    parity_dfs(state, u, 0);
    return state.found;
}

}  // namespace detail_parity

/**
 * @brief パリティグラフ認識 (直接定義検査)
 *
 * 全頂点ペア (u,v) について BFS 距離 d(u,v) を計算し、
 * d(u,v) と異なる偶奇性の誘導 u-v パスが存在しないことを検証する。
 */
inline ParityResult check_parity_direct(const Graph& g) {
    ParityResult res;
    res.is_parity = true;
    int n = g.n;
    if (n <= 2) return res;

    // BFS で全ペアの距離を計算
    std::vector<std::vector<int>> dist(n + 1, std::vector<int>(n + 1, -1));
    for (int s = 1; s <= n; ++s) {
        dist[s][s] = 0;
        std::queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (dist[s][w] == -1) {
                    dist[s][w] = dist[s][u] + 1;
                    q.push(w);
                }
            }
        }
    }

    // 各ペアで誘導パスの偶奇性を検証
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (dist[u][v] == -1) continue;  // 異なる連結成分
            int target_parity = dist[u][v] % 2;
            if (detail_parity::has_induced_path_diff_parity(g, u, v,
                                                            target_parity)) {
                res.is_parity = false;
                return res;
            }
        }
    }
    return res;
}

/**
 * @brief パリティグラフ認識 (デフォルト)
 */
inline ParityResult check_parity(
    const Graph& g,
    ParityAlgorithm algo = ParityAlgorithm::DIRECT_CHECK) {
    (void)algo;
    return check_parity_direct(g);
}

}  // namespace graph_recognition

#endif
