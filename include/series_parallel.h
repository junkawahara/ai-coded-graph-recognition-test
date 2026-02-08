#ifndef GRAPH_RECOGNITION_SERIES_PARALLEL_H
#define GRAPH_RECOGNITION_SERIES_PARALLEL_H

/**
 * @file series_parallel.h
 * @brief 直並列グラフ (series-parallel graph) 認識
 *
 * ここでは「K4 minor を持たない無向グラフ」を直並列グラフとする。
 * 同値な判定として、次数 2 以下の頂点を反復除去して全頂点を
 * 消せるかを用いる。
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 直並列グラフ認識の結果
 */
struct SeriesParallelResult {
    bool is_series_parallel; /**< 直並列グラフであれば true */
};

/**
 * @brief グラフが直並列グラフか判定する
 * @param g 入力グラフ
 * @return SeriesParallelResult
 */
inline SeriesParallelResult check_series_parallel(const Graph& g) {
    SeriesParallelResult res;
    res.is_series_parallel = true;

    int n = g.n;
    std::vector<std::vector<int>> neighbors(n + 1);
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);

    for (int v = 1; v <= n; ++v) {
        neighbors[v].reserve(g.adj_set[v].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[v].begin();
             it != g.adj_set[v].end(); ++it) {
            neighbors[v].push_back(*it);
        }
        degree[v] = (int)neighbors[v].size();
    }

    for (int step = 0; step < n; ++step) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] <= 2) {
                pick = v;
                break;
            }
        }

        if (pick == 0) {
            res.is_series_parallel = false;
            return res;
        }

        alive[pick] = 0;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
