#ifndef GRAPH_RECOGNITION_COCHAIN_H
#define GRAPH_RECOGNITION_COCHAIN_H

/**
 * @file cochain.h
 * @brief 余チェーングラフ (cochain graph) 認識
 *
 * アルゴリズム:
 *   - COMPLEMENT: 補グラフ構築 + チェーン判定 O(n²)
 *   - DIRECT: 補グラフ構築を回避した直接判定 O(n+m) (デフォルト)
 */

#include "bipartite.h"
#include "chain.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 余チェーングラフ認識アルゴリズムの選択
 */
enum class CochainAlgorithm {
    COMPLEMENT, /**< 補グラフのチェーン判定 O(n²) */
    DIRECT      /**< 補グラフ構築を回避した直接判定 O(n+m) (デフォルト) */
};

/**
 * @brief 余チェーングラフ認識の結果
 */
struct CochainResult {
    bool is_cochain; /**< 余チェーングラフであれば true */
};

namespace detail {

/** @brief 補グラフを構築する (内部関数) */
inline Graph build_complement_graph(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

/** @brief 補グラフ構築 + チェーン判定 (元のアルゴリズム) */
inline CochainResult check_cochain_complement(const Graph& g) {
    CochainResult res;
    res.is_cochain = false;

    Graph gc = build_complement_graph(g);
    ChainResult cres = check_chain(gc);
    if (!cres.is_chain) return res;

    res.is_cochain = true;
    return res;
}

/**
 * @brief 補グラフ構築を回避した直接判定 O(n+m)
 *
 * cochain ⟺ 補グラフが二部 + チェーン。
 * 補グラフの二部性 = co-bipartite = 頂点を 2 つのクリークに分割。
 * 補グラフのチェーン性 = 片側の「非隣接」が包含的 = 接尾辞性。
 *
 * 1. 補グラフ BFS で 2 彩色 (linked-list 技法で O(n+m))
 * 2. 各色クラスがクリークか検証 (辺数チェック)
 * 3. 片側を「反対側の非隣接数」昇順ソート → 接尾辞性検証
 */
inline CochainResult check_cochain_direct(const Graph& g) {
    CochainResult res;
    res.is_cochain = false;

    int n = g.n;
    if (n <= 1) { res.is_cochain = true; return res; }

    // 補グラフの BFS で 2 彩色 (linked-list で非隣接頂点を高速列挙)
    std::vector<int> color(n + 1, -1);
    // remaining: まだ色がついていない頂点のリスト (doubly-linked)
    std::vector<int> rem_prev(n + 2, 0), rem_next(n + 2, 0);
    // sentinel: 0
    rem_next[0] = 1;
    rem_prev[0] = 0;
    for (int v = 1; v <= n; ++v) {
        rem_next[v] = v + 1;
        rem_prev[v] = v - 1;
    }
    rem_next[n] = 0; // end sentinel
    rem_prev[0] = n; // not used but consistent

    // stamp 配列: 隣接判定用
    std::vector<unsigned char> stamped(n + 1, 0);

    // BFS queue
    std::vector<int> bfs_queue;
    bfs_queue.reserve(n);

    // 各連結成分を処理
    while (rem_next[0] != 0) {
        int start = rem_next[0];
        // start を remaining から除去
        rem_next[0] = rem_next[start];
        if (rem_next[start] != 0) rem_prev[rem_next[start]] = 0;

        color[start] = 0;
        bfs_queue.clear();
        bfs_queue.push_back(start);

        size_t qi = 0;
        while (qi < bfs_queue.size()) {
            int v = bfs_queue[qi++];

            // v の G 上の隣接頂点をスタンプ
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                stamped[g.adj[v][j]] = 1;
            }

            int new_color = 1 - color[v];

            // remaining リストを走査し、スタンプされていない頂点 = 補グラフの隣接
            int u = rem_next[0];
            while (u != 0) {
                int nxt = rem_next[u];
                if (!stamped[u]) {
                    // u は v と補グラフで隣接
                    if (color[u] == -1) {
                        color[u] = new_color;
                        bfs_queue.push_back(u);
                        // remaining から除去
                        rem_next[rem_prev[u]] = rem_next[u];
                        if (rem_next[u] != 0) rem_prev[rem_next[u]] = rem_prev[u];
                    } else if (color[u] != new_color) {
                        // 2 彩色不可能 → 補グラフは二部でない
                        return res;
                    }
                }
                u = nxt;
            }

            // スタンプ解除
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                stamped[g.adj[v][j]] = 0;
            }
        }
    }

