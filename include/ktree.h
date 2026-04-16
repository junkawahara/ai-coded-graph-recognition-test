#ifndef GRAPH_RECOGNITION_KTREE_H
#define GRAPH_RECOGNITION_KTREE_H

/**
 * @file ktree.h
 * @brief k-木 (k-tree) 認識
 *
 * k-木は弦グラフの一種で、K_{k+1} から始めて各ステップで
 * k-クリークに隣接する新頂点を追加して構成される。
 * 最小次数 k の simplicial 頂点を反復的に除去して判定する。
 */

#include "graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief k-木認識アルゴリズムの選択
 */
enum class KTreeAlgorithm {
    SIMPLICIAL_REMOVAL /**< simplicial 頂点除去 */
};

/**
 * @brief k-木認識の結果
 */
struct KTreeResult {
    bool is_ktree = false; /**< k-木であれば true */
    int k = -1;            /**< k の値 */
};

/**
 * @brief グラフが k-木か判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: SIMPLICIAL_REMOVAL)
 * @return KTreeResult
 *
 * k-木 ⟺ 連結かつ弦グラフで全極大クリークのサイズが k+1。
 * 等価的に: 最小次数 k の simplicial 頂点を反復除去して K_{k+1} に到達可能。
 */
inline KTreeResult check_ktree(const Graph& g,
    KTreeAlgorithm algo = KTreeAlgorithm::SIMPLICIAL_REMOVAL) {
    (void)algo;
    KTreeResult res;

    int n = g.n;

    /* 空グラフ: 0-木 */
    if (n == 0) {
        res.is_ktree = true;
        res.k = 0;
        return res;
    }

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

    /* 辺数計算 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    /* 最小次数を k の候補とする */
    int k_cand = n;
    for (int v = 1; v <= n; ++v) {
        int d = (int)g.adj[v].size();
        if (d < k_cand) k_cand = d;
    }

    /* n <= k_cand の場合: 完全グラフなら (n-1)-木 */
    if (n <= k_cand + 1) {
        if (m == (long long)n * (n - 1) / 2) {
            res.is_ktree = true;
            res.k = n - 1;
        }
        return res;
    }

    /* 辺数チェック: m = k*n - k*(k+1)/2 */
    long long expected_m = (long long)k_cand * n - (long long)k_cand * (k_cand + 1) / 2;
    if (m != expected_m) return res;

    /* 隣接行列 (動的) */
    std::vector<std::vector<char>> adj_mat(n + 1, std::vector<char>(n + 1, 0));
    std::vector<int> deg(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        deg[v] = (int)g.adj[v].size();
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            adj_mat[v][g.adj[v][i]] = 1;
        }
    }

    std::vector<char> alive(n + 1, 1);

    /* 次数 k_cand の頂点を集めるキュー */
    std::vector<int> cand;
    for (int v = 1; v <= n; ++v) {
        if (deg[v] == k_cand) cand.push_back(v);
    }

    int removed = 0;

    int target_remain = k_cand + 1;

    while (!cand.empty() && removed < n - target_remain) {
        int v = cand.back();
        cand.pop_back();
        if (!alive[v] || deg[v] != k_cand) continue;

        /* N(v) がクリークか確認 */
        std::vector<int> nbrs;
        for (int u = 1; u <= n; ++u) {
            if (alive[u] && adj_mat[v][u]) nbrs.push_back(u);
        }
        if ((int)nbrs.size() != k_cand) continue;

        bool is_clique = true;
        for (size_t i = 0; i < nbrs.size() && is_clique; ++i) {
            for (size_t j = i + 1; j < nbrs.size() && is_clique; ++j) {
                if (!adj_mat[nbrs[i]][nbrs[j]]) is_clique = false;
            }
        }
        if (!is_clique) return res;

        /* v を除去 */
        alive[v] = 0;
        for (size_t i = 0; i < nbrs.size(); ++i) {
            adj_mat[v][nbrs[i]] = 0;
            adj_mat[nbrs[i]][v] = 0;
            --deg[nbrs[i]];
            if (deg[nbrs[i]] == k_cand) cand.push_back(nbrs[i]);
        }
        deg[v] = 0;
        ++removed;
    }

    /* 残りの頂点が K_{k+1} であること */
    if (removed != n - (k_cand + 1)) return res;

    int remain_count = 0;
    for (int v = 1; v <= n; ++v) {
        if (alive[v]) {
            if (deg[v] != k_cand) return res;
            ++remain_count;
        }
    }
    if (remain_count != k_cand + 1) return res;

    res.is_ktree = true;
    res.k = k_cand;
    return res;
}

} // namespace graph_recognition

#endif
