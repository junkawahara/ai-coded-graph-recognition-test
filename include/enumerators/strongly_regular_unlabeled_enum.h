#ifndef GRAPH_RECOGNITION_STRONGLY_REGULAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_STRONGLY_REGULAR_UNLABELED_ENUM_H

/**
 * @file strongly_regular_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic strongly regular graphs
 *
 * Enumerates one representative per isomorphism class of strongly regular
 * graphs srg(n, k, lambda, mu) on n vertices (k-regular, adjacent pairs
 * have lambda common neighbors, non-adjacent pairs mu; complete and empty
 * graphs excluded, i.e. 0 < k < n-1), following the McKay--Spence recipe:
 * one exhaustive search per feasible parameter tuple, with the tuples
 * prefiltered by the counting identity k(k-lambda-1) = mu(n-k-1) and the
 * eigenvalue-integrality conditions (`detail::srg_enumerate_params` of the
 * labeled enumerator), and isomorph rejection inside each search.
 *
 * Each per-parameter search is a McKay canonical construction path on the
 * shared machinery of `util/canonical_augmentation.h` (the scheme of
 * `kregular_unlabeled_enum.h`, which contributes the degree constraints).
 * The class is not hereditary, but every induced subgraph of an
 * srg(n, k, lambda, mu) satisfies necessary conditions that drive the
 * pruning: with r vertices still to come and def(u) = k - deg(u),
 *   - def(u) <= r for every vertex, sum def <= k*r, and k*r - sum def
 *     even (the k-regular completability conditions), and
 *   - for every pair u, v with target t = lambda (adjacent) or mu
 *     (non-adjacent): cn(u, v) <= t and
 *     cn(u, v) + min(def(u), def(v), r) >= t, since common neighbors
 *     never disappear and each future common neighbor consumes one unit
 *     of both deficits.
 * At the last level (r = 0) these force all degrees to k and every
 * cn(u, v) to its target exactly, so each graph reaching level n is an
 * srg(n, k, lambda, mu) by construction. Completeness holds because the
 * conditions are necessary: every graph on the canonical-deletion chain
 * of an srg is one of its induced subgraphs and therefore passes them.
 *
 * Isomorph rejection is the canonical-parent test at every level (one
 * canonicalization per candidate child; the child survives when the new
 * vertex lies in the canonical-deletion orbit, with siblings deduplicated
 * by canonical form). No rejection is needed *across* parameter tuples: a
 * strongly regular graph determines (k, lambda, mu) uniquely (k is the
 * degree; lambda and mu are read off any adjacent / non-adjacent pair,
 * and both pair kinds exist because 0 < k < n-1), so distinct searches
 * emit disjoint sets of classes.
 *
 * Number of non-isomorphic strongly regular graphs on n = 1, 2, ... :
 * 0, 0, 0, 2, 1, 4, 0, 4, 3, 6, 0, 8, 1, 4, 6, ... (parameter-dependent;
 * the survey's OEIS pointer A088741 tracks individual tuples, not this
 * total). The imprimitive classes are the disjoint unions t x K_m and
 * their complements K_{t x m} (t, m >= 2); the primitive ones start with
 * C5, Paley(9), Petersen and its complement, Paley(13).
 *
 * References:
 *   McKay, Spence, "Classification of regular two-graphs on 36 and 38
 *   vertices," Australas. J. Combin. 24, 2001 (per-parameter exhaustive
 *   generation with isomorph rejection); McKay, "Isomorph-free exhaustive
 *   generation," J. Algorithms 26, 1998 (canonical construction path);
 *   Brouwer's strongly-regular-graph parameter tables
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "enumerators/strongly_regular_labeled_enum.h"
#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic strongly regular enumeration
 */
enum class StronglyRegularUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< Per-parameter McKay canonical construction path */
};

/**
 * @brief An enumerated strongly regular graph
 */
struct StronglyRegularUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic strongly regular enumeration
 */
struct StronglyRegularUnlabeledEnumerationResult {
    std::vector<StronglyRegularUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Population count of a bitmask (C++11, no compiler builtin) */
inline int strongly_regular_unlabeled_popcount(unsigned long long b) {
    int count = 0;
    for (; b != 0; b &= b - 1) ++count;
    return count;
}

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline StronglyRegularUnlabeledEnumeratedGraph
strongly_regular_unlabeled_build_graph(
    int c, const std::vector<unsigned long long>& adj) {
    StronglyRegularUnlabeledEnumeratedGraph g;
    g.n = c;
    g.edges = bitmask_graph_edges(c, adj);
    return g;
}

inline void strongly_regular_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k, int lam,
    int mu, std::vector<StronglyRegularUnlabeledEnumeratedGraph>& results);

