#ifndef GRAPH_RECOGNITION_CIRCULAR_ARC_H
#define GRAPH_RECOGNITION_CIRCULAR_ARC_H

/**
 * @file circular_arc.h
 * @brief 円弧グラフ (circular-arc graph) 認識
 *
 * アルゴリズム:
 *   - MCCONNELL: 極大クリーク列挙 + 円環クリーク順序 (多項式時間, デフォルト)
 *   - BACKTRACKING: 端点順序バックトラッキング + 2-SAT (指数時間, 小規模向け)
 */

#include "at_free.h"
#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 円弧グラフ認識アルゴリズムの選択
 */
enum class CircularArcAlgorithm {
    MCCONNELL,    /**< 極大クリーク列挙 + 円環クリーク順序 (デフォルト) */
    BACKTRACKING  /**< バックトラッキング + 2-SAT 枝刈り */
};

/**
 * @brief 円弧グラフ認識の結果
 */
struct CircularArcResult {
    bool is_circular_arc; /**< 円弧グラフであれば true */
};

namespace detail_circular_arc {

// ===== McConnell algorithm =====

struct GeneralMaxCliques {
    std::vector<std::vector<int>> cliques;
    std::vector<std::vector<int>> member; // member[v] = clique indices containing v
    bool exceeded;
};

inline void bron_kerbosch_pivot(
    std::vector<int>& R,
    std::vector<int>& P,
    std::vector<int>& X,
    const Graph& g,
    GeneralMaxCliques& result,
    int max_cliques) {

    if (result.exceeded) return;

    if (P.empty() && X.empty()) {
        result.cliques.push_back(R);
        if ((int)result.cliques.size() > max_cliques) {
            result.exceeded = true;
        }
        return;
    }

    // Choose pivot u from P ∪ X maximizing |P ∩ N(u)|
    int pivot = -1, best = -1;
    for (size_t i = 0; i < P.size(); ++i) {
        int u = P[i];
        int cnt = 0;
        for (size_t j = 0; j < P.size(); ++j) {
            if (g.adj_set[u].count(P[j])) cnt++;
        }
        if (cnt > best) { best = cnt; pivot = u; }
    }
    for (size_t i = 0; i < X.size(); ++i) {
        int u = X[i];
        int cnt = 0;
        for (size_t j = 0; j < P.size(); ++j) {
            if (g.adj_set[u].count(P[j])) cnt++;
        }
        if (cnt > best) { best = cnt; pivot = u; }
    }

    // Iterate over P \ N(pivot)
    std::vector<int> candidates;
    for (size_t i = 0; i < P.size(); ++i) {
        if (!g.adj_set[pivot].count(P[i])) {
            candidates.push_back(P[i]);
        }
    }

    for (size_t ci = 0; ci < candidates.size(); ++ci) {
        if (result.exceeded) return;
        int v = candidates[ci];

        R.push_back(v);
        std::vector<int> newP, newX;
        for (size_t i = 0; i < P.size(); ++i) {
            if (g.adj_set[v].count(P[i])) newP.push_back(P[i]);
        }
        for (size_t i = 0; i < X.size(); ++i) {
            if (g.adj_set[v].count(X[i])) newX.push_back(X[i]);
        }

        bron_kerbosch_pivot(R, newP, newX, g, result, max_cliques);

        R.pop_back();
        P.erase(std::find(P.begin(), P.end(), v));
        X.push_back(v);
    }
}

inline GeneralMaxCliques enumerate_maximal_cliques_general(
    const Graph& g, const std::vector<int>& vertices, int max_cliques) {

    GeneralMaxCliques mc;
    mc.exceeded = false;
    mc.member.resize(g.n + 1);

    std::vector<int> R, P(vertices), X;
    bron_kerbosch_pivot(R, P, X, g, mc, max_cliques);

    if (!mc.exceeded) {
        for (int i = 0; i < (int)mc.cliques.size(); ++i) {
            for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
                mc.member[mc.cliques[i][j]].push_back(i);
            }
        }
    }
    return mc;
}

