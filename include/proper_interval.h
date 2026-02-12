#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_H

/**
 * @file proper_interval.h
 * @brief 固有インターバルグラフ (proper interval graph) 認識
 *
 * アルゴリズム:
 *   - PQ_TREE: 三重ループ claw 検出 O(n·Δ³)
 *   - FAST_CLAW_CHECK: 辺計数 claw 検出 O(m·Δ) (デフォルト)
 */

#include "graph.h"
#include "interval.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 固有インターバルグラフ認識アルゴリズムの選択
 */
enum class ProperIntervalAlgorithm {
    PQ_TREE,        /**< 三重ループ claw 検出 O(n·Δ³) */
    FAST_CLAW_CHECK /**< 辺計数 claw 検出 O(m·Δ) (デフォルト) */
};

/**
 * @brief 固有インターバルグラフ認識の結果
 */
struct ProperIntervalResult {
    bool is_proper_interval; /**< 固有インターバルグラフであれば true */
};

namespace detail {

/**
 * @brief 誘導 claw (K_{1,3}) が存在するか判定する (元のアルゴリズム)
 */
inline bool has_induced_claw_triple(const Graph& g) {
    for (int c = 1; c <= g.n; ++c) {
        if (g.adj_set[c].size() < 3) continue;

        std::vector<int> nbrs;
        nbrs.reserve(g.adj_set[c].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[c].begin();
             it != g.adj_set[c].end(); ++it) {
            nbrs.push_back(*it);
        }

        for (size_t i = 0; i < nbrs.size(); ++i) {
            for (size_t j = i + 1; j < nbrs.size(); ++j) {
                if (g.has_edge(nbrs[i], nbrs[j])) continue;
                for (size_t k = j + 1; k < nbrs.size(); ++k) {
                    if (g.has_edge(nbrs[i], nbrs[k])) continue;
                    if (g.has_edge(nbrs[j], nbrs[k])) continue;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief 辺計数による高速 claw 検出 O(m·Δ)
 *
 * 各頂点 c について N(c) 内の辺数をカウント。
 * d = deg(c) として辺数 == d(d-1)/2 なら N(c) は完全 → claw なし。
 * そうでなければ N(c) に非辺があるので claw を探す。
 */
inline bool has_induced_claw_fast(const Graph& g) {
    int n = g.n;
    std::vector<unsigned char> stamped(n + 1, 0);

    for (int c = 1; c <= n; ++c) {
        int d = (int)g.adj[c].size();
        if (d < 3) continue;

        // N(c) をスタンプ
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        // N(c) 内の辺数をカウント
        long long edge_count = 0;
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            int u = g.adj[c][i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (stamped[w] && w > u) edge_count++;
            }
        }

        // スタンプ解除
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 0;
        }

        long long need = (long long)d * (d - 1) / 2;
        if (edge_count == need) continue; // N(c) は完全グラフ

        // N(c) に非辺あり → claw を探す
        // N(c) をスタンプ (再度)
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        bool found_claw = false;
        // 非辺 (a, b) を見つける
        for (size_t i = 0; i < g.adj[c].size() && !found_claw; ++i) {
            int a = g.adj[c][i];
            // a の N(c) 内隣接をマーク
            std::vector<unsigned char> a_adj(n + 1, 0);
            for (size_t j = 0; j < g.adj[a].size(); ++j) {
                if (stamped[g.adj[a][j]]) a_adj[g.adj[a][j]] = 1;
            }

            for (size_t j = i + 1; j < g.adj[c].size() && !found_claw; ++j) {
                int b = g.adj[c][j];
                if (a_adj[b]) continue; // a-b は辺

                // 非辺 (a, b) 発見。x in N(c) で x != a, b, !edge(x,a), !edge(x,b)
                for (size_t k = 0; k < g.adj[c].size(); ++k) {
                    int x = g.adj[c][k];
                    if (x == a || x == b) continue;
                    if (!a_adj[x] && !g.has_edge(x, b)) {
                        found_claw = true;
                        break;
                    }
                }
            }
        }

        // スタンプ解除
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 0;
        }

        if (found_claw) return true;
    }
    return false;
}

/** @brief PQ_TREE: 元のアルゴリズム */
inline ProperIntervalResult check_proper_interval_pq(const Graph& g) {
    ProperIntervalResult res;
    res.is_proper_interval = false;

    IntervalResult interval = check_interval(g);
    if (!interval.is_interval) return res;

    if (has_induced_claw_triple(g)) return res;

    res.is_proper_interval = true;
    return res;
}

/** @brief FAST_CLAW_CHECK: 辺計数による高速判定 */
inline ProperIntervalResult check_proper_interval_fast(const Graph& g) {
    ProperIntervalResult res;
    res.is_proper_interval = false;

    IntervalResult interval = check_interval(g);
    if (!interval.is_interval) return res;

    if (has_induced_claw_fast(g)) return res;

    res.is_proper_interval = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフが固有インターバルグラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: FAST_CLAW_CHECK)
 * @return ProperIntervalResult
 *
 * G が固有インターバルグラフ ⟺ G がインターバルグラフかつ claw-free。
 */
inline ProperIntervalResult check_proper_interval(const Graph& g,
    ProperIntervalAlgorithm algo = ProperIntervalAlgorithm::FAST_CLAW_CHECK) {
    switch (algo) {
        case ProperIntervalAlgorithm::PQ_TREE:
            return detail::check_proper_interval_pq(g);
        case ProperIntervalAlgorithm::FAST_CLAW_CHECK:
            return detail::check_proper_interval_fast(g);
    }
    return ProperIntervalResult();
}

} // namespace graph_recognition

#endif
