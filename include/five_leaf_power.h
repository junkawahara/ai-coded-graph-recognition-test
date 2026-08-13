#ifndef GRAPH_RECOGNITION_FIVE_LEAF_POWER_H
#define GRAPH_RECOGNITION_FIVE_LEAF_POWER_H

/**
 * @file five_leaf_power.h
 * @brief 5-leaf power graph recognition
 *
 * A graph G is a 5-leaf power if there exists a tree T whose leaves are
 * the vertices of G, where leaves u, v are adjacent in G iff d(u,v) <= 5 in T.
 *
 * Algorithms:
 *   1. Check if strongly chordal (necessary condition)
 *   2. Compute critical cliques (maximal sets of vertices with identical closed neighborhoods)
 *   3. Build quotient graph Q (verify edges between CCs are all-or-nothing)
 *   4. Search for a realization of Q: a tree on the k quotient nodes whose
 *      edges carry integer weights w_e >= 1 (weight = 1 + number of
 *      subdivisions) together with pendant lengths ell[i] in {1,2} (the
 *      distance from node i to the leaves of its critical clique) such that
 *      for every pair i != j
 *        Q[i][j] = 1  =>  ell[i] + d_w(i,j) + ell[j] <= 5
 *        Q[i][j] = 0  =>  ell[i] + d_w(i,j) + ell[j] >= 6.
 *      Mixed pendant lengths inside one critical clique are dominated by the
 *      uniform ell = 2 choice, so per-clique lengths are WLOG uniform.
 *
 * The search enumerates every labeled tree exactly once by growing a
 * connected subtree (see RealizationSearch::grow) instead of decoding all
 * k^(k-2) Prufer sequences, which allows the distance constraints to prune
 * partial trees: once two nodes are both placed their tree path is final.
 * Weights are bounded by w_e <= 4 (a single edge of weight 4 already
 * separates any pair, so larger weights are never needed) and the pendant
 * lengths and weights are searched with constraint propagation.
 *
 * Known limitation: realization trees requiring Steiner branch nodes that
 * are not critical clique nodes are not modeled (possible false NO for
 * larger graphs; exhaustively correct for n <= 5).
 *
 * References:
 *   - Chang, Ko (2007). Recognition of 5-leaf powers.
 *   - Lafond (2023). General k polynomial time recognition.
 *     ACM Trans. Algorithms.
 */

#include "graph.h"
#include "strongly_chordal.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

struct FiveLeafPowerResult {
    bool is_five_leaf_power = false;
};

namespace detail_five_leaf_power {

/**
 * @brief Backtracking search for a realization of a quotient graph Q
 *
 * Variables: a labeled tree on nodes 0..k-1, an integer weight w_e in
 * [lw_e, 4] for every tree edge and a pendant length ell[i] in {1,2} for
 * every node. lw_e is 1 when the edge joins a Q-adjacent pair and 2
 * otherwise (a Q-non-adjacent pair joined by a tree edge needs
 * ell[a] + w + ell[b] >= 6, hence w >= 2).
 *
 * All distances used while the tree is being grown are the lower bounds
 * obtained from lw, so every prune is valid for every weight assignment.
 */
struct RealizationSearch {
    int k;                              /**< number of quotient nodes */
    std::vector<std::vector<char> > Q;  /**< quotient adjacency (0-based) */

    /* --- tree growth state ------------------------------------------- */
    std::vector<int> ord;        /**< ord[t]: node placed at step t */
    std::vector<char> placed;    /**< placement flags */
    std::vector<int> bnd;        /**< canonical-order bound per placed node */
    std::vector<int> bnd_stack;  /**< saved bnd per depth (k * k) */
    std::vector<int> lwd;        /**< k*k lower-bound weighted distances */
    std::vector<unsigned> pmask; /**< k*k bitmask of the edges on each path */
    std::vector<int> elw;        /**< lower-bound weight per tree edge */