inline bool verify_circular_consecutive(
    const std::vector<int>& order,
    int n,
    const GeneralMaxCliques& mc) {

    int k = (int)order.size();
    if (k <= 2) return true;

    std::vector<int> pos(k);
    for (int i = 0; i < k; ++i) pos[order[i]] = i;

    for (int v = 1; v <= n; ++v) {
        int sz = (int)mc.member[v].size();
        if (sz <= 1) continue;

        std::vector<int> positions(sz);
        for (int i = 0; i < sz; ++i) {
            positions[i] = pos[mc.member[v][i]];
        }
        std::sort(positions.begin(), positions.end());

        // Check if positions form a consecutive arc in Z_k
        // Find the largest gap between consecutive positions (including wrap)
        int max_gap = (k - positions[sz - 1]) + positions[0] - 1;
        for (int i = 1; i < sz; ++i) {
            int gap = positions[i] - positions[i - 1] - 1;
            if (gap > max_gap) max_gap = gap;
        }
        // positions should cover exactly sz positions, so the complement gap = k - sz
        if (max_gap != k - sz) return false;
    }
    return true;
}

/**
 * @brief 円環クリーク順序をバックトラッキングで探索する
 *
 * interval.h の find_clique_path を円環版に適応。
 * 開始クリークの頂点はラップアラウンド（最後のクリークに再出現）が
 * 許されるため、"must continue" 制約から免除される。
 */
inline bool find_circular_clique_path(
    int k, int n,
    std::vector<int>& clique_order,
    std::vector<bool>& placed,
    std::vector<bool>& finished,
    std::vector<int>& unplaced_count,
    const std::vector<bool>& is_start_vert,
    const GeneralMaxCliques& mc,
    const std::vector<std::unordered_set<int>>& cset)
{
    if ((int)clique_order.size() == k) {
        return verify_circular_consecutive(clique_order, n, mc);
    }

    int cur = clique_order.back();

    // "active" = vertices in cur with remaining cliques, excluding start-clique vertices
    std::vector<int> active;
    for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
        int v = mc.cliques[cur][j];
        if (unplaced_count[v] > 0 && !is_start_vert[v]) active.push_back(v);
    }

    if (active.empty()) {
        // No forced continuation: try any unplaced clique without finished vertices
        for (int c = 0; c < k; ++c) {
            if (placed[c]) continue;
            bool ok = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
                if (finished[mc.cliques[c][j]]) { ok = false; break; }
            }
            if (!ok) continue;

            clique_order.push_back(c);
            placed[c] = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]--;

            // Mark vertices in cur that have left the sequence permanently
            std::vector<int> newly_finished;
            for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
                int v = mc.cliques[cur][j];
                if (!finished[v] && unplaced_count[v] == 0) {
                    finished[v] = true;
                    newly_finished.push_back(v);
                }
            }

            if (find_circular_clique_path(k, n, clique_order, placed, finished,
                                          unplaced_count, is_start_vert, mc, cset))
                return true;

            for (size_t j = 0; j < newly_finished.size(); ++j)
                finished[newly_finished[j]] = false;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]++;
            placed[c] = false;
            clique_order.pop_back();
        }
        return false;
    }

    // Has active vertices: next clique must contain all of them
    for (int c = 0; c < k; ++c) {
        if (placed[c]) continue;

        bool ok = true;
        for (size_t j = 0; j < active.size(); ++j) {
            if (cset[c].count(active[j]) == 0) { ok = false; break; }
        }
        if (!ok) continue;

        for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
            if (finished[mc.cliques[c][j]]) { ok = false; break; }
        }
        if (!ok) continue;

        clique_order.push_back(c);
        placed[c] = true;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]--;

        // Mark vertices that leave: in cur, not in c, remaining==0
        std::vector<int> newly_finished;
        for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
            int v = mc.cliques[cur][j];
            if (cset[c].count(v) == 0 && !finished[v] && unplaced_count[v] == 0) {
                finished[v] = true;
                newly_finished.push_back(v);
            }
        }

        if (find_circular_clique_path(k, n, clique_order, placed, finished,
                                      unplaced_count, is_start_vert, mc, cset))
            return true;

        for (size_t j = 0; j < newly_finished.size(); ++j)
            finished[newly_finished[j]] = false;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]++;
        placed[c] = false;
        clique_order.pop_back();
    }
    return false;
}

