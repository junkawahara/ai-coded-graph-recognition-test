#ifndef GRAPH_RECOGNITION_GEM_FREE_H
#define GRAPH_RECOGNITION_GEM_FREE_H

/**
 * @file gem_free.h
 * @brief Gem-free グラフ認識
 *
 * Gem-free グラフとは、誘導部分グラフとして gem を含まないグラフである。
 * Gem (fan F_{1,3}) は P4 {a,b,c,d} に universal vertex v を追加した
 * 5 頂点 7 辺のグラフ。辺: {va,vb,vc,vd,ab,bc,cd}, 非辺: {ac,ad,bd}。
 *
 * アルゴリズム:
 *   - BRUTE: 全 5-部分集合を検査 O(n^5)
 *   - NEIGHBOR_P4_SEARCH: 各頂点の近傍で P4 を探索 O(n*m*Delta) (デフォルト)
 *
 * 参考文献:
 *   - Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Gem-free グラフ認識アルゴリズムの選択
 */
enum class GemFreeAlgorithm {
    BRUTE,              /**< 全 5-部分集合検査 O(n^5) */
    NEIGHBOR_P4_SEARCH  /**< 近傍 P4 探索 O(n*m*Delta) (デフォルト) */
};

/**
 * @brief Gem-free グラフ認識の結果
 */
struct GemFreeResult {
    bool is_gem_free = false; /**< gem-free であれば true */
};

namespace detail {

/**
 * @brief 全 5-部分集合による誘導 gem 検出
 *
 * 全 C(n,5) 個の 5-部分集合について辺数と次数列を調べる。
 * gem は 5 頂点上の唯一の (辺数=7, 次数列={2,2,3,3,4}) グラフ。
 * 計算量: O(n^5)
 */
inline GemFreeResult check_gem_free_brute(const Graph& g) {
    GemFreeResult res;
    res.is_gem_free = true;

    int n = g.n;
    if (n < 5) return res;

    for (int a = 1; a <= n - 4; ++a) {
        for (int b = a + 1; b <= n - 3; ++b) {
            for (int c = b + 1; c <= n - 2; ++c) {
                for (int d = c + 1; d <= n - 1; ++d) {
                    for (int e = d + 1; e <= n; ++e) {
                        int v[5] = {a, b, c, d, e};
                        int deg[5] = {0, 0, 0, 0, 0};
                        int edge_count = 0;
                        for (int i = 0; i < 5; ++i) {
                            for (int j = i + 1; j < 5; ++j) {
                                if (g.has_edge(v[i], v[j])) {
                                    ++deg[i];
                                    ++deg[j];
                                    ++edge_count;
                                }
                            }
                        }
                        if (edge_count != 7) continue;

                        // gem の次数列は {2,2,3,3,4} (ソート済み)
                        int sorted_deg[5];
                        for (int i = 0; i < 5; ++i) sorted_deg[i] = deg[i];
                        for (int i = 0; i < 4; ++i)
                            for (int j = i + 1; j < 5; ++j)
                                if (sorted_deg[i] > sorted_deg[j]) {
                                    int tmp = sorted_deg[i];
                                    sorted_deg[i] = sorted_deg[j];
                                    sorted_deg[j] = tmp;
                                }

                        if (sorted_deg[0] == 2 && sorted_deg[1] == 2 &&
                            sorted_deg[2] == 3 && sorted_deg[3] == 3 &&
                            sorted_deg[4] == 4) {
                            res.is_gem_free = false;
                            return res;
                        }
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief 近傍 P4 探索による gem 検出
 *
 * gem = P4 + universal vertex なので、各頂点 v について
 * G[N(v)] 内に誘導 P4 が存在するか探索する。
 * 計算量: O(n * m * Delta)
 */
inline GemFreeResult check_gem_free_neighbor_p4_search(const Graph& g) {
    GemFreeResult res;
    res.is_gem_free = true;

    int n = g.n;
    if (n < 5) return res;

    for (int v = 1; v <= n; ++v) {
        if (g.adj[v].size() < 4) continue;

        // G[N(v)] 内で誘導 P4 a-b-c-d を探索
        for (size_t bi = 0; bi < g.adj[v].size(); ++bi) {
            int b = g.adj[v][bi];
            for (size_t ci = 0; ci < g.adj[b].size(); ++ci) {
                int c = g.adj[b][ci];
                if (c == v) continue;
                if (!g.has_edge(v, c)) continue; // c must be in N(v)

                // 辺 b-c あり、両方 N(v) に属する
                // a in N(v) ∩ N(b), a != c, a not adj c
                for (size_t ai = 0; ai < g.adj[v].size(); ++ai) {
                    int a = g.adj[v][ai];
                    if (a == b || a == c) continue;
                    if (!g.has_edge(a, b)) continue;
                    if (g.has_edge(a, c)) continue;

                    // 誘導 P3: a-b-c in N(v)
                    // d in N(v) ∩ N(c), d != b, d != a, d not adj b, d not adj a
                    for (size_t di = 0; di < g.adj[v].size(); ++di) {
                        int d = g.adj[v][di];
                        if (d == b || d == c || d == a) continue;
                        if (!g.has_edge(d, c)) continue;
                        if (g.has_edge(d, b)) continue;
                        if (g.has_edge(d, a)) continue;

                        // gem {v, a, b, c, d} 発見
                        res.is_gem_free = false;
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief グラフが gem-free か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: NEIGHBOR_P4_SEARCH)
 * @return GemFreeResult
 *
 * G が gem-free <=> 誘導部分グラフとして gem を含まない。
 * gem は P4 {a,b,c,d} + universal vertex v (5 頂点 7 辺)。
 */
inline GemFreeResult check_gem_free(const Graph& g,
    GemFreeAlgorithm algo = GemFreeAlgorithm::NEIGHBOR_P4_SEARCH) {
    switch (algo) {
        case GemFreeAlgorithm::BRUTE:
            return detail::check_gem_free_brute(g);
        case GemFreeAlgorithm::NEIGHBOR_P4_SEARCH:
            return detail::check_gem_free_neighbor_p4_search(g);
        default:
            break;
    }
    return GemFreeResult();
}

} // namespace graph_recognition

#endif
