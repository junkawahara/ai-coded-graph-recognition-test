#ifndef GRAPH_RECOGNITION_PARTIAL_KTREE_H
#define GRAPH_RECOGNITION_PARTIAL_KTREE_H

/**
 * @file partial_ktree.h
 * @brief Partial k-tree (treewidth <= k) recognition
 *
 * A partial k-tree is a subgraph of a k-tree, equivalently a graph of
 * treewidth at most k. k = 1 gives the forests, k = 2 the series-parallel
 * (K4-minor-free) graphs. Unlike the other recognizers, k is an input:
 * every graph is a partial (n-1)-tree, so nothing can be inferred.
 *
 * Deciding treewidth <= k is NP-complete when k is part of the input
 * (Arnborg-Corneil-Proskurowski 1987), so the recognizer is a memoized
 * exact search over elimination orders: a graph has treewidth <= k iff
 * its vertices can be eliminated one by one so that each eliminated
 * vertex has at most k neighbors in the current fill graph (the graph
 * where u, w are adjacent iff they are joined by a path whose interior
 * is already eliminated). The search branches on the vertex eliminated
 * next, memoizing on the set of eliminated vertices (the fill graph is
 * determined by that set alone), forcing the elimination of simplicial
 * vertices of fill degree <= k (always safe: tw(G) = max(deg(v), tw(G-v))
 * for simplicial v), pruning branches containing a simplicial vertex of
 * fill degree > k (its closed fill neighborhood is a clique on more than
 * k+1 vertices), and stopping as soon as at most k+1 vertices remain.
 * A degeneracy lower bound (degeneracy <= treewidth) rejects many NO
 * instances before the search starts.
 *
 * References:
 *   Arnborg, Corneil, Proskurowski, "Complexity of finding embeddings in
 *   a k-tree," SIAM J. Alg. Disc. Meth. 8, 1987 (NP-completeness, and the
 *   elimination-order characterization);
 *   Bodlaender, "A partial k-arboretum of graphs with bounded treewidth,"
 *   TCS 209, 1998 (survey);
 *   Bodlaender, Fomin, Koster, Kratsch, Thilikos, "On exact algorithms
 *   for treewidth," ACM TALG 9, 2012 (the O*(2^n) elimination-order DP
 *   this search memoizes)
 */

#include "util/graph.h"

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for partial k-tree recognition
 */
enum class PartialKTreeAlgorithm {
    ELIMINATION_SEARCH /**< Memoized exact search over elimination orders */
};

/**
 * @brief Result of partial k-tree recognition
 */
struct PartialKTreeResult {
    bool is_partial_ktree = false; /**< true if the graph has treewidth <= k */
    /**
     * @brief Width of the certifying elimination order (-1 on NO)
     *
     * The maximum fill degree along `elimination_order`, so
     * treewidth <= width <= k. It is an upper bound witness, not
     * necessarily the exact treewidth. The empty graph gets width -1
     * (its treewidth). Valid only when is_partial_ktree == true.
     */
    int width = -1;
    /**
     * @brief elimination_order[i] = the i-th vertex eliminated, for i in [1, n] (size n+1)
     *
     * A perfect elimination order of a chordal completion of width at
     * most k: eliminating the vertices in this order, every vertex has at
     * most k not-yet-eliminated neighbors in the fill graph. Read
     * backwards it is a k-tree construction order of a supergraph. Valid
     * only when is_partial_ktree == true.
     */
    std::vector<int> elimination_order;
};

