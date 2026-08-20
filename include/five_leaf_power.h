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
 *   4. G is a 5-leaf power iff Q has a 3-Steiner root: a tree T' that
 *      contains the nodes of Q (plus optional Steiner nodes) such that two
 *      Q-nodes are adjacent iff their T'-distance is at most 3.
 *      - (<=) attach every member of a critical clique as a pendant leaf at
 *        distance 1 below its Q-node: leaf distance = node distance + 2, so
 *        adjacency <=> node distance <= 3.
 *      - (=>) in any realization T of G, all members of one critical clique
 *        hang below a common attachment point; relocating each Q-node to the
 *        point at distance 1 above its leaves turns T into a 3-Steiner root.
 *
 * The 3-Steiner root search enumerates weighted trees on the kq quotient
 * nodes plus s = 0..kq-2 Steiner branch nodes (suppressing degree-2 Steiner
 * nodes into integer edge weights; a tree whose leaves are all quotient
 * nodes has at most kq-2 branch nodes). Every labeled tree is generated
 * exactly once by growing a connected subtree (RealizationSearch::grow),
 * so the distance constraints prune partial trees. Edge weights are
 * bounded by 4 (a non-adjacent pair only ever needs distance >= 4, so an
 * edge of weight 4 already separates everything through it) with lower
 * bound 4 on a direct edge between non-adjacent quotient nodes and 1
 * elsewhere. Steiner nodes carry no constraints but must end with tree
 * degree >= 3.
 *
 * The earlier implementation only enumerated trees whose nodes were the
 * critical cliques themselves (with pendant-length variables ell in {1,2});
 * realizations that need a Steiner branch node of degree >= 3 were missed
 * (false NO, first occurring at n = 6). The pendant lengths are subsumed by
 * the Steiner formulation: an ell = 2 attachment is a Q-node one weighted
 * edge below its former attachment point.
 *
 * References:
 *   - Chang, Ko (2007). Recognition of 5-leaf powers.
 *   - Brandstadt, Le, Sritharan (2008). Structure and linear-time
 *     recognition of 4-leaf powers (2-Steiner roots of quotients).
 *   - Ducoffe (2019). The 4-Steiner root problem (6-leaf powers); states
 *     the general equivalence "k-leaf power <=> CC quotient has a
 *     (k-2)-Steiner root".
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
 * @brief Backtracking search for a 3-Steiner root of a quotient graph Q
 *
 * Nodes 0..kq-1 are the quotient nodes, nodes kq..k-1 are Steiner branch
 * nodes. Variables: a labeled tree on the k nodes and an integer weight
 * w_e in [lw_e, 4] for every tree edge. lw_e is 4 when the edge joins a
 * Q-non-adjacent quotient pair (their path is that single edge, and it
 * must reach length >= 4) and 1 otherwise. Constraints for quotient pairs:
 *   Q[i][j] = 1  =>  d_w(i,j) <= 3
 *   Q[i][j] = 0  =>  d_w(i,j) >= 4.
 * Steiner nodes carry no distance constraints but need final degree >= 3
 * (degree <= 2 Steiner nodes are already modeled by edge weights).
 *
 * All distances used while the tree is being grown are the lower bounds
 * obtained from lw, so every prune is valid for every weight assignment.
 */
struct RealizationSearch {
    int kq;                             /**< number of quotient nodes */
    int k;                              /**< total nodes = kq + Steiner */
    std::vector<std::vector<char> > Q;  /**< quotient adjacency (0-based) */

    /* --- tree growth state ------------------------------------------- */
    std::vector<int> ord;        /**< ord[t]: node placed at step t */
    std::vector<char> placed;    /**< placement flags */
    std::vector<int> bnd;        /**< canonical-order bound per placed node */
    std::vector<int> bnd_stack;  /**< saved bnd per depth (k * k) */
    std::vector<int> lwd;        /**< k*k lower-bound weighted distances */
    std::vector<unsigned long long> pmask; /**< k*k bitmask of the edges on each path */
    std::vector<int> elw;        /**< lower-bound weight per tree edge */
    std::vector<int> deg;        /**< current tree degree per node */
    int deficit;                 /**< sum of max(0, 3 - deg) over placed Steiner nodes */
    int unplaced_steiner;        /**< Steiner nodes not yet in the tree */

