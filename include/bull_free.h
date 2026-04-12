#ifndef GRAPH_RECOGNITION_BULL_FREE_H
#define GRAPH_RECOGNITION_BULL_FREE_H

/**
 * @file bull_free.h
 * @brief Bull-free グラフ認識
 *
 * Bull-free グラフとは、誘導部分グラフとして bull を含まないグラフである。
 * Bull は三角形 {a,b,c} に 2 本のペンダント辺 a-x, b-y を追加した
 * 5 頂点 5 辺のグラフ (x,y は三角形外、x != y)。
 *
 * アルゴリズム:
 *   - BRUTE: 全 5-部分集合を検査 O(n^5)
 *   - TRIANGLE_SEARCH: 三角形列挙 + ペンダント探索 O(m*Delta^2) (デフォルト)
 *
 * 参考文献:
 *   - Chudnovsky, "The structure of bull-free graphs I-III," JCTB, 2012
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Bull-free グラフ認識アルゴリズムの選択
 */
enum class BullFreeAlgorithm {
    BRUTE,           /**< 全 5-部分集合検査 O(n^5) */
    TRIANGLE_SEARCH  /**< 三角形 + ペンダント探索 O(m*Delta^2) (デフォルト) */
};

/**
 * @brief Bull-free グラフ認識の結果
 */
struct BullFreeResult {
    bool is_bull_free = false; /**< bull-free であれば true */
};

namespace detail {

/**
 * @brief 全 5-部分集合による誘導 bull 検出
 *
 * 全 C(n,5) 個の 5-部分集合について辺数と次数列を調べる。
 * bull は 5 頂点上の唯一の (辺数=5, 次数列={1,1,2,3,3}) グラフ。
 * 計算量: O(n^5)
 */
inline BullFreeResult check_bull_free_brute(const Graph& g) {
    BullFreeResult res;
    res.is_bull_free = true;

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
                        if (edge_count != 5) continue;

                        // bull の次数列は {1,1,2,3,3} (ソート済み)
                        int sorted_deg[5];
                        for (int i = 0; i < 5; ++i) sorted_deg[i] = deg[i];
                        for (int i = 0; i < 4; ++i)
                            for (int j = i + 1; j < 5; ++j)
                                if (sorted_deg[i] > sorted_deg[j]) {
                                    int tmp = sorted_deg[i];
                                    sorted_deg[i] = sorted_deg[j];
                                    sorted_deg[j] = tmp;
                                }

                        if (sorted_deg[0] == 1 && sorted_deg[1] == 1 &&
                            sorted_deg[2] == 2 && sorted_deg[3] == 3 &&
                            sorted_deg[4] == 3) {
                            res.is_bull_free = false;
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
 * @brief 三角形列挙 + ペンダント探索による bull 検出
 *
 * 各辺 (a,b) の共通近傍 c で三角形を発見し、c を次数 2 頂点として
 * N(a)\{b,c} から b,c に非隣接な x を、N(b)\{a,c,x} から a,c,x に
 * 非隣接な y を探す。
 * 計算量: O(m * Delta^2)
 */
inline BullFreeResult check_bull_free_triangle_search(const Graph& g) {
    BullFreeResult res;
    res.is_bull_free = true;

    int n = g.n;
    if (n < 5) return res;

    for (int a = 1; a <= n; ++a) {
        for (size_t bi = 0; bi < g.adj[a].size(); ++bi) {
            int b = g.adj[a][bi];
            if (b <= a) continue; // 各辺を 1 回だけ処理

            // 共通近傍 c を探す (三角形 {a, b, c})
            for (size_t ci = 0; ci < g.adj[a].size(); ++ci) {
                int c = g.adj[a][ci];
                if (c == b) continue;
                if (!g.has_edge(b, c)) continue;

                // 三角形 {a,b,c} を発見。c を次数 2 頂点として扱う。
                // x in N(a)\{b,c}: x not adj to b, x not adj to c
                for (size_t xi = 0; xi < g.adj[a].size(); ++xi) {
                    int x = g.adj[a][xi];
                    if (x == b || x == c) continue;
                    if (g.has_edge(x, b)) continue;
                    if (g.has_edge(x, c)) continue;

                    // y in N(b)\{a,c,x}: y not adj to a, y not adj to c, y not adj to x
                    for (size_t yi = 0; yi < g.adj[b].size(); ++yi) {
                        int y = g.adj[b][yi];
                        if (y == a || y == c || y == x) continue;
                        if (g.has_edge(y, a)) continue;
                        if (g.has_edge(y, c)) continue;
                        if (g.has_edge(y, x)) continue;

                        // bull {a,b,c,x,y} 発見
                        res.is_bull_free = false;
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
 * @brief グラフが bull-free か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: TRIANGLE_SEARCH)
 * @return BullFreeResult
 *
 * G が bull-free <=> 誘導部分グラフとして bull を含まない。
 */
inline BullFreeResult check_bull_free(const Graph& g,
    BullFreeAlgorithm algo = BullFreeAlgorithm::TRIANGLE_SEARCH) {
    switch (algo) {
        case BullFreeAlgorithm::BRUTE:
            return detail::check_bull_free_brute(g);
        case BullFreeAlgorithm::TRIANGLE_SEARCH:
            return detail::check_bull_free_triangle_search(g);
        default:
            break;
    }
    return BullFreeResult();
}

} // namespace graph_recognition

#endif