namespace detail {

/** @brief Population count of a bitmask word */
inline int partial_ktree_popcount(unsigned long long x) {
    int c = 0;
    while (x != 0) {
        x &= x - 1;
        ++c;
    }
    return c;
}

/** @brief Tests bit v of a multi-word mask */
inline bool partial_ktree_test_bit(const std::vector<unsigned long long>& mask,
                                   int v) {
    return ((mask[v >> 6] >> (v & 63)) & 1ULL) != 0;
}

/** @brief Sets bit v of a multi-word mask */
inline void partial_ktree_set_bit(std::vector<unsigned long long>& mask,
                                  int v) {
    mask[v >> 6] |= 1ULL << (v & 63);
}

/**
 * @brief Fill neighborhood of v given the eliminated set
 * @param adj Adjacency bitmasks (n rows of words words each), 0-indexed
 * @param words Number of 64-bit words per mask
 * @param elim Mask of eliminated vertices
 * @param v A non-eliminated vertex
 * @param fnbr Output: the non-eliminated vertices joined to v by a path
 *             whose interior lies in elim (words words, overwritten)
 *
 * These are exactly v's neighbors after the eliminations, i.e. its
 * neighbors in the fill graph determined by elim.
 */
inline void partial_ktree_fill_neighborhood(
    const std::vector<std::vector<unsigned long long> >& adj, int words,
    const std::vector<unsigned long long>& elim, int v,
    std::vector<unsigned long long>& fnbr) {
    // Accumulate N(R) for the set R grown from v through eliminated
    // vertices; processed marks the eliminated vertices already expanded.
    std::vector<unsigned long long> acc = adj[v];
    std::vector<unsigned long long> processed(words, 0);
    bool changed = true;
    while (changed) {
        changed = false;
        for (int w = 0; w < words; ++w) {
            unsigned long long todo = acc[w] & elim[w] & ~processed[w];
            while (todo != 0) {
                const int u = w * 64 + __builtin_ctzll(todo);
                todo &= todo - 1;
                processed[w] |= 1ULL << (u & 63);
                for (int x = 0; x < words; ++x) acc[x] |= adj[u][x];
                changed = true;
            }
        }
    }
    fnbr.assign(words, 0);
    for (int w = 0; w < words; ++w) fnbr[w] = acc[w] & ~elim[w];
    fnbr[v >> 6] &= ~(1ULL << (v & 63));
}

/** @brief Shared state of the elimination-order search */
struct PartialKTreeSearchState {
    int n;     /**< Number of vertices */
    int words; /**< 64-bit words per vertex mask */
    int k;     /**< Target width */
    std::vector<std::vector<unsigned long long> > adj; /**< Adjacency bitmasks */
    std::set<std::vector<unsigned long long> > visited; /**< Memoized eliminated sets */
    std::vector<int> order; /**< Elimination order built so far (0-indexed) */
};

/**
 * @brief Memoized DFS over elimination orders
 * @param s Shared search state
 * @param elim Mask of eliminated vertices (taken by value; extended locally)
 * @param remaining Number of non-eliminated vertices
 * @return true if the remaining fill graph has an elimination order of width <= s.k
 *
 * On success s.order holds a complete certifying order; on failure it is
 * restored to its length at entry.
 */
inline bool partial_ktree_dfs(PartialKTreeSearchState& s,
                              std::vector<unsigned long long> elim,
                              int remaining) {
    const std::size_t order_base = s.order.size();
    std::vector<std::vector<unsigned long long> > fnbr(s.n);
    std::vector<int> deg(s.n, 0);

    for (;;) {
        if (s.k >= remaining - 1) {
            // Any completion works: at most k+1 vertices remain, so every
            // later fill degree is at most k.
            for (int v = 0; v < s.n; ++v) {
                if (!partial_ktree_test_bit(elim, v)) s.order.push_back(v);
            }
            return true;
        }

        for (int v = 0; v < s.n; ++v) {
            if (partial_ktree_test_bit(elim, v)) continue;
            partial_ktree_fill_neighborhood(s.adj, s.words, elim, v, fnbr[v]);
            deg[v] = 0;
            for (int w = 0; w < s.words; ++w) {
                deg[v] += partial_ktree_popcount(fnbr[v][w]);
            }
        }

        // Simplicial vertices: fill degree <= k forces the elimination
        // (tw = max(deg, tw of the rest)); fill degree > k exhibits a
        // clique on more than k+1 vertices, killing the whole branch.
        int forced = -1;
        for (int v = 0; v < s.n && forced < 0; ++v) {
            if (partial_ktree_test_bit(elim, v)) continue;
            bool simplicial = true;
            for (int w = 0; w < s.words && simplicial; ++w) {
                unsigned long long bits = fnbr[v][w];
                while (bits != 0) {
                    const int u = w * 64 + __builtin_ctzll(bits);
                    bits &= bits - 1;
                    for (int x = 0; x < s.words; ++x) {
                        unsigned long long missing = fnbr[v][x] & ~fnbr[u][x];
                        if (x == (u >> 6)) missing &= ~(1ULL << (u & 63));
                        if (missing != 0) {
                            simplicial = false;
                            break;
                        }
                    }
                    if (!simplicial) break;
                }
            }
            if (simplicial) {
                if (deg[v] > s.k) {
                    s.order.resize(order_base);
                    return false;
                }
                forced = v;
            }
        }
        if (forced >= 0) {
            s.order.push_back(forced);
            partial_ktree_set_bit(elim, forced);
            --remaining;
            continue;
        }

        if (!s.visited.insert(elim).second) {
            s.order.resize(order_base);
            return false;
        }

        // Branch on the next eliminated vertex, cheapest fill degree first.
        std::vector<std::pair<int, int> > candidates;
        for (int v = 0; v < s.n; ++v) {
            if (partial_ktree_test_bit(elim, v)) continue;
            if (deg[v] <= s.k) candidates.push_back(std::make_pair(deg[v], v));
        }
        std::sort(candidates.begin(), candidates.end());
        for (std::size_t i = 0; i < candidates.size(); ++i) {
            const int v = candidates[i].second;
            s.order.push_back(v);
            std::vector<unsigned long long> child = elim;
            partial_ktree_set_bit(child, v);
            if (partial_ktree_dfs(s, child, remaining - 1)) return true;
            s.order.pop_back();
        }
        s.order.resize(order_base);
        return false;
    }
}

/** @brief Degeneracy (max over the peeling of the minimum degree), a treewidth lower bound */
inline int partial_ktree_degeneracy(const Graph& g) {
    const int n = g.n;
    std::vector<int> deg(n + 1, 0);
    std::vector<bool> removed(n + 1, false);
    for (int v = 1; v <= n; ++v) deg[v] = (int)g.adj[v].size();
    int degeneracy = 0;
    for (int step = 0; step < n; ++step) {
        int best = -1;
        for (int v = 1; v <= n; ++v) {
            if (!removed[v] && (best < 0 || deg[v] < deg[best])) best = v;
        }
        if (deg[best] > degeneracy) degeneracy = deg[best];
        removed[best] = true;
        for (std::size_t j = 0; j < g.adj[best].size(); ++j) {
            const int u = g.adj[best][j];
            if (!removed[u]) --deg[u];
        }
    }
    return degeneracy;
}

}  // namespace detail

