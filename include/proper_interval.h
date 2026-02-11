#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_H

/**
 * @file proper_interval.h
 * @brief 固有インターバルグラフ (proper interval graph) 認識
 *
 * インターバルグラフかつ claw-free (K_{1,3}-free) であれば固有インターバルグラフ。
 */

#include "graph.h"
#include "interval.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 固有インターバルグラフ認識アルゴリズムの選択
 */
enum class ProperIntervalAlgorithm {
    PQ_TREE /**< PQ 木による判定 */
};

/**
 * @brief 固有インターバルグラフ認識の結果
 */
struct ProperIntervalResult {
    bool is_proper_interval; /**< 固有インターバルグラフであれば true */
};

namespace detail {

/**
 * @brief 誘導 claw (K_{1,3}) が存在するか判定する (内部関数)
 * @param g 入力グラフ
 * @return 誘導 claw が存在すれば true
 */
inline bool has_induced_claw(const Graph& g) {
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

} // namespace detail

/**
 * @brief グラフが固有インターバルグラフか判定する
 * @param g 入力グラフ
 * @return ProperIntervalResult
 *
 * G が固有インターバルグラフ ⟺ G がインターバルグラフかつ claw-free。
 */
inline ProperIntervalResult check_proper_interval(const Graph& g,
    ProperIntervalAlgorithm algo = ProperIntervalAlgorithm::PQ_TREE) {
    (void)algo;
    ProperIntervalResult res;
    res.is_proper_interval = false;

    IntervalResult interval = check_interval(g);
    if (!interval.is_interval) return res;

    if (detail::has_induced_claw(g)) return res;

    res.is_proper_interval = true;
    return res;
}

} // namespace graph_recognition

#endif
