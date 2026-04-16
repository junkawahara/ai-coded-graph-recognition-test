#ifndef GRAPH_RECOGNITION_SELF_COMPLEMENTARY_H
#define GRAPH_RECOGNITION_SELF_COMPLEMENTARY_H

/**
 * @file self_complementary.h
 * @brief 自己補グラフ (self-complementary graph) 認識
 *
 * G ≅ complement(G) であるかを判定する。
 * n ≡ 0 or 1 (mod 4) が必要条件。
 * 補グラフとの同型性をバックトラッキングで検査する。
 */

#include "graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief 自己補グラフ認識アルゴリズムの選択
 */
enum class SelfComplementaryAlgorithm {
    ISOMORPHISM_CHECK /**< 補グラフとの同型性チェック */
};

/**
 * @brief 自己補グラフ認識の結果
 */
struct SelfComplementaryResult {
    bool is_self_complementary = false; /**< 自己補グラフであれば true */
};

namespace detail {

/**
 * @brief グラフ同型性のバックトラッキング検査
 *
 * g1 から g2 への同型写像を探索する。
 * 頂点の次数列によるプルーニング付き。
 */
inline bool check_isomorphism_bt(const Graph& g1, const Graph& g2) {
    int n = g1.n;
    if (n != g2.n) return false;
    if (n == 0) return true;

    /* 次数列チェック */
    std::vector<int> deg1(n), deg2(n);
    for (int v = 1; v <= n; ++v) {
        deg1[v - 1] = (int)g1.adj[v].size();
        deg2[v - 1] = (int)g2.adj[v].size();
    }
    std::vector<int> sd1(deg1), sd2(deg2);
    std::sort(sd1.begin(), sd1.end());
    std::sort(sd2.begin(), sd2.end());
    if (sd1 != sd2) return false;

    /* 次数でグループ化し、候補を絞る */
    /* perm[i] = g1 の頂点 (i+1) が g2 のどの頂点に写るか */
    std::vector<int> perm(n + 1, 0);
    std::vector<char> used(n + 1, 0);

    /* 各頂点の候補リスト (同じ次数の頂点) */
    std::vector<std::vector<int>> candidates(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (int u = 1; u <= n; ++u) {
            if (deg2[u - 1] == deg1[v - 1]) {
                candidates[v].push_back(u);
            }
        }
    }

    /* 頂点を次数の候補数が少ない順にソート (より制約の強い頂点を先に) */
    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
        [&candidates](int a, int b) {
            return candidates[a].size() < candidates[b].size();
        });

    /* バックトラッキング */
    struct State {
        const Graph& g1;
        const Graph& g2;
        std::vector<int>& perm;
        std::vector<char>& used;
        std::vector<std::vector<int>>& candidates;
        std::vector<int>& order;
        int n;

        bool solve(int depth) {
            if (depth == n) return true;
            int v = order[depth]; /* g1 の頂点 v */
            const std::vector<int>& cands = candidates[v];
            for (size_t ci = 0; ci < cands.size(); ++ci) {
                int u = cands[ci]; /* g2 の頂点 u に写す試み */
                if (used[u]) continue;

                /* 既に割り当て済みの頂点との辺の整合性チェック */
                bool ok = true;
                for (int d = 0; d < depth && ok; ++d) {
                    int v2 = order[d];
                    int u2 = perm[v2];
                    bool e1 = g1.has_edge(v, v2);
                    bool e2 = g2.has_edge(u, u2);
                    if (e1 != e2) ok = false;
                }
                if (!ok) continue;

                perm[v] = u;
                used[u] = 1;
                if (solve(depth + 1)) return true;
                used[u] = 0;
                perm[v] = 0;
            }
            return false;
        }
    };

    State state = {g1, g2, perm, used, candidates, order, n};
    return state.solve(0);
}

} // namespace detail

/**
 * @brief グラフが自己補グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: ISOMORPHISM_CHECK)
 * @return SelfComplementaryResult
 *
 * 自己補グラフ ⟺ G ≅ complement(G)。
 * 必要条件: n ≡ 0 or 1 (mod 4), m = n(n-1)/4。
 */
inline SelfComplementaryResult check_self_complementary(const Graph& g,
    SelfComplementaryAlgorithm algo = SelfComplementaryAlgorithm::ISOMORPHISM_CHECK) {
    (void)algo;
    SelfComplementaryResult res;

    int n = g.n;
    if (n == 0) {
        res.is_self_complementary = true;
        return res;
    }
    if (n == 1) {
        res.is_self_complementary = true;
        return res;
    }

    /* n ≡ 0 or 1 (mod 4) */
    if (n % 4 != 0 && n % 4 != 1) return res;

    /* 辺数チェック: m = n(n-1)/4 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != (long long)n * (n - 1) / 4) return res;

    /* 補グラフを構築 */
    std::vector<std::pair<int, int>> comp_edges;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) {
                comp_edges.push_back(std::make_pair(u, v));
            }
        }
    }
    Graph comp(n, comp_edges);

    /* G と complement(G) の同型性チェック */
    if (detail::check_isomorphism_bt(g, comp)) {
        res.is_self_complementary = true;
    }
    return res;
}

} // namespace graph_recognition

#endif
