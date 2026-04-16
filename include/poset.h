#ifndef GRAPH_RECOGNITION_POSET_H
#define GRAPH_RECOGNITION_POSET_H

/**
 * @file poset.h
 * @brief 半順序集合 (poset) 認識 — Hasse 図の判定
 *
 * 入力の有向グラフが有効な Hasse 図 (被覆関係) であるか判定する。
 * 条件: DAG (非巡回) かつ推移的簡約 (transitive reduction)。
 *
 * 入力形式: n m (頂点数, 弧数) 続いて m 行の弧 u v (u <_P v の被覆関係)
 */

#include <iostream>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 半順序認識アルゴリズムの選択
 */
enum class PosetAlgorithm {
    DAG_AND_REDUCTION /**< DAG チェック + 推移的簡約チェック */
};

/**
 * @brief 半順序認識の結果
 */
struct PosetResult {
    bool is_poset = false; /**< 有効な Hasse 図であれば true */
};

/**
 * @brief 有向グラフが Hasse 図か判定する
 * @param n 頂点数
 * @param arcs 弧リスト (u, v) = u が v を被覆 (u <_P v)
 * @param algo 使用アルゴリズム
 * @return PosetResult
 *
 * Hasse 図 ⟺ DAG かつ推移的簡約 (任意の弧 u→v に対し
 * u から v への長さ ≥ 2 の有向パスが存在しない)。
 */
inline PosetResult check_poset(int n,
    const std::vector<std::pair<int, int>>& arcs,
    PosetAlgorithm algo = PosetAlgorithm::DAG_AND_REDUCTION) {
    (void)algo;
    PosetResult res;

    if (n < 0) return res;
    if (n == 0) {
        res.is_poset = arcs.empty();
        return res;
    }

    /* 隣接リスト構築 + 入力検証 */
    std::vector<std::vector<int>> adj_out(n + 1);
    std::set<std::pair<int, int>> arc_set;
    std::vector<int> in_deg(n + 1, 0);

    for (size_t i = 0; i < arcs.size(); ++i) {
        int u = arcs[i].first, v = arcs[i].second;
        if (u < 1 || u > n || v < 1 || v > n) return res;
        if (u == v) return res; /* 自己ループ */
        if (!arc_set.insert(std::make_pair(u, v)).second) return res; /* 重複弧 */
        adj_out[u].push_back(v);
        in_deg[v]++;
    }

    /* DAG チェック (トポロジカルソート) */
    std::vector<int> topo;
    std::vector<int> queue;
    for (int v = 1; v <= n; ++v) {
        if (in_deg[v] == 0) queue.push_back(v);
    }
    std::vector<int> tmp_in(in_deg);
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int v = queue[qi];
        topo.push_back(v);
        for (size_t i = 0; i < adj_out[v].size(); ++i) {
            int u = adj_out[v][i];
            if (--tmp_in[u] == 0) queue.push_back(u);
        }
    }
    if ((int)topo.size() != n) return res; /* 閉路あり */

    /* 推移的簡約チェック: 各弧 u→v に対し、
       u から v への長さ ≥ 2 のパスが存在しないことを確認。
       u の各子 w (w ≠ v) から v に到達可能なら、u→v は冗長。 */
    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < adj_out[u].size(); ++ei) {
            int v = adj_out[u][ei];

            /* u の子 (v 以外) から BFS で v に到達可能か */
            std::vector<char> reachable(n + 1, 0);
            std::vector<int> bfs;
            for (size_t j = 0; j < adj_out[u].size(); ++j) {
                int w = adj_out[u][j];
                if (w != v && !reachable[w]) {
                    reachable[w] = 1;
                    bfs.push_back(w);
                }
            }
            for (size_t qi = 0; qi < bfs.size(); ++qi) {
                int w = bfs[qi];
                if (w == v) {
                    return res; /* 冗長弧 → 推移的簡約でない */
                }
                for (size_t j = 0; j < adj_out[w].size(); ++j) {
                    int x = adj_out[w][j];
                    if (!reachable[x]) {
                        reachable[x] = 1;
                        bfs.push_back(x);
                    }
                }
            }
            if (reachable[v]) return res;
        }
    }

    res.is_poset = true;
    return res;
}

} // namespace graph_recognition

#endif
