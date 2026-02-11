#ifndef GRAPH_RECOGNITION_DSU_H
#define GRAPH_RECOGNITION_DSU_H

/**
 * @file dsu.h
 * @brief Union-Find (素集合データ構造)
 *
 * 経路圧縮とランクによる統合を備えた Disjoint Set Union。
 */

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Disjoint Set Union (Union-Find)
 *
 * 経路圧縮 (path compression) とランクによる統合 (union by rank) を実装。
 */
struct DSU {
    std::vector<int> p; /**< 親配列 */
    std::vector<int> r; /**< ランク配列 */

    /**
     * @brief コンストラクタ
     * @param n 要素数 (0-indexed: 0 .. n-1)
     */
    DSU(int n = 0) { init(n); }

    /**
     * @brief 初期化
     * @param n 要素数
     */
    void init(int n) {
        p.resize(n);
        r.assign(n, 0);
        for (int i = 0; i < n; ++i) p[i] = i;
    }

    /**
     * @brief 要素 x の代表元を返す
     * @param x 要素
     * @return x の代表元
     */
    int find(int x) {
        return p[x] == x ? x : p[x] = find(p[x]);
    }

    /**
     * @brief 要素 a と b を同じ集合に統合する
     * @param a 要素 a
     * @param b 要素 b
     * @return 統合が行われた場合 true、既に同じ集合なら false
     */
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) std::swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) r[a]++;
        return true;
    }
};

} // namespace graph_recognition

#endif
