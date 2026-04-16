#ifndef GRAPH_RECOGNITION_FULLERENE_H
#define GRAPH_RECOGNITION_FULLERENE_H

/**
 * @file fullerene.h
 * @brief フラーレングラフ (fullerene graph) 認識
 *
 * フラーレンは三正則 3-連結平面グラフで全ての面が
 * 五角形 (5-gon) または六角形 (6-gon) であるもの。
 * n は偶数、n >= 20、n != 22。
 */

#include "graph.h"
#include "planar.h"
#include "triconnected.h"
#include "planar_embedding.h"

namespace graph_recognition {

/**
 * @brief フラーレン認識アルゴリズムの選択
 */
enum class FullereneAlgorithm {
    FACE_CHECK /**< 平面埋め込みの面サイズチェック */
};

/**
 * @brief フラーレン認識の結果
 */
struct FullereneResult {
    bool is_fullerene = false; /**< フラーレンであれば true */
};

/**
 * @brief グラフがフラーレンか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: FACE_CHECK)
 * @return FullereneResult
 *
 * フラーレン ⟺ 三正則 ∧ 3-連結 ∧ 平面 ∧ 全面が五角形 or 六角形。
 * 必要条件: n 偶数, n >= 20, n != 22。
 */
inline FullereneResult check_fullerene(const Graph& g,
    FullereneAlgorithm algo = FullereneAlgorithm::FACE_CHECK) {
    (void)algo;
    FullereneResult res;

    int n = g.n;

    /* 必要条件: n 偶数, n >= 20, n != 22 */
    if (n < 20) return res;
    if (n % 2 != 0) return res;
    if (n == 22) return res;

    /* 三正則チェック */
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != 3) return res;
    }

    /* 辺数チェック: m = 3n/2 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 3LL * n / 2) return res;

    /* 連結性チェック */
    {
        std::vector<char> visited(n + 1, 0);
        std::vector<int> queue;
        queue.push_back(1);
        visited[1] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (!visited[u]) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
        if ((int)queue.size() != n) return res;
    }

    /* 3-連結チェック */
    TriconnectedResult tr = check_triconnected(g);
    if (!tr.is_triconnected) return res;

    /* 平面性チェック */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    /* 平面埋め込みを計算 */
    PlanarEmbeddingResult emb = compute_planar_embedding(g);
    if (!emb.success) return res;

    /* 面数チェック: Euler 公式 f = m - n + 2 = 3n/2 - n + 2 = n/2 + 2 */
    int expected_faces = n / 2 + 2;
    if ((int)emb.faces.size() != expected_faces) return res;

    /* 全面が五角形 (5) または六角形 (6) であることを検証 */
    int pentagons = 0, hexagons = 0;
    for (size_t i = 0; i < emb.faces.size(); ++i) {
        int fsize = (int)emb.faces[i].size();
        if (fsize == 5) {
            ++pentagons;
        } else if (fsize == 6) {
            ++hexagons;
        } else {
            return res; /* 五角形でも六角形でもない面 */
        }
    }

    /* 検証: 正確に 12 個の五角形 */
    if (pentagons != 12) return res;
    /* 検証: 六角形 = n/2 - 10 */
    if (hexagons != n / 2 - 10) return res;

    res.is_fullerene = true;
    return res;
}

} // namespace graph_recognition

#endif
