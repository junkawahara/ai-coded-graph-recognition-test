#ifndef GRAPH_RECOGNITION_HALIN_H
#define GRAPH_RECOGNITION_HALIN_H

/**
 * @file halin.h
 * @brief Halin グラフ認識
 *
 * Halin グラフは、次数 2 の内部頂点を持たない木を平面に埋め込み、
 * 全葉を巡る閉路で結んだグラフ。
 * 3-連結平面グラフで、ある面のサイクル辺を除去すると
 * 葉が面の頂点に一致する木になることで判定する。
 */

#include "graph.h"
#include "planar.h"
#include "triconnected.h"
#include "planar_embedding.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Halin グラフ認識アルゴリズムの選択
 */
enum class HalinAlgorithm {
    FACE_CHECK /**< 平面埋め込みの面チェック */
};

/**
 * @brief Halin グラフ認識の結果
 */
struct HalinResult {
    bool is_halin = false; /**< Halin グラフであれば true */
};

/**
 * @brief グラフが Halin グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: FACE_CHECK)
 * @return HalinResult
 *
 * Halin グラフ ⟺ 3-連結平面グラフで、ある面 F について
 * F の辺を除去すると木 T が得られ、T の葉集合 = F の頂点集合、
 * T の全内部頂点の次数 ≥ 3。
 */
inline HalinResult check_halin(const Graph& g,
    HalinAlgorithm algo = HalinAlgorithm::FACE_CHECK) {
    (void)algo;
    HalinResult res;

    int n = g.n;
    if (n < 4) return res;

    /* 最小次数 ≥ 3 */
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() < 3) return res;
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

    /* 辺数 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    /* 各面について: 面の辺を除去した残りが条件を満たすか検査 */
    for (size_t fi = 0; fi < emb.faces.size(); ++fi) {
        const std::vector<int>& face = emb.faces[fi];
        int fsize = (int)face.size();
        if (fsize < 3) continue;

        /* 面の辺数 = fsize (閉路) */
        /* 木の辺数 = m - fsize, 頂点数 = n → 木なら m - fsize = n - 1 */
        if ((long long)(m - fsize) != (long long)(n - 1)) continue;

        /* 面の辺集合を構築 */
        std::vector<char> face_vertex(n + 1, 0);
        /* face_edge[u] のセットに v が含まれれば (u,v) は面の辺 */
        std::vector<std::unordered_set<int>> face_edge(n + 1);
        for (int i = 0; i < fsize; ++i) {
            int u = face[i];
            int v = face[(i + 1) % fsize];
            face_vertex[u] = 1;
            face_edge[u].insert(v);
            face_edge[v].insert(u);
        }

        /* 面の辺を除去した残りグラフの次数を計算 */
        std::vector<int> tree_deg(n + 1, 0);
        for (int v = 1; v <= n; ++v) {
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!face_edge[v].count(u)) {
                    tree_deg[v]++;
                }
            }
        }
        /* tree_deg は各頂点の辺を二重にカウントしているわけではない
           (adj[v] の各要素に対して 1 ずつカウント) */

        /* 条件チェック:
           1. 面の頂点 = 木の葉 (tree_deg == 1)
           2. 非面頂点 = 木の内部 (tree_deg >= 3)
           3. 残りグラフが木 (連結 + 辺数 = n-1, 既にチェック済み) */
        bool valid = true;
        for (int v = 1; v <= n; ++v) {
            if (face_vertex[v]) {
                if (tree_deg[v] != 1) { valid = false; break; }
            } else {
                if (tree_deg[v] < 3) { valid = false; break; }
            }
        }
        if (!valid) continue;

        /* 連結性チェック (面辺除去後) */
        std::vector<char> visited(n + 1, 0);
        std::vector<int> queue;
        queue.push_back(1);
        visited[1] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!visited[u] && !face_edge[v].count(u)) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
        if ((int)queue.size() != n) continue;

        res.is_halin = true;
        return res;
    }

    return res;
}

} // namespace graph_recognition

#endif