    /* --- pendant lengths --------------------------------------------- */
    std::vector<int> ell;

    /* --- edge weight sub-search -------------------------------------- */
    std::vector<int> hi;            /**< per-edge upper bound on w_e - lw_e */
    std::vector<unsigned> ymask_v;  /**< upper-bound (adjacent) constraints */
    std::vector<int> ybound, ysum;
    std::vector<unsigned> nmask_v;  /**< lower-bound (non-adjacent) constraints */
    std::vector<int> nneed, nsum, remain;
    std::vector<int> rel;                    /**< relevant edges */
    std::vector<std::vector<int> > yes_at;   /**< constraints per relevant edge */
    std::vector<std::vector<int> > no_at;
    int r, ny, nn, nsat;

    /** @brief Allocates the scratch space for a quotient graph with kk nodes */
    void init(const std::vector<std::vector<char> >& q, int kk) {
        k = kk;
        Q = q;
        int m = (k > 0) ? k : 1;
        ord.assign(m, 0);
        placed.assign(m, 0);
        bnd.assign(m, -1);
        bnd_stack.assign(m * m, 0);
        lwd.assign(m * m, 0);
        pmask.assign(m * m, 0u);
        elw.assign(m, 0);
        ell.assign(m, 1);
        hi.assign(m, 0);
        int maxc = m * (m - 1) / 2 + 1;
        ymask_v.assign(maxc, 0u);
        ybound.assign(maxc, 0);
        ysum.assign(maxc, 0);
        nmask_v.assign(maxc, 0u);
        nneed.assign(maxc, 0);
        nsum.assign(maxc, 0);
        remain.assign(maxc, 0);
        rel.assign(m, 0);
        yes_at.assign(m, std::vector<int>());
        no_at.assign(m, std::vector<int>());
        r = ny = nn = nsat = 0;
    }

    /** @brief Runs the search; true iff Q admits a realization */
    bool run() {
        if (k <= 1) return true;
        placed[0] = 1;
        ord[0] = 0;
        bnd[0] = -1;
        return grow(1);
    }

    /**
     * @brief Grows the tree by one node (t nodes are already placed)
     *
     * Every labeled tree is generated exactly once: nodes are added in the
     * canonical order "always attach the smallest node reachable from the
     * current subtree", which is enforced by bnd[u] = the largest node added
     * after u itself joined the subtree (a new child of u must exceed it).
     */
    bool grow(int t) {
        if (t == k) return realize();

        int e = t - 1; /* index of the edge created by this step */
        /* Q-adjacent attachments of the smallest available node first: with
         * the BFS numbering of Q imposed by quotient_realizable this builds
         * a spanning tree of Q on the very first descent, which is the
         * realization whenever Q itself is one. */
        for (int pass = 0; pass < 2; ++pass) {
            for (int v = 0; v < k; ++v) {
                if (placed[v]) continue;
                for (int pi = 0; pi < t; ++pi) {
                    int u = ord[pi];
                    if (v <= bnd[u]) continue;
                    int qadj = Q[u][v] ? 1 : 0;
                    if (qadj != (pass == 0 ? 1 : 0)) continue;
                    int lw = qadj ? 1 : 2;

                    bool ok = true;
                    for (int pj = 0; pj < t; ++pj) {
                        int x = ord[pj];
                        int d = lwd[u * k + x] + lw;
                        /* adjacent pairs need ell[v] + d_w + ell[x] <= 5 and
                         * both pendant lengths are at least 1 */
                        if (Q[v][x] && d > 3) { ok = false; break; }
                        lwd[v * k + x] = lwd[x * k + v] = d;
                        pmask[v * k + x] = pmask[x * k + v] =
                            pmask[u * k + x] | (1u << e);
                    }
                    if (!ok) continue;

                    elw[e] = lw;
                    ord[t] = v;
                    placed[v] = 1;
                    for (int i = 0; i < k; ++i) bnd_stack[t * k + i] = bnd[i];
                    for (int pj = 0; pj < t; ++pj)
                        if (bnd[ord[pj]] < v) bnd[ord[pj]] = v;
                    bnd[v] = -1;

                    if (grow(t + 1)) return true;

                    for (int i = 0; i < k; ++i) bnd[i] = bnd_stack[t * k + i];
                    placed[v] = 0;
                }
            }
        }
        return false;
    }