/**
 * @brief Tests the necessary completability conditions after adding a vertex
 * @param c Number of vertices before the addition
 * @param adj Adjacency bitmasks of the c-vertex graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., c-1
 * @param n Target number of vertices
 * @param k Target degree
 * @param lam Target common neighbor count of adjacent pairs
 * @param mu Target common neighbor count of non-adjacent pairs
 * @return true if the (c+1)-vertex graph may still extend to an srg(n, k, lam, mu)
 *
 * With r = n - c - 1 future vertices: the k-regular deficit conditions
 * (def(u) <= r, sum def <= k*r, k*r - sum def even) plus, for every
 * vertex pair with target t = lam / mu, the common-neighbor window
 * cn <= t and cn + min(def(u), def(v), r) >= t. All hold for every
 * induced subgraph of an srg(n, k, lam, mu), so pruning on them never
 * cuts a canonical-deletion chain.
 */
inline bool strongly_regular_unlabeled_enum_feasible(
    int c, const std::vector<unsigned long long>& adj, unsigned long long s,
    int n, int k, int lam, int mu) {
    const int r = n - c - 1;
    std::vector<int> deg(c + 1, 0);
    int total_def = 0;
    for (int u = 0; u < c; ++u) {
        int d = strongly_regular_unlabeled_popcount(adj[u]);
        if ((s >> u) & 1ULL) ++d;
        deg[u] = d;
        if (k - d > r) return false;
        total_def += k - d;
    }
    deg[c] = strongly_regular_unlabeled_popcount(s);
    if (k - deg[c] > r) return false;
    total_def += k - deg[c];
    if (total_def > k * r) return false;
    if ((k * r - total_def) % 2 != 0) return false;

    for (int u = 0; u < c; ++u) {
        {
            // Pair (u, new vertex): the new vertex is adjacent to u iff
            // u is in s, and their common neighbors are N(u) ∩ s.
            const int cn = strongly_regular_unlabeled_popcount(adj[u] & s);
            const int target = ((s >> u) & 1ULL) ? lam : mu;
            if (cn > target) return false;
            int future = k - deg[u] < k - deg[c] ? k - deg[u] : k - deg[c];
            if (future > r) future = r;
            if (cn + future < target) return false;
        }
        for (int v = u + 1; v < c; ++v) {
            // Pair of old vertices: the new vertex is a common neighbor
            // iff both are in s.
            int cn = strongly_regular_unlabeled_popcount(adj[u] & adj[v]);
            if (((s >> u) & (s >> v)) & 1ULL) ++cn;
            const int target = ((adj[u] >> v) & 1ULL) ? lam : mu;
            if (cn > target) return false;
            int future = k - deg[u] < k - deg[v] ? k - deg[u] : k - deg[v];
            if (future > r) future = r;
            if (cn + future < target) return false;
        }
    }
    return true;
}

/**
 * @brief Extends the graph by a new vertex with neighborhood s and recurses
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., c-1
 * @param n Target number of vertices
 * @param k Target degree
 * @param lam Target common neighbor count of adjacent pairs
 * @param mu Target common neighbor count of non-adjacent pairs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * The child survives the canonical-parent test when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument.
 */
inline void strongly_regular_unlabeled_enum_extend(
    int c, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, int k, int lam, int mu,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<StronglyRegularUnlabeledEnumeratedGraph>& results) {
    adj.push_back(s);
    for (int u = 0; u < c; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << c;
    }

    CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(c + 1, adj);
    if (((canon.last_orbit >> c) & 1ULL) &&
        child_forms.insert(canon.form).second) {
        strongly_regular_unlabeled_enum_dfs(c + 1, adj, n, k, lam, mu,
                                            results);
    }

    for (int u = 0; u < c; ++u) {
        if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << c);
    }
    adj.pop_back();
}

