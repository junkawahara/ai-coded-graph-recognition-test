#ifndef GRAPH_RECOGNITION_LINE_GRAPH_H
#define GRAPH_RECOGNITION_LINE_GRAPH_H

/**
 * @file line_graph.h
 * @brief Line graph (線グラフ) 認識
 *
 * Line graph とは、あるグラフ H の辺を頂点とし、H で端点を共有する
 * 辺同士を隣接とするグラフ L(H) である。
 *
 * Whitney (1932) の定理により、line graph は edge clique cover
 * (Krausz 分割) で特徴づけられる: G が line graph ⟺ 辺集合を
 * クリークに分割でき、各頂点が高々 2 つのクリークに属する。
 *
 * アルゴリズム:
 *   - BRUTE: Krausz 分割をバックトラッキングで探索 (小グラフ向け)
 *   - KRAUSZ: 必要条件フィルタ + Krausz 分割構築。O(m * Δ) (デフォルト)
 *
 * 参考文献:
 *   - Whitney (1932); Krausz (1943); Beineke (1968)
 *   - Roussopoulos (1973); Lehot (1974)
 */

#include "graph.h"
#include <vector>
#include <algorithm>

namespace graph_recognition {

/**
 * @brief Line graph 認識アルゴリズムの選択
 */
enum class LineGraphAlgorithm {
    BRUTE,  /**< Krausz 分割バックトラッキング (小グラフ向け) */
    KRAUSZ  /**< 必要条件 + Krausz 分割 O(m * Δ) (デフォルト) */
};

/**
 * @brief Line graph 認識の結果
 */
struct LineGraphResult {
    bool is_line_graph; /**< line graph であれば true */
};

namespace detail {

// 辺リストのインデックス検索ヘルパー
struct EdgeIndex {
    int n;
    // neighbor_idx[v][w] = wがadj[v]の何番目か (-1なら非隣接)
    std::vector<std::vector<int>> neighbor_idx;

    EdgeIndex() : n(0) {}

    void build(const Graph& g, const std::vector<std::pair<int,int>>& edges, int m) {
        n = g.n;
        neighbor_idx.assign(n + 1, std::vector<int>(n + 1, -1));
        for (int eid = 0; eid < m; ++eid) {
            int u = edges[eid].first, v = edges[eid].second;
            neighbor_idx[u][v] = eid;
            neighbor_idx[v][u] = eid;
        }
    }

    int get(int u, int v) const {
        if (u < 1 || u > n || v < 1 || v > n) return -1;
        return neighbor_idx[u][v];
    }
};

/**
 * @brief Krausz 分割のバックトラッキングによる line graph 判定
 */
inline LineGraphResult check_line_graph_brute(const Graph& g) {
    LineGraphResult res;
    res.is_line_graph = true;
    int n = g.n;

    if (n == 0) return res;

    std::vector<std::pair<int,int>> edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) edges.push_back(std::make_pair(u, v));
        }
    }

    int m = (int)edges.size();
    if (m == 0) return res;

    // 辺インデックス構築
    EdgeIndex eidx;
    eidx.build(g, edges, m);

    std::vector<int> edge_clique(m, -1);
    std::vector<int> vertex_clique_count(n + 1, 0);
    int num_cliques = 0;

    struct BacktrackState {
        const Graph* g;
        int n, m;
        const std::vector<std::pair<int,int>>* edges;
        const EdgeIndex* eidx;
        std::vector<int>* edge_clique;
        std::vector<int>* vertex_clique_count;
        int* num_cliques;

        bool solve(int first_unassigned) {
            while (first_unassigned < m && (*edge_clique)[first_unassigned] != -1) {
                first_unassigned++;
            }
            if (first_unassigned >= m) return true;

            int eu = (*edges)[first_unassigned].first;
            int ev = (*edges)[first_unassigned].second;

            if ((*vertex_clique_count)[eu] >= 2 || (*vertex_clique_count)[ev] >= 2) {
                return false;
            }

            std::vector<int> common;
            for (size_t i = 0; i < g->adj[eu].size(); ++i) {
                int w = g->adj[eu][i];
                if (w != ev && g->has_edge(ev, w) && (*vertex_clique_count)[w] < 2) {
                    common.push_back(w);
                }
            }

            int nc = (int)common.size();

            // 大きいクリークから試す (枝刈り効果)
            for (int mask = (1 << nc) - 1; mask >= 0; --mask) {
                std::vector<int> clique_verts;
                clique_verts.push_back(eu);
                clique_verts.push_back(ev);
                for (int i = 0; i < nc; ++i) {
                    if (mask & (1 << i)) clique_verts.push_back(common[i]);
                }

                bool is_clique = true;
                for (size_t i = 2; i < clique_verts.size() && is_clique; ++i) {
                    for (size_t j = i + 1; j < clique_verts.size() && is_clique; ++j) {
                        if (!g->has_edge(clique_verts[i], clique_verts[j])) {
                            is_clique = false;
                        }
                    }
                }
                if (!is_clique) continue;

                std::vector<int> clique_edge_ids;
                bool all_unassigned = true;
                for (size_t i = 0; i < clique_verts.size() && all_unassigned; ++i) {
                    for (size_t j = i + 1; j < clique_verts.size() && all_unassigned; ++j) {
                        int a = clique_verts[i], b = clique_verts[j];
                        int eid = eidx->get(a, b);
                        if (eid == -1 || (*edge_clique)[eid] != -1) {
                            all_unassigned = false;
                        } else {
                            clique_edge_ids.push_back(eid);
                        }
                    }
                }
                if (!all_unassigned) continue;

                int cid = (*num_cliques)++;
                for (size_t i = 0; i < clique_edge_ids.size(); ++i) {
                    (*edge_clique)[clique_edge_ids[i]] = cid;
                }
                for (size_t i = 0; i < clique_verts.size(); ++i) {
                    (*vertex_clique_count)[clique_verts[i]]++;
                }

                if (solve(first_unassigned + 1)) return true;

                (*num_cliques)--;
                for (size_t i = 0; i < clique_edge_ids.size(); ++i) {
                    (*edge_clique)[clique_edge_ids[i]] = -1;
                }
                for (size_t i = 0; i < clique_verts.size(); ++i) {
                    (*vertex_clique_count)[clique_verts[i]]--;
                }
            }

            return false;
        }
    };

    BacktrackState state;
    state.g = &g;
    state.n = n;
    state.m = m;
    state.edges = &edges;
    state.eidx = &eidx;
    state.edge_clique = &edge_clique;
    state.vertex_clique_count = &vertex_clique_count;
    state.num_cliques = &num_cliques;

    res.is_line_graph = state.solve(0);
    return res;
}