    /* --- edge weight sub-search -------------------------------------- */
    std::vector<int> hi;            /**< per-edge upper bound on w_e - lw_e */
    std::vector<unsigned long long> ymask_v;  /**< upper-bound (adjacent) constraints */
    std::vector<int> ybound, ysum;
    std::vector<unsigned long long> nmask_v;  /**< lower-bound (non-adjacent) constraints */
    std::vector<int> nneed, nsum, remain;
    std::vector<int> rel;                    /**< relevant edges */
    std::vector<std::vector<int> > yes_at;   /**< constraints per relevant edge */
    std::vector<std::vector<int> > no_at;
    int r, ny, nn, nsat;

    /** @brief Allocates the scratch space: q has kk quotient nodes, and the
     *         tree additionally contains steiner_cnt Steiner nodes */
    void init(const std::vector<std::vector<char> >& q, int kk, int steiner_cnt) {
        kq = kk;
        k = kk + steiner_cnt;
        Q = q;
        int m = (k > 0) ? k : 1;
        ord.assign(m, 0);
        placed.assign(m, 0);
        bnd.assign(m, -1);
        bnd_stack.assign(m * m, 0);
        lwd.assign(m * m, 0);
        pmask.assign(m * m, 0ull);
        elw.assign(m, 0);
        deg.assign(m, 0);
        hi.assign(m, 0);
        int maxc = kq * (kq - 1) / 2 + 1;
        ymask_v.assign(maxc, 0ull);
        ybound.assign(maxc, 0);
        ysum.assign(maxc, 0);
        nmask_v.assign(maxc, 0ull);
        nneed.assign(maxc, 0);
        nsum.assign(maxc, 0);
        remain.assign(maxc, 0);
        rel.assign(m, 0);
        yes_at.assign(m, std::vector<int>());
        no_at.assign(m, std::vector<int>());
        r = ny = nn = nsat = 0;
    }

    /** @brief Runs the search; true iff Q admits a 3-Steiner root with the
     *         configured number of Steiner branch nodes */
    bool run() {
        if (k <= 1) return true;
        placed[0] = 1;
        ord[0] = 0;
        bnd[0] = -1;
        deg[0] = 0;
        deficit = 0;
        unplaced_steiner = k - kq;
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
         * realization whenever Q itself is one. Steiner nodes join in the
         * second pass. */
        for (int pass = 0; pass < 2; ++pass) {
            for (int v = 0; v < k; ++v) {
                if (placed[v]) continue;
                for (int pi = 0; pi < t; ++pi) {
                    int u = ord[pi];
                    if (v <= bnd[u]) continue;
                    bool qq = (u < kq && v < kq);
                    int qadj = (qq && Q[u][v]) ? 1 : 0;
                    if (qadj != (pass == 0 ? 1 : 0)) continue;
                    int lw = qq ? (qadj ? 1 : 4) : 1;

                    /* Degree-3 feasibility for Steiner nodes: each of the
                     * rem placements still to come adds one edge and can
                     * lower the total deficit by at most 1, while every
                     * still-unplaced Steiner node will add 2 on arrival. */
                    int ddelta = 0;
                    if (u >= kq && deg[u] < 3) ddelta -= 1;
                    if (v >= kq) ddelta += 2;
                    int rs = unplaced_steiner - (v >= kq ? 1 : 0);
                    int rem = k - t - 1;
                    if (deficit + ddelta + 2 * rs > rem) continue;

                    bool ok = true;
                    for (int pj = 0; pj < t; ++pj) {
                        int x = ord[pj];
                        int d = lwd[u * k + x] + lw;
                        /* adjacent quotient pairs need d_w <= 3 */
                        if (v < kq && x < kq && Q[v][x] && d > 3) { ok = false; break; }
                        lwd[v * k + x] = lwd[x * k + v] = d;
                        pmask[v * k + x] = pmask[x * k + v] =
                            pmask[u * k + x] | (1ull << e);
                    }
                    if (!ok) continue;

                    elw[e] = lw;
                    ord[t] = v;
                    placed[v] = 1;
                    deg[u]++;
                    deg[v] = 1;
                    deficit += ddelta;
                    unplaced_steiner = rs;
                    for (int i = 0; i < k; ++i) bnd_stack[t * k + i] = bnd[i];
                    for (int pj = 0; pj < t; ++pj)
                        if (bnd[ord[pj]] < v) bnd[ord[pj]] = v;
                    bnd[v] = -1;

                    if (grow(t + 1)) return true;

                    for (int i = 0; i < k; ++i) bnd[i] = bnd_stack[t * k + i];
                    placed[v] = 0;
                    deg[u]--;
                    deficit -= ddelta;
                    unplaced_steiner = rs + (v >= kq ? 1 : 0);
                }
            }
        }
        return false;
    }

