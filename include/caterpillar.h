#ifndef GRAPH_RECOGNITION_CATERPILLAR_H
#define GRAPH_RECOGNITION_CATERPILLAR_H

/**
 * @file caterpillar.h
 * @brief 毛虫グラフ (caterpillar graph) 認識
 *
 * 木であることを確認した上で、葉を除去した残りが道であることを判定する。
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief 毛虫グラフ認識アルゴリズムの選択
 */
enum class CaterpillarAlgorithm {
    LEAF_REMOVAL /**< 葉除去 + 道判定 */
};

/**
 * @brief 毛虫グラフ認識の結果
 */
struct CaterpillarResult {
    bool is_caterpillar = false; /**< 毛虫グラフであれば true */
};

/**
 * @brief グラフが毛虫グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: LEAF_REMOVAL)
 * @return CaterpillarResult
 *
 * 毛虫グラフ ⟺ 木であり、葉を全て除去すると道 (または空) になる。
 */
inline CaterpillarResult check_caterpillar(const Graph& g,
    CaterpillarAlgorithm algo = CaterpillarAlgorithm::LEAF_REMOVAL) {
    (void)algo;
    CaterpillarResult res;

    int n = g.n;

    /* n <= 2 の木は全て毛虫 */
    if (n <= 2) {
        /* n=0: 空グラフ → 毛虫 */
        /* n=1: 孤立頂点 → 毛虫 */
        /* n=2: 辺 0 本 (2頂点の森) または 1 本 (パス) → 辺 1 本の場合のみ木 */
        long long m = 0;
        for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
        m /= 2;
        if (n == 0 || m == (long long)n - 1) {
            /* 連結チェック (n=2 で辺がない場合は非連結 → 木ではない) */
            if (n <= 1) {
                res.is_caterpillar = true;
                return res;
            }
            /* n=2: m=1 であれば木 */
            if (m == 1) {
                res.is_caterpillar = true;
            }
            return res;
        }
        return res;
    }

    /* 木であるか確認: m = n - 1 かつ連結 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != (long long)n - 1) return res;

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

    /* 葉 (次数 1) を除去し、残りの頂点の次数を計算 */
    std::vector<char> is_leaf(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() == 1) {
            is_leaf[v] = 1;
        }
    }

    /* 非葉頂点の中で、非葉近傍の次数 (spine の次数) を計算 */
    int spine_count = 0;
    for (int v = 1; v <= n; ++v) {
        if (is_leaf[v]) continue;
        ++spine_count;
        int spine_deg = 0;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            if (!is_leaf[g.adj[v][i]]) {
                ++spine_deg;
            }
        }
        /* 道であるためには各頂点の (spine 内) 次数が 2 以下 */
        if (spine_deg > 2) return res;
    }

    /* spine が空 (全頂点が葉 → n <= 2 で既に処理済み、ここに来ない) */
    /* spine が 1 頂点 → 星グラフ → 毛虫 */
    /* spine が道であることを確認 (連結 + 各次数 ≤ 2 → 道または閉路) */
    /* 木の部分グラフなので閉路はありえない → 道 */
    res.is_caterpillar = true;
    return res;
}

} // namespace graph_recognition

#endif