    /** @brief Searches pendant lengths for the completed tree */
    bool realize() { return ell_dfs(0); }

    /**
     * @brief Assigns ell[i] in {1,2}, pruning on the adjacency upper bounds
     */
    bool ell_dfs(int i) {
        if (i == k) return weights_feasible();
        for (int L = 1; L <= 2; ++L) {
            ell[i] = L;
            bool ok = true;
            for (int j = 0; j < i; ++j) {
                if (Q[i][j] && lwd[i * k + j] + L + ell[j] > 5) { ok = false; break; }
            }
            if (ok && ell_dfs(i + 1)) return true;
        }
        return false;
    }

    /**
     * @brief Decides the edge weights for the current tree and ell
     *
     * With x_e = w_e - lw_e >= 0 the constraints become
     *   sum over path of x_e <= (5 - ell[i] - ell[j]) - lwd(i,j)  (adjacent)
     *   sum over path of x_e >= (6 - ell[i] - ell[j]) - lwd(i,j)  (else)
     * and x_e <= 4 - lw_e. Only edges appearing in an unsatisfied
     * lower-bound constraint need to be searched; raising the others can
     * only violate upper bounds.
     */
    bool weights_feasible() {
        int ne = k - 1;
        for (int e = 0; e < ne; ++e) hi[e] = 4 - elw[e];

        ny = 0;
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                if (!Q[i][j]) continue;
                int slack = (5 - ell[i] - ell[j]) - lwd[i * k + j];
                if (slack < 0) return false;
                unsigned m = pmask[i * k + j];
                ymask_v[ny] = m;
                ybound[ny] = slack;
                ++ny;
                for (int e = 0; e < ne; ++e)
                    if (((m >> e) & 1u) && hi[e] > slack) hi[e] = slack;
            }
        }