/**
 * @brief Enumerates the candidate neighborhoods of the new vertex
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param available Vertices of degree < k, eligible as neighbors
 * @param start Next index into available to consider
 * @param s Neighborhood accumulated so far as a bitmask
 * @param s_size Number of vertices in s
 * @param n Target number of vertices
 * @param k Target degree
 * @param lam Target common neighbor count of adjacent pairs
 * @param mu Target common neighbor count of non-adjacent pairs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * Standard combination DFS over the at-most-k-subsets of available; each
 * subset that passes the completability conditions becomes a candidate
 * child.
 */
inline void strongly_regular_unlabeled_enum_choose(
    int c, std::vector<unsigned long long>& adj,
    const std::vector<int>& available, std::size_t start,
    unsigned long long s, int s_size, int n, int k, int lam, int mu,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<StronglyRegularUnlabeledEnumeratedGraph>& results) {
    if (strongly_regular_unlabeled_enum_feasible(c, adj, s, n, k, lam, mu)) {
        strongly_regular_unlabeled_enum_extend(c, adj, s, n, k, lam, mu,
                                               child_forms, results);
    }
    if (s_size == k) return;
    for (std::size_t i = start; i < available.size(); ++i) {
        strongly_regular_unlabeled_enum_choose(
            c, adj, available, i + 1, s | (1ULL << available[i]), s_size + 1,
            n, k, lam, mu, child_forms, results);
    }
}

/**
 * @brief Canonical augmentation DFS from a c-vertex canonical representative
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param k Target degree
 * @param lam Target common neighbor count of adjacent pairs
 * @param mu Target common neighbor count of non-adjacent pairs
 * @param results Storage for results
 *
 * A graph reaching level n is an srg(n, k, lam, mu) by construction (at
 * r = 0 the completability conditions force all degrees and all pairwise
 * common neighbor counts to their targets exactly).
 */
inline void strongly_regular_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k, int lam,
    int mu, std::vector<StronglyRegularUnlabeledEnumeratedGraph>& results) {
    if (c == n) {
        results.push_back(strongly_regular_unlabeled_build_graph(c, adj));
        return;
    }
    std::vector<int> available;
    for (int u = 0; u < c; ++u) {
        if (strongly_regular_unlabeled_popcount(adj[u]) < k) {
            available.push_back(u);
        }
    }
    std::set<std::vector<unsigned long long> > child_forms;
    strongly_regular_unlabeled_enum_choose(c, adj, available, 0, 0ULL, 0, n,
                                           k, lam, mu, child_forms, results);
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic strongly regular graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return StronglyRegularUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class, across all feasible
 * parameter tuples (k, lambda, mu) for the given n: 2 classes at n = 4
 * (2K2 and C4), 1 at n = 5 (C5), 4 at n = 6, none at n = 7 or 11, 6 at
 * n = 10 (Petersen, its complement, and four imprimitive classes).
 * n < 4 yields nothing, matching
 * `enumerate_strongly_regular_labeled_graphs` (complete and empty graphs
 * are excluded by definition, so no strongly regular graph has fewer
 * than 4 vertices). There is no `connected_only` flag, also matching the
 * labeled enumerator (the disconnected classes are exactly the disjoint
 * unions t x K_m).
 *
 * @note The intermediate levels range over the max-degree-<= k graphs
 *       passing the common-neighbor windows, with one exact
 *       branch-and-bound canonicalization per candidate child. The
 *       windows prune far harder than the plain degree conditions of
 *       `enumerate_kregular_unlabeled_graphs`, so the enumeration is
 *       practical to about n = 15 (n = 12 ~1 s, n = 14 ~53 s, n = 15
 *       ~24 s — the cost is dominated by the infeasible parameter
 *       tuples that survive the arithmetic prefilter, not by the number
 *       of graphs emitted).
 */
inline StronglyRegularUnlabeledEnumerationResult
enumerate_strongly_regular_unlabeled_graphs(
    int n,
    StronglyRegularUnlabeledEnumAlgorithm algo =
        StronglyRegularUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    StronglyRegularUnlabeledEnumerationResult result;
    if (n < 4 || n >= 64) return result;
    std::vector<detail::SRGParams> params = detail::srg_enumerate_params(n);
    for (std::size_t pi = 0; pi < params.size(); ++pi) {
        std::vector<unsigned long long> adj(1, 0);
        detail::strongly_regular_unlabeled_enum_dfs(
            1, adj, n, params[pi].k, params[pi].lambda, params[pi].mu,
            result.graphs);
    }
    return result;
}

}  // namespace graph_recognition

#endif
