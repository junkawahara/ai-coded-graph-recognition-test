#ifndef GRAPH_RECOGNITION_CIRCULAR_ARC_H
#define GRAPH_RECOGNITION_CIRCULAR_ARC_H

/**
 * @file circular_arc.h
 * @brief 円弧グラフ (circular-arc graph) 認識
 *
 * 円周上の端点順序をバックトラッキングで探索し、
 * 各頂点の向き (2 値) を 2-SAT で判定する。
 */

#include "graph.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 円弧グラフ認識アルゴリズムの選択
 */
enum class CircularArcAlgorithm {
    MCCONNELL /**< McConnell のアルゴリズム */
};

/**
 * @brief 円弧グラフ認識の結果
 */
struct CircularArcResult {
    bool is_circular_arc; /**< 円弧グラフであれば true */
};

namespace detail_circular_arc {

inline std::vector<std::vector<unsigned char>> build_adj_matrix(const Graph& g) {
    std::vector<std::vector<unsigned char>> a(
        g.n + 1, std::vector<unsigned char>(g.n + 1, 0));
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            a[u][v] = 1;
        }
    }
    return a;
}

inline bool is_alternating(int a1, int a2, int b1, int b2) {
    bool b1_in = (a1 < b1 && b1 < a2);
    bool b2_in = (a1 < b2 && b2 < a2);
    return b1_in != b2_in;
}

inline void build_sequence_after_insertion(
    const std::vector<int>& seq,
    int v,
    int g1,
    int g2,
    std::vector<int>* out) {
    int len = (int)seq.size();
    out->clear();
    out->reserve(len + 2);
    for (int gap = 0; gap <= len; ++gap) {
        if (gap == g1) out->push_back(v);
        if (gap == g2) out->push_back(v);
        if (gap < len) out->push_back(seq[gap]);
    }
}

inline int shifted_pos(int pos, int g1, int g2) {
    int s = 0;
    if (pos >= g1) s++;
    if (pos >= g2) s++;
    return pos + s;
}

inline void dfs1(
    int v,
    const std::vector<std::vector<int>>& g,
    std::vector<unsigned char>* vis,
    std::vector<int>* order) {
    (*vis)[v] = 1;
    for (size_t i = 0; i < g[v].size(); ++i) {
        int to = g[v][i];
        if (!(*vis)[to]) dfs1(to, g, vis, order);
    }
    order->push_back(v);
}

inline void dfs2(
    int v,
    int cid,
    const std::vector<std::vector<int>>& rg,
    std::vector<int>* comp) {
    (*comp)[v] = cid;
    for (size_t i = 0; i < rg[v].size(); ++i) {
        int to = rg[v][i];
        if ((*comp)[to] == -1) dfs2(to, cid, rg, comp);
    }
}

inline bool orientation_feasible(
    const std::vector<int>& verts,
    const std::vector<int>& pos_first,
    const std::vector<int>& pos_second,
    const std::vector<std::vector<unsigned char>>& adj,
    int len) {
    int k = (int)verts.size();
    if (k <= 1) return true;

    std::vector<std::vector<unsigned char>> active0(
        k, std::vector<unsigned char>(len, 0));
    std::vector<std::vector<unsigned char>> active1(
        k, std::vector<unsigned char>(len, 0));

    for (int i = 0; i < k; ++i) {
        int v = verts[i];
        int a = pos_first[v];
        int b = pos_second[v];
        if (a > b) std::swap(a, b);
        for (int s = 0; s < len; ++s) {
            bool in_short = (a <= s && s < b);
            active0[i][s] = in_short ? 1 : 0;
            active1[i][s] = in_short ? 0 : 1;
        }
    }

    int nlit = 2 * k;
    std::vector<std::vector<int>> g(nlit), rg(nlit);

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int u = verts[i], v = verts[j];

            bool allowed[2][2];
            bool any_allowed = false;
            for (int xu = 0; xu < 2; ++xu) {
                for (int xv = 0; xv < 2; ++xv) {
                    bool inter = false;
                    for (int s = 0; s < len; ++s) {
                        unsigned char au = (xu == 0) ? active0[i][s] : active1[i][s];
                        unsigned char av = (xv == 0) ? active0[j][s] : active1[j][s];
                        if (au && av) {
                            inter = true;
                            break;
                        }
                    }
                    allowed[xu][xv] = adj[u][v] ? inter : !inter;
                    if (allowed[xu][xv]) any_allowed = true;
                }
            }
            if (!any_allowed) return false;

            for (int xu = 0; xu < 2; ++xu) {
                for (int xv = 0; xv < 2; ++xv) {
                    if (allowed[xu][xv]) continue;

                    int lit_u_neq = (xu == 0) ? (2 * i) : (2 * i + 1);
                    int lit_v_neq = (xv == 0) ? (2 * j) : (2 * j + 1);
                    int not_u = lit_u_neq ^ 1;
                    int not_v = lit_v_neq ^ 1;

                    g[not_u].push_back(lit_v_neq);
                    rg[lit_v_neq].push_back(not_u);
                    g[not_v].push_back(lit_u_neq);
                    rg[lit_u_neq].push_back(not_v);
                }
            }
        }
    }

    std::vector<unsigned char> vis(nlit, 0);
    std::vector<int> order;
    order.reserve(nlit);
    for (int v = 0; v < nlit; ++v) {
        if (!vis[v]) dfs1(v, g, &vis, &order);
    }

    std::vector<int> comp(nlit, -1);
    int cid = 0;
    for (int i = nlit - 1; i >= 0; --i) {
        int v = order[i];
        if (comp[v] != -1) continue;
        dfs2(v, cid++, rg, &comp);
    }

    for (int i = 0; i < k; ++i) {
        if (comp[2 * i] == comp[2 * i + 1]) return false;
    }
    return true;
}