        nn = 0;
        unsigned relevant = 0u;
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                if (Q[i][j]) continue;
                int need = (6 - ell[i] - ell[j]) - lwd[i * k + j];
                if (need <= 0) continue;
                unsigned m = pmask[i * k + j];
                int avail = 0;
                for (int e = 0; e < ne; ++e) if ((m >> e) & 1u) avail += hi[e];
                if (avail < need) return false;
                nmask_v[nn] = m;
                nneed[nn] = need;
                ++nn;
                relevant |= m;
            }
        }
        if (nn == 0) return true;

        r = 0;
        for (int e = 0; e < ne; ++e)
            if (((relevant >> e) & 1u) && hi[e] > 0) rel[r++] = e;

        /* upper-bound constraints that cannot be violated are dropped */
        int kept = 0;
        for (int c = 0; c < ny; ++c) {
            int cap = 0;
            for (int p = 0; p < r; ++p)
                if ((ymask_v[c] >> rel[p]) & 1u) cap += hi[rel[p]];
            if (cap > ybound[c]) {
                ymask_v[kept] = ymask_v[c];
                ybound[kept] = ybound[c];
                ++kept;
            }
        }
        ny = kept;

        for (int p = 0; p < r; ++p) {
            yes_at[p].clear();
            no_at[p].clear();
            for (int c = 0; c < ny; ++c)
                if ((ymask_v[c] >> rel[p]) & 1u) yes_at[p].push_back(c);
            for (int c = 0; c < nn; ++c)
                if ((nmask_v[c] >> rel[p]) & 1u) no_at[p].push_back(c);
        }
        for (int c = 0; c < ny; ++c) ysum[c] = 0;
        nsat = 0;
        for (int c = 0; c < nn; ++c) {
            nsum[c] = 0;
            int av = 0;
            for (int p = 0; p < r; ++p)
                if ((nmask_v[c] >> rel[p]) & 1u) av += hi[rel[p]];
            if (av < nneed[c]) return false;
            remain[c] = av;
        }
        return wdfs(0);
    }

    /** @brief Backtracking over the relevant edges' extra weight */
    bool wdfs(int d) {
        if (nsat == nn) return true;
        if (d == r) return false;

        int e = rel[d];
        int cap = hi[e];
        const std::vector<int>& yl = yes_at[d];
        for (size_t t = 0; t < yl.size(); ++t) {
            int room = ybound[yl[t]] - ysum[yl[t]];
            if (room < cap) cap = room;
        }
        const std::vector<int>& nl = no_at[d];
        for (size_t t = 0; t < nl.size(); ++t) remain[nl[t]] -= hi[e];

        bool found = false;
        for (int x = cap; x >= 0 && !found; --x) {
            for (size_t t = 0; t < yl.size(); ++t) ysum[yl[t]] += x;
            int gained = 0;
            bool ok = true;
            for (size_t t = 0; t < nl.size(); ++t) {
                int c = nl[t];
                int before = nsum[c];
                nsum[c] = before + x;
                if (before < nneed[c] && nsum[c] >= nneed[c]) ++gained;
                if (nsum[c] + remain[c] < nneed[c]) ok = false;
            }
            nsat += gained;
            if (ok && wdfs(d + 1)) found = true;
            nsat -= gained;
            for (size_t t = 0; t < nl.size(); ++t) nsum[nl[t]] -= x;
            for (size_t t = 0; t < yl.size(); ++t) ysum[yl[t]] -= x;
        }

        for (size_t t = 0; t < nl.size(); ++t) remain[nl[t]] += hi[e];
        return found;
    }
};

/**
 * @brief Determines whether a quotient graph is realizable
 *
 * Disjoint parts of Q are handled independently: joining the realizations of
 * two parts by an edge of weight 4 keeps every cross pair at distance >= 6,
 * and conversely the subtree spanned by one connected part of Q never passes
 * through a node of another part (two nodes of different parts on one path
 * would both have to be at distance >= 2 from the endpoints of an adjacent
 * pair, which allows at most distance 3 in total).
 */
inline bool quotient_realizable(const std::vector<std::vector<char> >& Q, int k) {
    if (k <= 1) return true;

    std::vector<int> comp(k, -1);
    std::vector<int> stack_;
    int nc = 0;
    for (int s = 0; s < k; ++s) {
        if (comp[s] >= 0) continue;
        comp[s] = nc;
        stack_.clear();
        stack_.push_back(s);
        while (!stack_.empty()) {
            int u = stack_.back();
            stack_.pop_back();
            for (int v = 0; v < k; ++v)
                if (v != u && Q[u][v] && comp[v] < 0) {
                    comp[v] = nc;
                    stack_.push_back(v);
                }
        }
        ++nc;
    }

    RealizationSearch search;
    std::vector<int> members;
    for (int c = 0; c < nc; ++c) {
        /* BFS numbering: every node except the first has a Q-neighbour with a
         * smaller number, so the search reaches a spanning tree of Q first */
        members.clear();
        for (int i = 0; i < k; ++i)
            if (comp[i] == c) { members.push_back(i); break; }
        for (size_t h = 0; h < members.size(); ++h) {
            int u = members[h];
            for (int v = 0; v < k; ++v) {
                if (comp[v] != c || v == u || !Q[u][v]) continue;
                bool have = false;
                for (size_t t = 0; t < members.size() && !have; ++t)
                    if (members[t] == v) have = true;
                if (!have) members.push_back(v);
            }
        }
        int kc = (int)members.size();
        if (kc <= 1) continue;
        std::vector<std::vector<char> > sub(kc, std::vector<char>(kc, 0));
        for (int i = 0; i < kc; ++i) {
            sub[i][i] = 1;
            for (int j = i + 1; j < kc; ++j)
                sub[i][j] = sub[j][i] = Q[members[i]][members[j]];
        }
        search.init(sub, kc);
        if (!search.run()) return false;
    }
    return true;
}

