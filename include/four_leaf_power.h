#ifndef GRAPH_RECOGNITION_FOUR_LEAF_POWER_H
#define GRAPH_RECOGNITION_FOUR_LEAF_POWER_H

/**
 * @file four_leaf_power.h
 * @brief 4-leaf power graph recognition
 *
 * A graph G is a 4-leaf power if there exists a tree T whose leaves are
 * the vertices of G, where leaves u, v are adjacent in G iff d(u,v) <= 4 in T.
 *
 * Algorithm:
 *   1. Check if strongly chordal (necessary condition)
 *   2. Compute critical cliques (maximal sets of vertices with identical
 *      closed neighborhoods) and the quotient graph Q. G is a 4-leaf power
 *      iff Q has a "Steiner 2-root": a tree T' containing the vertices of Q
 *      such that two Q-vertices are adjacent iff their distance in T' is
 *      at most 2 (attach each critical clique's members as leaves at its
 *      Q-node; leaf distance = node distance + 2).
 *   3. Steiner 2-root search. In any realization every maximal clique C of
 *      Q equals the set of vertices within distance 1 of some tree node
 *      (its center; unit balls of a tree metric have the Helly property),
 *      and the star {center} x C is a subtree. Conversely, if each maximal
 *      clique is assigned a distinct center (one of its vertices or a new
 *      Steiner node) such that the union F of the stars is acyclic and
 *      every non-adjacent pair of Q-vertices has forest distance >= 3,
 *      then F plus length-3 connector chains between components is a
 *      Steiner 2-root. So Q is realizable iff such a center assignment
 *      exists; try all (|C|+1 choices per maximal clique, backtracking).
 *      Earlier versions enumerated only trees whose nodes are the critical
 *      cliques themselves and missed realizations that need Steiner branch
 *      nodes (false NO, e.g. three cliques sharing pairwise intersections
 *      arranged around a Steiner hub).
 *
 * References:
 *   - Brandstädt, Le, Sritharan (2008). Structure and linear-time
 *     recognition of 4-leaf powers. ACM Trans. Algorithms 5(1).
 *     (4-leaf powers = squares of trees with cliques substituted.)
 */

#include "graph.h"
#include "strongly_chordal.h"
#include "chordal.h"
#include "clique.h"
#include "twins.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {

struct FourLeafPowerResult {
    bool is_four_leaf_power = false;
};

namespace detail_four_leaf_power {

/**
 * @brief Backtracking over center assignments for the Steiner 2-root search
 *
 * Nodes 1..k are the quotient vertices; node k+1+i is the potential Steiner
 * center of maximal clique i. Maintains the star forest F incrementally.
 */
struct SteinerTwoRootSearch {
    int k;                                    /**< number of quotient vertices */
    int total;                                /**< k + number of cliques */
    const std::vector<std::vector<int>>* cliques; /**< maximal cliques (1-based members) */
    const std::vector<std::vector<char>>* Q;  /**< quotient adjacency (0-based) */
    std::vector<std::vector<char>> fadj;      /**< forest adjacency (1-based nodes) */
    std::vector<int> parent;                  /**< DSU parent */

    int dsu_find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }

    /** @brief Adds star edges for clique ci with the given center; returns
     *         the added edges through out_added, or false on a cycle. */
    bool add_star(int ci, int center, std::vector<std::pair<int,int>>* out_added) {
        const std::vector<int>& c = (*cliques)[ci];
        for (size_t t = 0; t < c.size(); ++t) {
            int v = c[t];
            if (v == center) continue;
            if (fadj[center][v]) continue; /* duplicate edge: no cycle */
            int ra = dsu_find(center), rb = dsu_find(v);
            if (ra == rb) {
                /* cycle: undo what this call added */
                for (size_t j = 0; j < out_added->size(); ++j) {
                    int a = (*out_added)[j].first, b = (*out_added)[j].second;
                    fadj[a][b] = fadj[b][a] = 0;
                }
                return false;
            }
            parent[ra] = rb;
            fadj[center][v] = fadj[v][center] = 1;
            out_added->push_back(std::make_pair(center, v));
        }
        return true;
    }