inline bool search_endpoint_order(
    const std::vector<int>& place_order,
    int idx,
    const std::vector<std::vector<unsigned char>>& adj,
    const std::vector<int>& seq,
    const std::vector<int>& pos_first,
    const std::vector<int>& pos_second,
    const std::vector<int>& placed,
    std::vector<int>* out_seq,
    std::vector<int>* out_pos_first,
    std::vector<int>* out_pos_second) {
    int n = (int)place_order.size();
    if (idx == n) {
        *out_seq = seq;
        *out_pos_first = pos_first;
        *out_pos_second = pos_second;
        return true;
    }

    int v = place_order[idx];
    int len = (int)seq.size();

    for (int g1 = 0; g1 <= len; ++g1) {
        for (int g2 = g1; g2 <= len; ++g2) {
            int p1 = g1;
            int p2 = g2 + 1;

            bool ok = true;
            for (size_t i = 0; i < placed.size(); ++i) {
                int u = placed[i];
                if (adj[u][v]) continue;

                int u1 = shifted_pos(pos_first[u], g1, g2);
                int u2 = shifted_pos(pos_second[u], g1, g2);
                if (u1 > u2) std::swap(u1, u2);

                if (is_alternating(u1, u2, p1, p2)) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;

            std::vector<int> next_seq;
            build_sequence_after_insertion(seq, v, g1, g2, &next_seq);

            std::vector<int> next_pos_first = pos_first;
            std::vector<int> next_pos_second = pos_second;
            for (size_t i = 0; i < placed.size(); ++i) {
                int u = placed[i];
                next_pos_first[u] = shifted_pos(pos_first[u], g1, g2);
                next_pos_second[u] = shifted_pos(pos_second[u], g1, g2);
            }
            next_pos_first[v] = p1;
            next_pos_second[v] = p2;

            std::vector<int> next_placed = placed;
            next_placed.push_back(v);

            if (!orientation_feasible(
                    next_placed, next_pos_first, next_pos_second,
                    adj, len + 2)) {
                continue;
            }

            if (search_endpoint_order(
                    place_order, idx + 1, adj,
                    next_seq, next_pos_first, next_pos_second, next_placed,
                    out_seq, out_pos_first, out_pos_second)) {
                return true;
            }
        }
    }

    return false;
}

} // namespace detail_circular_arc

/**
 * @brief グラフが円弧グラフか判定する
 * @param g 入力グラフ
 * @return CircularArcResult
 */
inline CircularArcResult check_circular_arc(const Graph& g,
    CircularArcAlgorithm algo = CircularArcAlgorithm::MCCONNELL) {
    (void)algo;
    CircularArcResult res;
    res.is_circular_arc = false;

    int n = g.n;
    if (n <= 2) {
        res.is_circular_arc = true;
        return res;
    }

    std::vector<std::vector<unsigned char>> adj =
        detail_circular_arc::build_adj_matrix(g);

    std::vector<int> place_order;
    place_order.reserve(n);
    for (int v = 1; v <= n; ++v) place_order.push_back(v);

    std::sort(place_order.begin(), place_order.end(),
              [&](int a, int b) {
                  int na = n - 1 - (int)g.adj[a].size();
                  int nb = n - 1 - (int)g.adj[b].size();
                  if (na != nb) return na > nb;
                  if (g.adj[a].size() != g.adj[b].size()) {
                      return g.adj[a].size() > g.adj[b].size();
                  }
                  return a < b;
              });

    int root = place_order[0];
    std::vector<int> seq;
    seq.push_back(root);
    seq.push_back(root);

    std::vector<int> pos_first(n + 1, -1), pos_second(n + 1, -1);
    pos_first[root] = 0;
    pos_second[root] = 1;

    std::vector<int> placed;
    placed.push_back(root);

    std::vector<int> out_seq, out_pos_first, out_pos_second;
    if (detail_circular_arc::search_endpoint_order(
            place_order, 1, adj,
            seq, pos_first, pos_second, placed,
            &out_seq, &out_pos_first, &out_pos_second)) {
        res.is_circular_arc = true;
    }

    return res;
}

} // namespace graph_recognition

#endif