/**
 * @brief Determines whether the graph has treewidth at most k
 * @param g Input graph
 * @param k Target treewidth bound (the class parameter)
 * @param algo Algorithm to use (default: ELIMINATION_SEARCH)
 * @return PartialKTreeResult
 *
 * The empty graph is a partial k-tree for every k; a nonempty graph with
 * k < 0 is not.
 *
 * @note Deciding treewidth <= k is NP-complete, so the worst case is
 *       exponential: the memoization bounds the search by O*(2^n) time
 *       and space. The degeneracy lower bound, the forced simplicial
 *       eliminations and the <= k+1 remainder cutoff decide most small
 *       or sparse instances quickly, but expect exponential behavior
 *       when the answer is NO and the degeneracy is <= k.
 */
inline PartialKTreeResult check_partial_ktree(const Graph& g, int k,
    PartialKTreeAlgorithm algo = PartialKTreeAlgorithm::ELIMINATION_SEARCH) {
    (void)algo;
    PartialKTreeResult res;
    const int n = g.n;
    if (n == 0) {
        res.is_partial_ktree = true;
        res.width = -1;
        res.elimination_order.assign(1, 0);
        return res;
    }
    if (k < 0) return res;
    if (detail::partial_ktree_degeneracy(g) > k) return res;

    const int words = (n + 63) / 64;
    detail::PartialKTreeSearchState s;
    s.n = n;
    s.words = words;
    s.k = k;
    s.adj.assign(n, std::vector<unsigned long long>(words, 0));
    for (int v = 1; v <= n; ++v) {
        for (std::size_t j = 0; j < g.adj[v].size(); ++j) {
            const int u = g.adj[v][j];
            s.adj[v - 1][(u - 1) >> 6] |= 1ULL << ((u - 1) & 63);
        }
    }

    std::vector<unsigned long long> elim(words, 0);
    if (!detail::partial_ktree_dfs(s, elim, n)) return res;

    res.is_partial_ktree = true;
    res.elimination_order.assign(n + 1, 0);
    for (int i = 0; i < n; ++i) res.elimination_order[i + 1] = s.order[i] + 1;

    // Replay the order once to report the width it certifies.
    res.width = 0;
    std::vector<unsigned long long> replay_elim(words, 0);
    std::vector<unsigned long long> fnbr;
    for (int i = 0; i < n; ++i) {
        const int v = s.order[i];
        detail::partial_ktree_fill_neighborhood(s.adj, words, replay_elim, v,
                                                fnbr);
        int d = 0;
        for (int w = 0; w < words; ++w) {
            d += detail::partial_ktree_popcount(fnbr[w]);
        }
        if (d > res.width) res.width = d;
        detail::partial_ktree_set_bit(replay_elim, v);
    }
    return res;
}

}  // namespace graph_recognition

#endif
