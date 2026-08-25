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
 * Search budget: the tree search is exponential in the worst case, and NO
 * instances can otherwise run essentially forever. check_five_leaf_power
 * therefore counts search steps against a budget (default
 * five_leaf_power_default_budget, a few seconds of work; pass 0 for
 * unlimited). Exhausting the budget throws std::runtime_error -- an
 * explicit "could not decide", never a silent wrong NO. Easy instances
 * (in particular every actual 5-leaf power whose quotient spanning tree is
 * found on the first descent, e.g. long paths) stay far below the budget.
 * A per-component memory guard on the O(k^2 * k/64) path-mask table throws
 * the same way for quotient components too large to search at all.
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
#include "twins.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace graph_recognition {

struct FiveLeafPowerResult {
    bool is_five_leaf_power = false;
};

/** @brief Default step budget for the 3-Steiner root search (roughly a few
 *         seconds of work). Exceeding it throws std::runtime_error; pass 0
 *         to check_five_leaf_power for an unlimited search. */
const unsigned long long five_leaf_power_default_budget = 50000000ULL;

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
    typedef unsigned long long U64;

    int kq;                             /**< number of quotient nodes */
    int k;                              /**< total nodes = kq + Steiner */
    int mw;                             /**< 64-bit words per edge-set mask */
    std::vector<std::vector<char> > Q;  /**< quotient adjacency (0-based) */

    /* --- tree growth state ------------------------------------------- */
    std::vector<int> ord;        /**< ord[t]: node placed at step t */
    std::vector<char> placed;    /**< placement flags */
    std::vector<int> bnd;        /**< canonical-order bound per placed node */
    std::vector<int> bnd_stack;  /**< saved bnd per depth (k * k) */
    std::vector<int> lwd;        /**< k*k lower-bound weighted distances */
    std::vector<U64> pmask;      /**< k*k masks (mw words each) of the edges on each path */
    std::vector<int> elw;        /**< lower-bound weight per tree edge */
    std::vector<int> deg;        /**< current tree degree per node */
    int deficit;                 /**< sum of max(0, 3 - deg) over placed Steiner nodes */
    int unplaced_steiner;        /**< Steiner nodes not yet in the tree */

    /* --- edge weight sub-search -------------------------------------- */
    std::vector<int> hi;            /**< per-edge upper bound on w_e - lw_e */
    std::vector<U64> ymask_v;       /**< upper-bound (adjacent) constraints (mw words each) */
    std::vector<int> ybound, ysum;
    std::vector<U64> nmask_v;       /**< lower-bound (non-adjacent) constraints (mw words each) */
    std::vector<int> nneed, nsum, remain;
    std::vector<U64> relevant_;     /**< scratch: union of unsatisfied lower-bound masks */
    std::vector<int> rel;                    /**< relevant edges */
    std::vector<std::vector<int> > yes_at;   /**< constraints per relevant edge */
    std::vector<std::vector<int> > no_at;
    int r, ny, nn, nsat;

    /* --- work budget (shared across init() calls) -------------------- */
    unsigned long long steps = 0;      /**< grow/wdfs invocations so far */
    unsigned long long step_limit = 0; /**< abort threshold; 0 = unlimited */
    bool aborted = false;              /**< set when step_limit was exhausted */

    static bool mask_test(const U64* m, int e) {
        return ((m[e >> 6] >> (e & 63)) & 1ull) != 0;
    }

    /** @brief Allocates the scratch space: q has kk quotient nodes, and the
     *         tree additionally contains steiner_cnt Steiner nodes.
     *         Throws std::runtime_error when the component is too large for
     *         the O(k^2) mask tables (explicit refusal, never a silent NO). */
    void init(const std::vector<std::vector<char> >& q, int kk, int steiner_cnt) {
        kq = kk;
        k = kk + steiner_cnt;
        Q = q;
        int m = (k > 0) ? k : 1;
        mw = (k >= 2) ? (k - 1 + 63) / 64 : 1;
        int maxc = kq * (kq - 1) / 2 + 1;
        /* The k^2 path masks plus the two constraint tables dominate the
         * allocation; refuse components whose tables would not fit. */
        const std::size_t max_mask_words = (std::size_t)1 << 25; /* 32M words = 256 MB */
        std::size_t mask_words = (std::size_t)m * m * mw;
        std::size_t constraint_words = 2 * (std::size_t)maxc * mw;
        if (mask_words + constraint_words > max_mask_words)
            throw std::runtime_error(
                "five_leaf_power: quotient component too large for the "
                "3-Steiner root search (mask tables would exceed 256 MB); "
                "result unknown");
        ord.assign(m, 0);
        placed.assign(m, 0);
        bnd.assign(m, -1);
        bnd_stack.assign(m * m, 0);
        lwd.assign(m * m, 0);
        pmask.assign(mask_words, 0ull);
        elw.assign(m, 0);
        deg.assign(m, 0);
        hi.assign(m, 0);
        ymask_v.assign((std::size_t)maxc * mw, 0ull);
        ybound.assign(maxc, 0);
        ysum.assign(maxc, 0);
        nmask_v.assign((std::size_t)maxc * mw, 0ull);
        nneed.assign(maxc, 0);
        nsum.assign(maxc, 0);
        remain.assign(maxc, 0);
        relevant_.assign(mw, 0ull);
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
        if (step_limit != 0 && ++steps >= step_limit) {
            aborted = true;
            return false;
        }
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
                        const U64* pux = &pmask[((std::size_t)u * k + x) * mw];
                        U64* pvx = &pmask[((std::size_t)v * k + x) * mw];
                        U64* pxv = &pmask[((std::size_t)x * k + v) * mw];
                        for (int w = 0; w < mw; ++w) {
                            U64 pw = pux[w];
                            pvx[w] = pw;
                            pxv[w] = pw;
                        }
                        pvx[e >> 6] |= 1ull << (e & 63);
                        pxv[e >> 6] |= 1ull << (e & 63);
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
                    if (aborted) return false;
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
                const U64* m = &pmask[((std::size_t)i * k + j) * mw];
                U64* ym = &ymask_v[(std::size_t)ny * mw];
                for (int w = 0; w < mw; ++w) ym[w] = m[w];
                ybound[ny] = slack;
                ++ny;
                for (int e = 0; e < ne; ++e)
                    if (mask_test(m, e) && hi[e] > slack) hi[e] = slack;
            }
        }

        nn = 0;
        for (int w = 0; w < mw; ++w) relevant_[w] = 0ull;
        for (int i = 0; i < kq; ++i) {
            for (int j = i + 1; j < kq; ++j) {
                if (Q[i][j]) continue;
                int need = 4 - lwd[i * k + j];
                if (need <= 0) continue;
                const U64* m = &pmask[((std::size_t)i * k + j) * mw];
                int avail = 0;
                for (int e = 0; e < ne; ++e) if (mask_test(m, e)) avail += hi[e];
                if (avail < need) return false;
                U64* nm = &nmask_v[(std::size_t)nn * mw];
                for (int w = 0; w < mw; ++w) nm[w] = m[w];
                nneed[nn] = need;
                ++nn;
                for (int w = 0; w < mw; ++w) relevant_[w] |= m[w];
            }
        }
        if (nn == 0) return true;

        r = 0;
        for (int e = 0; e < ne; ++e)
            if (mask_test(&relevant_[0], e) && hi[e] > 0) rel[r++] = e;

        /* upper-bound constraints that cannot be violated are dropped */
        int kept = 0;
        for (int c = 0; c < ny; ++c) {
            const U64* ym = &ymask_v[(std::size_t)c * mw];
            int cap = 0;
            for (int p = 0; p < r; ++p)
                if (mask_test(ym, rel[p])) cap += hi[rel[p]];
            if (cap > ybound[c]) {
                if (kept != c) {
                    U64* dst = &ymask_v[(std::size_t)kept * mw];
                    for (int w = 0; w < mw; ++w) dst[w] = ym[w];
                }
                ybound[kept] = ybound[c];
                ++kept;
            }
        }
        ny = kept;

        for (int p = 0; p < r; ++p) {
            yes_at[p].clear();
            no_at[p].clear();
            for (int c = 0; c < ny; ++c)
                if (mask_test(&ymask_v[(std::size_t)c * mw], rel[p])) yes_at[p].push_back(c);
            for (int c = 0; c < nn; ++c)
                if (mask_test(&nmask_v[(std::size_t)c * mw], rel[p])) no_at[p].push_back(c);
        }
        for (int c = 0; c < ny; ++c) ysum[c] = 0;
        nsat = 0;
        for (int c = 0; c < nn; ++c) {
            nsum[c] = 0;
            int av = 0;
            const U64* nm = &nmask_v[(std::size_t)c * mw];
            for (int p = 0; p < r; ++p)
                if (mask_test(nm, rel[p])) av += hi[rel[p]];
            if (av < nneed[c]) return false;
            remain[c] = av;
        }
        return wdfs(0);
    }

    /** @brief Backtracking over the relevant edges' extra weight */
    bool wdfs(int d) {
        if (step_limit != 0 && ++steps >= step_limit) {
            aborted = true;
            return false;
        }
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
            if (aborted) break;
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
 *
 * budget bounds the total number of search steps across all components and
 * Steiner counts (0 = unlimited); exhausting it throws std::runtime_error
 * instead of returning a wrong answer.
 */
inline bool quotient_realizable(const std::vector<std::vector<char> >& Q, int k,
    unsigned long long budget = five_leaf_power_default_budget) {
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
    search.steps = 0;
    search.step_limit = budget;
    search.aborted = false;
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
            search.init(sub, kc, s);
            if (search.run()) found = true;
            if (search.aborted)
                throw std::runtime_error(
                    "five_leaf_power: search budget exceeded (the instance "
                    "is too hard for the exponential 3-Steiner root "
                    "search); result unknown");
        }
        if (!found) return false;
    }
    return true;
}