    /** @brief Rejects Steiner nodes of degree <= 2, then searches weights */
    bool realize() {
        for (int i = kq; i < k; ++i)
            if (deg[i] < 3) return false;
        return weights_feasible();
    }

    /**
     * @brief Decides the edge weights for the current tree
     *
     * With x_e = w_e - lw_e >= 0 the constraints become
     *   sum over path of x_e <= 3 - lwd(i,j)  (adjacent quotient pairs)
     *   sum over path of x_e >= 4 - lwd(i,j)  (non-adjacent quotient pairs)
     * and x_e <= 4 - lw_e. Only edges appearing in an unsatisfied
     * lower-bound constraint need to be searched; raising the others can
     * only violate upper bounds.
     */
    bool weights_feasible() {
        int ne = k - 1;
        for (int e = 0; e < ne; ++e) hi[e] = 4 - elw[e];

        ny = 0;
        for (int i = 0; i < kq; ++i) {
            for (int j = i + 1; j < kq; ++j) {
                if (!Q[i][j]) continue;
                int slack = 3 - lwd[i * k + j];
                if (slack < 0) return false;
                unsigned long long m = pmask[i * k + j];
                ymask_v[ny] = m;
                ybound[ny] = slack;
                ++ny;
                for (int e = 0; e < ne; ++e)
                    if (((m >> e) & 1ull) && hi[e] > slack) hi[e] = slack;
            }
        }

        nn = 0;
        unsigned long long relevant = 0ull;
        for (int i = 0; i < kq; ++i) {
            for (int j = i + 1; j < kq; ++j) {
                if (Q[i][j]) continue;
                int need = 4 - lwd[i * k + j];
                if (need <= 0) continue;
                unsigned long long m = pmask[i * k + j];
                int avail = 0;
                for (int e = 0; e < ne; ++e) if ((m >> e) & 1ull) avail += hi[e];
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
            if (((relevant >> e) & 1ull) && hi[e] > 0) rel[r++] = e;

        /* upper-bound constraints that cannot be violated are dropped */
        int kept = 0;
        for (int c = 0; c < ny; ++c) {
            int cap = 0;
            for (int p = 0; p < r; ++p)
                if ((ymask_v[c] >> rel[p]) & 1ull) cap += hi[rel[p]];
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
                if ((ymask_v[c] >> rel[p]) & 1ull) yes_at[p].push_back(c);
            for (int c = 0; c < nn; ++c)
                if ((nmask_v[c] >> rel[p]) & 1ull) no_at[p].push_back(c);
        }
        for (int c = 0; c < ny; ++c) ysum[c] = 0;
        nsat = 0;
        for (int c = 0; c < nn; ++c) {
            nsum[c] = 0;
            int av = 0;
            for (int p = 0; p < r; ++p)
                if ((nmask_v[c] >> rel[p]) & 1ull) av += hi[rel[p]];
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
 * @brief Determines whether a quotient graph has a 3-Steiner root
 *
 * Disjoint parts of Q are handled independently: joining the realizations of
 * two parts by an edge of weight 4 keeps every cross pair at distance >= 4,
 * and conversely the subtree spanned by one connected part of Q never passes
 * through a node of another part (two nodes of different parts on one path
 * would both have to be within distance 3 of the endpoints of an adjacent
 * pair, contradicting the >= 4 separation of the parts).
 *
 * Per connected part with kc nodes, the number of Steiner branch nodes is
 * tried from 0 up to kc - 2 (every leaf of a 3-Steiner root can be assumed
 * to be a quotient node, so at most kc - 2 nodes of degree >= 3 exist).
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
        int max_steiner = (kc >= 3) ? kc - 2 : 0;
        bool found = false;
        for (int s = 0; s <= max_steiner && !found; ++s) {
            /* 64-bit path masks: k-1 edges must fit in an unsigned long
             * long. Components that large are far beyond what the
             * exponential search could finish anyway. */
            if (kc + s > 64) break;
            search.init(sub, kc, s);
            if (search.run()) found = true;
        }
        if (!found) return false;
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

    // 4. Search for a 3-Steiner root of Q
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
