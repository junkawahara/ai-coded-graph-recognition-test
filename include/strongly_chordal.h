#ifndef GRAPH_RECOGNITION_STRONGLY_CHORDAL_H
#define GRAPH_RECOGNITION_STRONGLY_CHORDAL_H

/**
 * @file strongly_chordal.h
 * @brief 強弦グラフ (strongly chordal graph) 認識
 *
 * 強弦グラフは、すべての誘導部分グラフが simple vertex を持つ
 * (simple elimination ordering を持つ) ことと同値。
 */

#include "chordal.h"
#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 強弦グラフ認識アルゴリズムの選択
 */
enum class StronglyChordalAlgorithm {
    STRONG_ELIMINATION /**< 強消去順序 */
};

/**
 * @brief 強弦グラフ認識の結果
 */
struct StronglyChordalResult {
    bool is_strongly_chordal; /**< 強弦グラフであれば true */
};

namespace detail_strongly_chordal {

/** @brief alive 部分グラフにおける v の隣接頂点を列挙する */
inline void collect_alive_neighbors(
    const Graph& g,
    int v,
    const std::vector<unsigned char>& alive,
    std::vector<int>* neighbors) {
    neighbors->clear();
    for (size_t i = 0; i < g.adj[v].size(); ++i) {
        int u = g.adj[v][i];
        if (alive[u]) neighbors->push_back(u);
    }
}

/** @brief alive 部分グラフで N[x] が N[y] に含まれるか判定する */
inline bool is_closed_neighborhood_subset(
    const Graph& g,
    int x,
    int y,
    const std::vector<unsigned char>& alive) {
    // x in N[x] が N[y] に入る必要があるので、x==y か xy が辺。
    if (x != y && !g.has_edge(x, y)) return false;

    for (size_t i = 0; i < g.adj[x].size(); ++i) {
        int z = g.adj[x][i];
        if (!alive[z]) continue;
        if (z == y) continue;
        if (!g.has_edge(y, z)) return false;
    }
    return true;
}

/** @brief alive 部分グラフで v が simple vertex か判定する */
inline bool is_simple_vertex(
    const Graph& g,
    int v,
    const std::vector<unsigned char>& alive,
    std::vector<int>* neighbors) {
    collect_alive_neighbors(g, v, alive, neighbors);

    // simple vertex は simplicial である必要がある。
    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (!g.has_edge(x, y)) return false;
        }
    }

    // 近傍の閉近傍同士が包含で比較可能。
    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (is_closed_neighborhood_subset(g, x, y, alive)) continue;
            if (is_closed_neighborhood_subset(g, y, x, alive)) continue;
            return false;
        }
    }

    return true;
}

} // namespace detail_strongly_chordal

/**
 * @brief グラフが強弦グラフか判定する
 * @param g 入力グラフ
 * @return StronglyChordalResult
 *
 * まず弦グラフ性を確認し、その後 simple vertex を
 * 繰り返し除去できるかで判定する。
 */
inline StronglyChordalResult check_strongly_chordal(const Graph& g,
    StronglyChordalAlgorithm algo = StronglyChordalAlgorithm::STRONG_ELIMINATION) {
    (void)algo;
    StronglyChordalResult res;
    res.is_strongly_chordal = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    std::vector<unsigned char> alive(g.n + 1, 1);
    std::vector<int> neighbors;
    neighbors.reserve(g.n);

    int remaining = g.n;
    while (remaining > 0) {
        int pick = 0;
        for (int v = 1; v <= g.n; ++v) {
            if (!alive[v]) continue;
            if (detail_strongly_chordal::is_simple_vertex(g, v, alive, &neighbors)) {
                pick = v;
                break;
            }
        }
        if (pick == 0) return res;

        alive[pick] = 0;
        remaining--;
    }

    res.is_strongly_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
