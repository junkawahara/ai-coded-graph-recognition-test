#ifndef GRAPH_RECOGNITION_COCHAIN_ENUM_H
#define GRAPH_RECOGNITION_COCHAIN_ENUM_H

/**
 * @file cochain_enum.h
 * @brief Cochain グラフ (chain グラフの補グラフ) の列挙
 *
 * chain グラフを列挙し、各グラフの補グラフを計算することで
 * 全非同型 cochain グラフを列挙する。
 * 補グラフ操作は同型類上の全単射であるため、
 * 非同型数は chain グラフと同一 (OEIS A005418)。
 */

#include "chain_enum.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 列挙された cochain グラフ
 */
struct CochainEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int>> edges;       /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief Cochain グラフ列挙の結果
 */
struct CochainEnumerationResult {
    std::vector<CochainEnumeratedGraph> graphs;   /**< 列挙された cochain グラフの配列 */
};

/**
 * @brief 頂点数 n の全非同型 cochain グラフを列挙する
 * @param n 頂点数
 * @return CochainEnumerationResult
 *
 * chain グラフを列挙し、各グラフの補グラフを計算して出力する。
 */
inline CochainEnumerationResult enumerate_cochain_graphs(int n) {
    CochainEnumerationResult result;
    if (n <= 0) return result;

    ChainEnumerationResult chains = enumerate_chain_graphs(n);

    // 各 chain グラフの補グラフを構築
    for (std::size_t i = 0; i < chains.graphs.size(); ++i) {
        const ChainEnumeratedGraph& cg = chains.graphs[i];

        // 辺集合をビットで管理 (高速ルックアップ)
        std::vector<std::vector<bool> > has_edge(n + 1, std::vector<bool>(n + 1, false));
        for (std::size_t e = 0; e < cg.edges.size(); ++e) {
            int u = cg.edges[e].first;
            int v = cg.edges[e].second;
            has_edge[u][v] = true;
            has_edge[v][u] = true;
        }

        CochainEnumeratedGraph g;
        g.n = n;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                if (!has_edge[u][v]) {
                    g.edges.push_back(std::make_pair(u, v));
                }
            }
        }
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