inline CircularArcResult check_circular_arc_mcconnell(const Graph& g) {
    CircularArcResult res;
    res.is_circular_arc = false;
    int n = g.n;

    if (n <= 2) {
        res.is_circular_arc = true;
        return res;
    }

    // Find connected components
    std::vector<int> comp_id(n + 1, -1);
    std::vector<std::vector<int>> components;
    for (int v = 1; v <= n; ++v) {
        if (comp_id[v] >= 0) continue;
        int cid = (int)components.size();
        components.push_back(std::vector<int>());
        std::queue<int> q;
        q.push(v);
        comp_id[v] = cid;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            components[cid].push_back(u);
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (comp_id[w] < 0) {
                    comp_id[w] = cid;
                    q.push(w);
                }
            }
        }
    }

    // Disconnected graph: circular-arc iff all components are interval graphs
    // (In a circular-arc model, a non-interval component covers the full
    //  circle, leaving no room for other components' arcs.)
    if (components.size() > 1) {
        ChordalResult chordal = check_chordal(g);
        if (!chordal.is_chordal) return res;
        if (detail::has_asteroidal_triple(g)) return res;
        res.is_circular_arc = true;
        return res;
    }

    // Connected graph: check C1CP via clique enumeration + circular ordering
    {
        const std::vector<int>& verts = components[0];
        int nv = (int)verts.size();

        if (nv <= 2) {
            res.is_circular_arc = true;
            return res;
        }

        // Enumerate maximal cliques (limit 2n)
        GeneralMaxCliques mc = enumerate_maximal_cliques_general(g, verts, 2 * nv);
        if (mc.exceeded) return res; // too many cliques -> not circular-arc

        int k = (int)mc.cliques.size();
        if (k <= 2) {
            res.is_circular_arc = true;
            return res;
        }

        // Build clique sets for efficient lookup
        std::vector<std::unordered_set<int>> cset(k);
        for (int i = 0; i < k; ++i) {
            cset[i].reserve(mc.cliques[i].size() * 2 + 1);
            for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
                cset[i].insert(mc.cliques[i][j]);
            }
        }

        // Try each starting clique with backtracking
        std::vector<int> unplaced_count(n + 1, 0);
        for (int v = 1; v <= n; ++v) {
            unplaced_count[v] = (int)mc.member[v].size();
        }

        bool found = false;
        std::vector<int> starts;
        for (int i = 0; i < k; ++i) {
            for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
                if ((int)mc.member[mc.cliques[i][j]].size() == 1) {
                    starts.push_back(i);
                    break;
                }
            }
        }
        if (starts.empty()) {
            starts.push_back(0);
        }

        for (size_t si = 0; si < starts.size() && !found; ++si) {
            int s = starts[si];
            std::vector<int> order;
            order.reserve(k);
            order.push_back(s);

            std::vector<bool> placed(k, false);
            placed[s] = true;

            std::vector<bool> finished(n + 1, false);

            std::vector<bool> is_start_vert(n + 1, false);
            for (size_t j = 0; j < mc.cliques[s].size(); ++j) {
                is_start_vert[mc.cliques[s][j]] = true;
            }

            std::vector<int> ucount = unplaced_count;
            for (size_t j = 0; j < mc.cliques[s].size(); ++j) {
                ucount[mc.cliques[s][j]]--;
            }

            if (find_circular_clique_path(k, n, order, placed, finished,
                                          ucount, is_start_vert, mc, cset)) {
                found = true;
            }
        }

        if (!found) return res;

        res.is_circular_arc = true;
        return res;
    }
}

// ===== Backtracking algorithm (original) =====

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

inline CircularArcResult check_circular_arc_backtracking(const Graph& g) {
    CircularArcResult res;
    res.is_circular_arc = false;

    int n = g.n;
    if (n <= 2) {
        res.is_circular_arc = true;
        return res;
    }

    std::vector<std::vector<unsigned char>> adj = build_adj_matrix(g);

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
    if (search_endpoint_order(
            place_order, 1, adj,
            seq, pos_first, pos_second, placed,
            &out_seq, &out_pos_first, &out_pos_second)) {
        res.is_circular_arc = true;
    }

    return res;
}

} // namespace detail_circular_arc

/**
 * @brief グラフが円弧グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: MCCONNELL)
 * @return CircularArcResult
 */
inline CircularArcResult check_circular_arc(const Graph& g,
    CircularArcAlgorithm algo = CircularArcAlgorithm::MCCONNELL) {
    switch (algo) {
        case CircularArcAlgorithm::MCCONNELL:
            return detail_circular_arc::check_circular_arc_mcconnell(g);
        case CircularArcAlgorithm::BACKTRACKING:
            return detail_circular_arc::check_circular_arc_backtracking(g);
    }
    return CircularArcResult();
}

} // namespace graph_recognition

#endif