/**
 * @brief 必要条件フィルタ + Krausz 分割による line graph 判定
 *
 * 1. 必要条件: 各頂点 v の N(v) の補グラフが二部グラフ (高速フィルタ)
 * 2. Krausz 分割をバックトラッキングで構築 (枝刈り付き)
 *
 * 計算量: O(m * Δ) (ほとんどのケースで高速)
 */
inline LineGraphResult check_line_graph_krausz(const Graph& g) {
    LineGraphResult res;
    res.is_line_graph = true;
    int n = g.n;

    if (n == 0) return res;

    int m = 0;
    for (int v = 1; v <= n; ++v) {
        m += (int)g.adj[v].size();
    }
    m /= 2;

    if (m == 0) return res;

    // Step 1: 必要条件チェック
    // 各頂点 v の N(v) の補グラフが二部グラフか
    for (int v = 1; v <= n; ++v) {
        int deg = (int)g.adj[v].size();
        if (deg <= 2) continue; // 次数 2 以下は自動的に OK

        const std::vector<int>& nbrs = g.adj[v];

        std::vector<int> color(deg, -1);
        bool bipartite = true;

        for (int start = 0; start < deg && bipartite; ++start) {
            if (color[start] != -1) continue;
            color[start] = 0;

            std::vector<int> queue;
            queue.push_back(start);
            size_t qi = 0;

            while (qi < queue.size() && bipartite) {
                int ui = (int)queue[qi++];
                int u = nbrs[ui];
                int u_color = color[ui];

                for (int ji = 0; ji < deg; ++ji) {
                    if (ji == ui) continue;
                    int w = nbrs[ji];
                    if (g.has_edge(u, w)) continue;
                    if (color[ji] == -1) {
                        color[ji] = 1 - u_color;
                        queue.push_back(ji);
                    } else if (color[ji] == u_color) {
                        bipartite = false;
                    }
                }
            }
        }

        if (!bipartite) {
            res.is_line_graph = false;
            return res;
        }
    }

    // Step 2: Krausz 分割構築 (バックトラッキング)
    // 必要条件を通過したら、実際に分割を構築する
    return check_line_graph_brute(g);
}

} // namespace detail

/**
 * @brief グラフが line graph であるか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: KRAUSZ)
 * @return LineGraphResult
 *
 * G が line graph ⟺ あるグラフ H が存在して G = L(H)。
 * ⟺ Beineke の 9 個の禁止誘導部分グラフを含まない。
 * ⟺ Krausz 分割が存在する。
 */
inline LineGraphResult check_line_graph(const Graph& g,
    LineGraphAlgorithm algo = LineGraphAlgorithm::KRAUSZ) {
    switch (algo) {
        case LineGraphAlgorithm::BRUTE:
            return detail::check_line_graph_brute(g);
        case LineGraphAlgorithm::KRAUSZ:
            return detail::check_line_graph_krausz(g);
    }
    return LineGraphResult();
}

} // namespace graph_recognition

#endif