    /** @brief Final check: non-adjacent quotient pairs at forest distance >= 3 */
    bool distances_ok() {
        std::vector<int> dist(total + 1);
        std::vector<int> queue;
        for (int s = 1; s <= k; ++s) {
            std::fill(dist.begin(), dist.end(), -1);
            queue.clear();
            dist[s] = 0;
            queue.push_back(s);
            for (size_t qi = 0; qi < queue.size(); ++qi) {
                int v = queue[qi];
                if (dist[v] >= 2) break; /* only distances <= 2 matter */
                for (int w = 1; w <= total; ++w) {
                    if (fadj[v][w] && dist[w] == -1) {
                        dist[w] = dist[v] + 1;
                        queue.push_back(w);
                    }
                }
            }
            for (int t = 1; t <= k; ++t) {
                if (t == s) continue;
                bool close = (dist[t] >= 0 && dist[t] <= 2);
                bool adj = (*Q)[s - 1][t - 1] != 0;
                if (close != adj) return false;
            }
        }
        return true;
    }

    bool search(int ci) {
        int m = (int)cliques->size();
        if (ci == m) return distances_ok();

        const std::vector<int>& c = (*cliques)[ci];
        /* Candidate centers: each member, then a fresh Steiner node */
        for (size_t t = 0; t <= c.size(); ++t) {
            int center = (t < c.size()) ? c[t] : (k + 1 + ci);
            std::vector<std::pair<int,int>> added;
            std::vector<int> saved_parent(parent);
            if (add_star(ci, center, &added)) {
                if (search(ci + 1)) return true;
                for (size_t j = 0; j < added.size(); ++j) {
                    int a = added[j].first, b = added[j].second;
                    fadj[a][b] = fadj[b][a] = 0;
                }
            }
            parent.swap(saved_parent);
        }
        return false;
    }
};

/**
 * @brief Implementation of 4-leaf power recognition
 */
inline FourLeafPowerResult check_four_leaf_power_impl(const Graph& g) {
    FourLeafPowerResult res;
    res.is_four_leaf_power = false;

    if (g.n == 0) { res.is_four_leaf_power = true; return res; }

    // 1. Strongly chordal check (4-leaf power is a subclass of strongly chordal)
    StronglyChordalResult scr = check_strongly_chordal(g);
    if (!scr.is_strongly_chordal) return res;

    // 2./3. Critical cliques and the quotient graph Q (0-indexed for the
    // Steiner root search below, which indexes cliques from 0)
    TwinQuotientResult cq = critical_clique_quotient(g);
    int k = cq.quotient.n;

    std::vector<std::vector<char>> Q(k, std::vector<char>(k, 0));
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

    // k=1: complete graph -> always a 4-leaf power
    if (k == 1) { res.is_four_leaf_power = true; return res; }

    // 4. Steiner 2-root search on Q
    std::vector<std::pair<int, int>> q_edges;
    for (int i = 0; i < k; ++i)
        for (int j = i + 1; j < k; ++j)
            if (Q[i][j]) q_edges.push_back(std::make_pair(i + 1, j + 1));
    Graph qg(k, q_edges);

    ChordalResult qch = check_chordal(qg);
    if (!qch.is_chordal) return res; /* implied by strong chordality of G */
    MaximalCliques mc = enumerate_maximal_cliques(qg, qch);

    SteinerTwoRootSearch search;
    search.k = k;
    search.total = k + (int)mc.cliques.size();
    search.cliques = &mc.cliques;
    search.Q = &Q;
    search.fadj.assign(search.total + 1,
                       std::vector<char>(search.total + 1, 0));
    search.parent.resize(search.total + 1);
    for (int i = 0; i <= search.total; ++i) search.parent[i] = i;

    res.is_four_leaf_power = search.search(0);
    return res;
}

} // namespace detail_four_leaf_power

/**
 * @brief Determines whether the graph is a 4-leaf power
 * @param g Input graph
 * @return FourLeafPowerResult
 */
inline FourLeafPowerResult check_four_leaf_power(const Graph& g) {
    return detail_four_leaf_power::check_four_leaf_power_impl(g);
}

} // namespace graph_recognition

#endif