/**
 * @brief Implementation of 5-leaf power recognition
 */
inline FiveLeafPowerResult check_five_leaf_power_impl(const Graph& g,
    unsigned long long search_budget) {
    FiveLeafPowerResult res;
    res.is_five_leaf_power = false;

    if (g.n == 0) { res.is_five_leaf_power = true; return res; }

    // 1. Strongly chordal check (5-leaf power is a subclass of strongly chordal)
    StronglyChordalResult scr = check_strongly_chordal(g);
    if (!scr.is_strongly_chordal) return res;

    // 2./3. Critical cliques and the quotient graph Q (0-indexed, reflexive:
    // the 3-Steiner root search below reads Q[i][i] as "same clique")
    TwinQuotientResult cq = critical_clique_quotient(g);
    int k = cq.quotient.n;

    std::vector<std::vector<char> > Q(k, std::vector<char>(k, 0));
    for (int i = 0; i < k; ++i) Q[i][i] = 1;
    for (int ci = 1; ci <= k; ++ci) {
        for (size_t ei = 0; ei < cq.quotient.adj[ci].size(); ++ei) {
            Q[ci - 1][cq.quotient.adj[ci][ei] - 1] = 1;
        }
    }

    // Consistency check: if Q[ci][cj]=1 then all pairs are adjacent
    for (int ci = 0; ci < k; ++ci) {
        int rep = cq.members[ci + 1][0];
        int my_size = (int)cq.members[ci + 1].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);
        int expected = 0;
        for (int cj = 0; cj < k; ++cj) {
            if (cj != ci && Q[ci][cj])
                expected += (int)cq.members[cj + 1].size();
        }
        if (external_edges != expected) return res;
    }

    // k=1: complete graph -> always a 5-leaf power
    if (k == 1) { res.is_five_leaf_power = true; return res; }

    // 4. Search for a 3-Steiner root of Q
    res.is_five_leaf_power = quotient_realizable(Q, k, search_budget);
    return res;
}

} // namespace detail_five_leaf_power

/**
 * @brief Determines whether the graph is a 5-leaf power
 * @param g Input graph
 * @param search_budget Step budget for the exponential 3-Steiner root
 *        search (0 = unlimited). Default: five_leaf_power_default_budget.
 * @return FiveLeafPowerResult
 * @throws std::runtime_error if the budget (or the per-component memory
 *         guard) is exhausted before the search decides -- the result is
 *         then unknown, never silently reported as NO
 */
inline FiveLeafPowerResult check_five_leaf_power(const Graph& g,
    unsigned long long search_budget = five_leaf_power_default_budget) {
    return detail_five_leaf_power::check_five_leaf_power_impl(g, search_budget);
}

} // namespace graph_recognition

#endif