    // 色クラスの分割
    std::vector<int> left, right;
    left.reserve(n);
    right.reserve(n);
    for (int v = 1; v <= n; ++v) {
        if (color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    // 各色クラスがクリークか検証 (= G 上の辺数チェック)
    // left のクリーク: left 内の全ペアが G の辺
    long long left_edges = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        for (size_t j = 0; j < g.adj[left[i]].size(); ++j) {
            int u = g.adj[left[i]][j];
            if (color[u] == 0) left_edges++;
        }
    }
    long long left_need = (long long)left.size() * ((long long)left.size() - 1);
    if (left_edges != left_need) return res; // 各辺が 2 回カウント

    long long right_edges = 0;
    for (size_t i = 0; i < right.size(); ++i) {
        for (size_t j = 0; j < g.adj[right[i]].size(); ++j) {
            int u = g.adj[right[i]][j];
            if (color[u] == 1) right_edges++;
        }
    }
    long long right_need = (long long)right.size() * ((long long)right.size() - 1);
    if (right_edges != right_need) return res;

    if (left.empty() || right.empty()) {
        res.is_cochain = true;
        return res;
    }

    // チェーン性検証 (補グラフ上): 片側の「非隣接数」で接尾辞性
    // L 側各頂点の R 側非隣接数 = |R| - (R 側隣接数)
    int left_size = (int)left.size();
    int right_size = (int)right.size();

    // L 側の rank を付与 (非隣接数昇順)
    std::vector<int> non_adj_r(n + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        int adj_r = 0;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            if (color[g.adj[v][j]] == 1) adj_r++;
        }
        non_adj_r[v] = right_size - adj_r;
    }

    // カウンティングソートで L 側を非隣接数昇順にソート
    int max_nonadj = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        if (non_adj_r[left[i]] > max_nonadj) max_nonadj = non_adj_r[left[i]];
    }
    std::vector<int> cnt(max_nonadj + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) cnt[non_adj_r[left[i]]]++;
    std::vector<int> sorted_left(left_size);
    std::vector<int> start_pos(max_nonadj + 1, 0);
    for (int k = 1; k <= max_nonadj; ++k) start_pos[k] = start_pos[k - 1] + cnt[k - 1];
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        sorted_left[start_pos[non_adj_r[v]]++] = v;
    }

    // rank[v] = sorted_left 内の位置
    std::vector<int> rank_l(n + 1, -1);
    for (int i = 0; i < left_size; ++i) {
        rank_l[sorted_left[i]] = i;
    }

    // R 側各頂点の L 側非隣接 (= 補グラフ隣接) の最小 rank と個数を検証
    for (size_t i = 0; i < right.size(); ++i) {
        int r = right[i];
        // R 側の r の L 側非隣接を見つける
        // r の G 上 L 側隣接をスタンプ
        for (size_t j = 0; j < g.adj[r].size(); ++j) {
            int u = g.adj[r][j];
            if (color[u] == 0) stamped[u] = 1;
        }
        int min_rank = left_size;
        int count_nonadj = 0;
        for (int li = 0; li < left_size; ++li) {
            int u = left[li];
            if (!stamped[u]) {
                count_nonadj++;
                if (rank_l[u] < min_rank) min_rank = rank_l[u];
            }
        }
        // スタンプ解除
        for (size_t j = 0; j < g.adj[r].size(); ++j) {
            int u = g.adj[r][j];
            if (color[u] == 0) stamped[u] = 0;
        }

        if (count_nonadj == 0) continue;
        // 接尾辞性: count_nonadj == left_size - min_rank
        if (count_nonadj != left_size - min_rank) return res;
    }

    res.is_cochain = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフが余チェーングラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DIRECT)
 * @return CochainResult
 *
 * G が余チェーングラフ ⟺ complement(G) がチェーングラフ。
 */
inline CochainResult check_cochain(const Graph& g,
    CochainAlgorithm algo = CochainAlgorithm::DIRECT) {
    switch (algo) {
        case CochainAlgorithm::COMPLEMENT:
            return detail::check_cochain_complement(g);
        case CochainAlgorithm::DIRECT:
            return detail::check_cochain_direct(g);
    }
    return CochainResult();
}

} // namespace graph_recognition

#endif
