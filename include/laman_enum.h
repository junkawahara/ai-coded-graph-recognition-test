#ifndef GRAPH_RECOGNITION_LAMAN_ENUM_H
#define GRAPH_RECOGNITION_LAMAN_ENUM_H

/**
 * @file laman_enum.h
 * @brief Laman グラフ (最小剛性グラフ) の列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き Laman グラフを全列挙する。
 *
 * Laman グラフ: n 頂点 2n-3 辺で、任意の k 頂点部分グラフが
 * 2k-3 辺以下 ((2,3)-tight)。2次元最小剛性グラフと一致。
 *
 * (2,3)-sparsity は遺伝的性質のため、各頂点追加後に
 * 部分集合検査で枝刈りする。
 * tightness (辺数 = 2n-3) はリーフでのみ検査する。
 *
 * 枝刈り:
 *   1. 辺数上限: edge_count > 2x-3 なら即座に枝刈り
 *   2. 辺数到達可能性: edge_count + max_future < 2n-3 なら枝刈り
 *   3. 次数下限: n >= 3 のとき全頂点 deg >= 2 が必要
 *   4. (2,3)-sparsity: 増分部分集合検査で全部分グラフを検証
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Laman 列挙アルゴリズムの選択
 */
enum class LamanEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Laman 列挙の結果
 */
struct LamanEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Laman グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct LamanEnumState {
    int total_n;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;  /**< 各頂点の次数 */
    int edge_count;        /**< 現在の辺数 */

    explicit LamanEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0), edge_count(0) {}
};

/**
 * @brief {1, ..., x} 上の部分グラフが (2,3)-sparse か判定 (増分部分集合検査)
 *
 * (2,3)-sparse: 任意の k 頂点部分グラフ (k >= 2) が 2k-3 辺以下。
 *
 * 増分検査の最適化: {1,...,x-1} 上の部分グラフは既に検査済みのため、
 * 頂点 x を含む部分集合のみ検査する (2^(x-1) 通り)。
 *
 * @param state 列挙状態
 * @param x 判定対象の頂点数 ({1, ..., x})
 * @return (2,3)-sparse なら true
 */
inline bool is_23_sparse(const LamanEnumState& state, int x) {
    if (x <= 2) return true;

    // 頂点 x を含む部分集合のみ検査
    // sub は {1,...,x-1} の部分集��を表すビットマスク (sub >= 1)
    unsigned int x_minus_1 = static_cast<unsigned int>(x - 1);
    for (unsigned int sub = 1; sub < (1u << x_minus_1); ++sub) {
        // k = |sub| + 1 (頂点 x を含む)
        int k_minus_1 = 0;
        {
            unsigned int tmp = sub;
            while (tmp) { tmp &= (tmp - 1); ++k_minus_1; }
        }
        int k = k_minus_1 + 1;
        int limit = 2 * k - 3;

        // 部分集合内の辺数を数える
        int edges = 0;
        bool exceeded = false;

        // 頂点 x からの辺
        for (int i = 0; i < static_cast<int>(x_minus_1) && !exceeded; ++i) {
            if (!(sub & (1u << i))) continue;
            if (state.adj[x][i + 1]) {
                ++edges;
                if (edges > limit) exceeded = true;
            }
        }

        // {1,...,x-1} 内の辺
        for (int i = 0; i < static_cast<int>(x_minus_1) && !exceeded; ++i) {
            if (!(sub & (1u << i))) continue;
            for (int j = i + 1; j < static_cast<int>(x_minus_1) && !exceeded;
                 ++j) {
                if (!(sub & (1u << j))) continue;
                if (state.adj[i + 1][j + 1]) {
                    ++edges;
                    if (edges > limit) exceeded = true;
                }
            }
        }

        if (exceeded) return false;
    }
    return true;
}

/**
 * @brief Laman 逆探索の DFS
 *
 * 頂点 alive_count+1 を追加し、{1,...,alive_count} の部分集合を
 * 近傍として試す。(2,3)-sparsity は遺伝的なので各ステップで枝刈り。
 * tightness (辺数 = 2n-3) はリーフでのみ検査。
 */
inline void laman_enum_dfs(LamanEnumState& state,
                           std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // リーフ: tightness 検査
        if (state.edge_count == 2 * state.total_n - 3) {
            EnumeratedGraph graph;
            graph.n = state.total_n;
            for (int u = 1; u <= state.total_n; ++u)
                for (int v = u + 1; v <= state.total_n; ++v)
                    if (state.adj[u][v])
                        graph.edges.push_back(std::make_pair(u, v));
            out->push_back(graph);
        }
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    int remaining = state.total_n - x;  // x の後に追加される頂点数

    // max_future: 頂点 x+1,...,n が追加できる最大辺数
    int max_future = 0;
    for (int j = x; j < state.total_n; ++j) max_future += j;

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // deg_x: 頂点 x の次数
        int deg_x = 0;
        {
            unsigned long long tmp = mask;
            while (tmp) { tmp &= (tmp - 1); ++deg_x; }
        }
        int new_edge_count = state.edge_count + deg_x;

        // 枝刈り 1: 辺数上限 (sparsity の簡易チェック)
        if (x >= 2 && new_edge_count > 2 * x - 3) continue;

        // 枝刈り 2: 辺数到達可能性
        if (new_edge_count + max_future < 2 * state.total_n - 3) continue;

        // 辺を設定
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
            if (bit) state.deg[u]++;
        }
        state.deg[x] = deg_x;
        state.edge_count = new_edge_count;
        state.alive_count = x;

        bool prune = false;

        // 枝刈り 3: 次数下限 (n >= 3 のとき全頂点 deg >= 2)
        if (state.total_n >= 3) {
            for (int v = 1; v <= x; ++v) {
                if (state.deg[v] + remaining < 2) {
                    prune = true;
                    break;
                }
            }
        }

        // 枝刈り 4: (2,3)-sparsity (増分部分集合検査)
        if (!prune && x >= 3) {
            if (!is_23_sparse(state, x)) prune = true;
        }

        if (!prune) {
            laman_enum_dfs(state, out);
        }

        // 復元
        for (int u = 1; u <= k; ++u) {
            if (state.adj[x][u]) state.deg[u]--;
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.deg[x] = 0;
        state.edge_count -= deg_x;
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Laman グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return LamanEnumerationResult
 *
 * Laman グラフ: n 頂点 2n-3 辺の (2,3)-tight グラフ。
 * (2,3)-sparsity は増分部分集合検査で判定。
 */
inline LamanEnumerationResult
enumerate_laman_graphs(int n,
    LamanEnumAlgorithm algo = LamanEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    LamanEnumerationResult result;
    if (n <= 0) return result;
    if (n == 1) {
        // n=1: 空グラフ (0 辺) を 1 個出力
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }
    if (n == 2) {
        // n=2: K2 (1 辺) を 1 個出力
        EnumeratedGraph g;
        g.n = 2;
        g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }
    detail::LamanEnumState root(n);
    detail::laman_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
