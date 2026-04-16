#ifndef GRAPH_RECOGNITION_LAMAN_H
#define GRAPH_RECOGNITION_LAMAN_H

/**
 * @file laman.h
 * @brief Laman グラフ認識
 *
 * Laman グラフは 2 次元における最小剛性グラフであり、
 * (2,3)-tight: m = 2n - 3 かつ任意の部分集合 S (|S| >= 2) で
 * edges(S) <= 2|S| - 3 を満たす。
 *
 * アルゴリズム: pebble game O(n^2)
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Laman グラフ認識アルゴリズムの選択
 */
enum class LamanAlgorithm {
    PEBBLE_GAME /**< ペブルゲーム O(n^2) */
};

/**
 * @brief Laman グラフ認識の結果
 */
struct LamanResult {
    bool is_laman = false; /**< Laman グラフであれば true */
};

namespace detail {

/**
 * @brief ペブルゲームによる (2,3)-sparsity チェック
 *
 * 各頂点に 2 個のペブルを配置し、各辺を追加する際に
 * 端点から到達可能なペブルを 3 個確保できるか検査する。
 */
inline bool laman_pebble_game(const Graph& g) {
    int n = g.n;
    std::vector<int> pebbles(n + 1, 2);
    /* out[v] = v から向けた有向辺の先 */
    std::vector<std::vector<int>> out(n + 1);

    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (v <= u) continue; /* 各辺を一度だけ処理 */

            /* u, v から到達可能なペブルが 3 個になるまで探索 */
            int need = 3 - pebbles[u] - pebbles[v];
            for (int attempts = 0; attempts < need; ++attempts) {
                /* BFS で u または v からペブルを持つ頂点を探す */
                bool found = false;
                for (int start_idx = 0; start_idx < 2 && !found; ++start_idx) {
                    int start = (start_idx == 0) ? u : v;
                    std::vector<int> bfs_queue;
                    std::vector<int> parent(n + 1, -1);
                    parent[start] = start;
                    bfs_queue.push_back(start);

                    for (size_t qi = 0; qi < bfs_queue.size() && !found; ++qi) {
                        int w = bfs_queue[qi];
                        if (w != u && w != v && pebbles[w] > 0) {
                            /* ペブルを start まで移動: パスを逆転 */
                            int cur = w;
                            pebbles[w]--;
                            while (cur != start) {
                                int p = parent[cur];
                                /* p→cur の辺を cur→p に逆転 */
                                /* out[p] から cur を除去し out[cur] に p を追加 */
                                std::vector<int>& po = out[p];
                                for (size_t k = 0; k < po.size(); ++k) {
                                    if (po[k] == cur) {
                                        po[k] = po.back();
                                        po.pop_back();
                                        break;
                                    }
                                }
                                out[cur].push_back(p);
                                cur = p;
                            }
                            pebbles[start]++;
                            found = true;
                        }
                        if (!found) {
                            for (size_t j = 0; j < out[w].size(); ++j) {
                                int x = out[w][j];
                                if (parent[x] == -1) {
                                    parent[x] = w;
                                    bfs_queue.push_back(x);
                                }
                            }
                        }
                    }
                }
                if (!found) return false; /* sparsity 違反 */
            }

            /* 辺 (u, v) を向き付け */
            if (pebbles[u] > 0) {
                out[u].push_back(v);
                pebbles[u]--;
            } else {
                out[v].push_back(u);
                pebbles[v]--;
            }
        }
    }
    return true;
}

} // namespace detail

/**
 * @brief グラフが Laman グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: PEBBLE_GAME)
 * @return LamanResult
 *
 * Laman グラフ ⟺ (2,3)-tight: m = 2n - 3 かつ (2,3)-sparse。
 */
inline LamanResult check_laman(const Graph& g,
    LamanAlgorithm algo = LamanAlgorithm::PEBBLE_GAME) {
    (void)algo;
    LamanResult res;

    int n = g.n;
    if (n <= 1) return res; /* n=0: 辺なし, n=1: m=0 ≠ 2*1-3=-1 */

    /* 辺数チェック: m = 2n - 3 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 2LL * n - 3) return res;

    /* 連結性チェック */
    std::vector<char> visited(n + 1, 0);
    std::vector<int> queue;
    queue.push_back(1);
    visited[1] = 1;
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int v = queue[qi];
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (!visited[u]) {
                visited[u] = 1;
                queue.push_back(u);
            }
        }
    }
    if ((int)queue.size() != n) return res;

    /* ペブルゲームで (2,3)-sparsity チェック */
    if (!detail::laman_pebble_game(g)) return res;

    res.is_laman = true;
    return res;
}

} // namespace graph_recognition

#endif
