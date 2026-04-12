#ifndef GRAPH_RECOGNITION_P5_FREE_H
#define GRAPH_RECOGNITION_P5_FREE_H

/**
 * @file p5_free.h
 * @brief P5-free グラフ認識
 *
 * P5-free グラフとは、誘導部分グラフとして P5 (長さ 4 のパス) を含まない
 * グラフである。P5 は 5 頂点 {a,b,c,d,e}、辺 {ab,bc,cd,de} のグラフ。
 *
 * P3-free = cluster (done), P4-free = cograph (done) に続く Pk-free 系列。
 *
 * アルゴリズム:
 *   - BRUTE: 全 5-部分集合を検査 O(n^5)
 *   - PATH_SEARCH: 各辺から両方向にパスを伸長して P5 を探索 (デフォルト)
 *
 * 参考文献:
 *   - Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief P5-free グラフ認識アルゴリズムの選択
 */
enum class P5FreeAlgorithm {
    BRUTE,       /**< 全 5-部分集合検査 O(n^5) */
    PATH_SEARCH  /**< パス伸長探索 (デフォルト) */
};

/**
 * @brief P5-free グラフ認識の結果
 */
struct P5FreeResult {
    bool is_p5_free = false; /**< P5-free であれば true */
};

namespace detail {

/**
 * @brief 全 5-部分集合による誘導 P5 検出
 *
 * 全 C(n,5) 個の 5-部分集合について辺数と次数列を調べる。
 * P5 は 5 頂点上の唯一の (辺数=4, 次数列={1,1,2,2,2}) グラフ。
 * 計算量: O(n^5)
 */
inline P5FreeResult check_p5_free_brute(const Graph& g) {
    P5FreeResult res;
    res.is_p5_free = true;

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
                        if (edge_count != 4) continue;

                        // P5 の次数列は {1,1,2,2,2} (ソート済み)
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
                            sorted_deg[2] == 2 && sorted_deg[3] == 2 &&
                            sorted_deg[4] == 2) {
                            res.is_p5_free = false;
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
 * @brief パス伸長探索による誘導 P5 検出
 *
 * 各辺 (b,c) に対し、a-b-c を誘導 P3 に伸ばし、
 * さらに a-b-c-d, a-b-c-d-e と伸長して誘導 P5 を探索する。
 */
inline P5FreeResult check_p5_free_path_search(const Graph& g) {
    P5FreeResult res;
    res.is_p5_free = true;

    int n = g.n;
    if (n < 5) return res;

    // 各有向辺 b→c について誘導 P5: a-b-c-d-e を探索
    for (int b = 1; b <= n; ++b) {
        for (size_t ci = 0; ci < g.adj[b].size(); ++ci) {
            int c = g.adj[b][ci];

            // a: N(b) \ {c}, a not adj c → 誘導 P3: a-b-c
            for (size_t ai = 0; ai < g.adj[b].size(); ++ai) {
                int a = g.adj[b][ai];
                if (a == c) continue;
                if (g.has_edge(a, c)) continue;

                // d: N(c) \ {b}, d not adj b, d not adj a → 誘導 P4: a-b-c-d
                for (size_t di = 0; di < g.adj[c].size(); ++di) {
                    int d = g.adj[c][di];
                    if (d == b || d == a) continue;
                    if (g.has_edge(d, b)) continue;
                    if (g.has_edge(d, a)) continue;

                    // e: N(d) \ {c}, e not adj c, e not adj b, e not adj a
                    // → 誘導 P5: a-b-c-d-e
                    for (size_t ei = 0; ei < g.adj[d].size(); ++ei) {
                        int e = g.adj[d][ei];
                        if (e == c || e == b || e == a) continue;
                        if (g.has_edge(e, c)) continue;
                        if (g.has_edge(e, b)) continue;
                        if (g.has_edge(e, a)) continue;

                        // 誘導 P5: a-b-c-d-e 発見
                        res.is_p5_free = false;
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
 * @brief グラフが P5-free か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: PATH_SEARCH)
 * @return P5FreeResult
 *
 * G が P5-free <=> 誘導部分グラフとして P5 (長さ 4 のパス) を含まない。
 * P5 は 5 頂点 {a,b,c,d,e}、辺 {ab,bc,cd,de} のグラフ。
 * n ≤ 4 では全グラフが P5-free。
 */
inline P5FreeResult check_p5_free(const Graph& g,
    P5FreeAlgorithm algo = P5FreeAlgorithm::PATH_SEARCH) {
    switch (algo) {
        case P5FreeAlgorithm::BRUTE:
            return detail::check_p5_free_brute(g);
        case P5FreeAlgorithm::PATH_SEARCH:
            return detail::check_p5_free_path_search(g);
        default:
            break;
    }
    return P5FreeResult();
}

} // namespace graph_recognition

#endif