/**
 * @brief Implementation of 5-leaf power recognition
 */
inline FiveLeafPowerResult check_five_leaf_power_impl(const Graph& g) {
    FiveLeafPowerResult res;
    res.is_five_leaf_power = false;

    if (g.n == 0) { res.is_five_leaf_power = true; return res; }

    // 1. Strongly chordal check (5-leaf power is a subclass of strongly chordal)
    StronglyChordalResult scr = check_strongly_chordal(g);
    if (!scr.is_strongly_chordal) return res;

    // 2. Compute critical cliques
    std::vector<std::vector<int> > closed_nbr(g.n + 1);
    for (int v = 1; v <= g.n; ++v) {
        closed_nbr[v] = g.adj[v];
        closed_nbr[v].push_back(v);
        std::sort(closed_nbr[v].begin(), closed_nbr[v].end());
    }

    std::vector<int> order(g.n);
    for (int i = 0; i < g.n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
              [&closed_nbr](int a, int b) {
                  return closed_nbr[a] < closed_nbr[b];
              });

    std::vector<int> cc_id(g.n + 1, -1);
    int num_cc = 0;
    std::vector<std::vector<int> > cc_members;

    for (int i = 0; i < g.n; ) {
        int j = i;
        while (j < g.n && closed_nbr[order[j]] == closed_nbr[order[i]]) {
            cc_id[order[j]] = num_cc;
            ++j;
        }
        cc_members.push_back(std::vector<int>(order.begin() + i, order.begin() + j));
        num_cc++;
        i = j;
    }

    int k = num_cc;

    // 3. Build quotient graph Q
    std::vector<std::vector<char> > Q(k, std::vector<char>(k, 0));
    for (int i = 0; i < k; ++i) Q[i][i] = 1;

    std::vector<int> seen(k, -1);
    for (int ci = 0; ci < k; ++ci) {
        int rep = cc_members[ci][0];
        for (size_t ei = 0; ei < g.adj[rep].size(); ++ei) {
            int w = g.adj[rep][ei];
            int cj = cc_id[w];
            if (cj != ci && seen[cj] != ci) {
                seen[cj] = ci;
                Q[ci][cj] = 1;
                Q[cj][ci] = 1;
            }
        }
    }

    // Consistency check: if Q[ci][cj]=1 then all pairs are adjacent
    for (int ci = 0; ci < k; ++ci) {
        int rep = cc_members[ci][0];
        int my_size = (int)cc_members[ci].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);
        int expected = 0;
        for (int cj = 0; cj < k; ++cj) {
            if (cj != ci && Q[ci][cj])
                expected += (int)cc_members[cj].size();
        }
        if (external_edges != expected) return res;
    }

    // k=1: complete graph -> always a 5-leaf power
    if (k == 1) { res.is_five_leaf_power = true; return res; }

    // 4. Search for a realization of Q
    if (k >= 31) return res; // 32-bit path mask guard; far beyond practical sizes
    res.is_five_leaf_power = quotient_realizable(Q, k);
    return res;
}

} // namespace detail_five_leaf_power

/**
 * @brief Determines whether the graph is a 5-leaf power
 * @param g Input graph
 * @return FiveLeafPowerResult
 */
inline FiveLeafPowerResult check_five_leaf_power(const Graph& g) {
    return detail_five_leaf_power::check_five_leaf_power_impl(g);
}

} // namespace graph_recognition

#endif
