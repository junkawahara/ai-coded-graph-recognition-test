#ifndef GRAPH_RECOGNITION_CAGE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CAGE_UNLABELED_ENUM_H

/**
 * @file cage_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic (k,g)-graphs (k-regular, girth >= g)
 *
 * Enumerates one representative per isomorphism class of the k-regular
 * graphs with girth at least g on n vertices — GENREG's girth-constrained
 * regular graph generation (Meringer 1999) lifted onto the shared
 * canonical-augmentation machinery, exactly as `kregular_unlabeled_enum.h`
 * (the g <= 3 special case) with the girth constraint folded into the
 * search. The girth parameter is a *lower bound*, matching GENREG: the
 * (k,g)-graphs with girth exactly g are the difference between the g and
 * the g+1 enumerations, and the (k,g)-cages are the members of the first
 * nonempty level, i.e. the output at the smallest n for which the
 * enumeration is nonempty (by the strict monotonicity of the cage order
 * n(k,g) that smallest level has girth exactly g).
 *
 * Girth >= g is hereditary under vertex deletion, so the intermediate
 * levels range over the hereditary class of graphs with maximum degree
 * <= k and girth >= g. Every cycle created by joining the new vertex to
 * a neighborhood S passes through the new vertex, so its length is
 * dist(u, v) + 2 for some pair u, v in S with the distance taken in the
 * current graph: girth >= g is preserved exactly when the members of S
 * are pairwise at distance >= g - 2 (the generalization of the
 * distance->= 3 rule of `snark_unlabeled_enum.h`, which is g = 5). The
 * completability pruning of the k-regular search (def(u) <= r,
 * sum def <= k*r, k*r - sum def even, with r vertices still to come)
 * carries over verbatim, and the Moore bound cuts the search off before
 * it starts: a k-regular graph with girth >= g (k >= 2) has at least
 * M(k, g) vertices — 1 + k*((k-1)^((g-1)/2) - 1)/(k-2) for odd g,
 * 2*((k-1)^(g/2) - 1)/(k-2) for even g — so n < M(k, g) yields nothing.
 *
 * Isomorph rejection is the canonical-parent test at every level, as in
 * the k-regular enumerator; completeness holds because every graph on
 * the canonical-deletion chain of a k-regular girth->= g graph is one of
 * its induced subgraphs and therefore satisfies both the degree and the
 * girth pruning conditions.
 *
 * g <= 3 places no constraint on a simple graph, so it reproduces
 * `enumerate_kregular_unlabeled_graphs` (OEIS triangle A051031); k = 2
 * yields the disjoint unions of cycles of length >= g (partitions of n
 * into parts >= g; C_n alone in connected mode for n >= g); k <= 1
 * graphs are acyclic (girth infinite), so the girth constraint is
 * vacuous there. (3,5) first becomes nonempty at n = 10 with exactly
 * the Petersen graph, (3,6) at n = 14 with the Heawood graph, and the
 * (3,5) counts are 1, 2, 9 for n = 10, 12, 14 (matching the connected
 * counts A014372 — no disconnected member exists below n = 20, where
 * twice the Petersen graph appears; connected cubic girth >= 4/5/6/7
 * are OEIS A014371/A014372/A014374/A014375).
 *
 * References:
 *   Meringer, "Fast generation of regular graphs and construction of
 *   cages," J. Graph Theory 30, 1999 (GENREG's degree- and
 *   girth-constrained orderly generation); McKay, "Isomorph-free
 *   exhaustive generation," J. Algorithms 26, 1998 (canonical
 *   construction path); Exoo, Jajcay, "Dynamic cage survey," Electron.
 *   J. Combin. DS16 (cage orders and the Moore bound); OEIS A051031,
 *   A000066, A014371, A014372, A014374, A014375
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic (k,g)-graph enumeration
 */
enum class CageUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path, degree- and girth-constrained */
};

/**
 * @brief An enumerated (k,g)-graph
 */
struct CageUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic (k,g)-graph enumeration
 */
struct CageUnlabeledEnumerationResult {
    std::vector<CageUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief The Moore lower bound on the order of a k-regular graph with girth >= g
 * @return The bound, capped at 2^40 so callers can compare against any n
 *
 * For k >= 3 and g >= 4 this is the classic Moore bound (Exoo-Jajcay
 * survey); girth > g satisfies the (larger) bound for that girth, so it
 * is valid for the whole girth->= g class. k = 2 needs a cycle of
 * length >= g, k <= 1 and g <= 3 reduce to the k-regularity minimum
 * n >= k + 1.
 */
inline long long cage_unlabeled_moore_bound(int k, int g) {
    const long long cap = 1LL << 40;
    if (k <= 1 || g <= 3) return k + 1;
    if (k == 2) return g;
    long long pw = 1;
    const int half = (g % 2 == 1) ? (g - 1) / 2 : g / 2;
    for (int i = 0; i < half; ++i) {
        pw *= k - 1;
        if (pw >= cap) return cap;
    }
    if (g % 2 == 1) return 1 + (long long)k * (pw - 1) / (k - 2);
    return 2 * (pw - 1) / (k - 2);
}

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline CageUnlabeledEnumeratedGraph cage_unlabeled_build_graph(
    int c, const std::vector<unsigned long long>& adj) {
    CageUnlabeledEnumeratedGraph g;
    g.n = c;
    g.edges = bitmask_graph_edges(c, adj);
    return g;
}

inline bool cage_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k, int g,
    bool connected_only, bool exists_only,
    std::vector<CageUnlabeledEnumeratedGraph>* results);

/**
 * @brief Tests the necessary degree-completability conditions after adding a vertex
 * @param c Number of vertices before the addition
 * @param adj Adjacency bitmasks of the c-vertex graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., c-1
 * @param s_size Population count of s
 * @param n Target number of vertices
 * @param k Target degree
 * @return true if the (c+1)-vertex graph may still extend to a k-regular graph
 *
 * Identical to the k-regular enumerator: with r = n - c - 1 future
 * vertices, requires def(u) <= r for every vertex, sum def <= k*r, and
 * k*r - sum def even. All three hold for every induced subgraph of a
 * k-regular n-vertex graph, so pruning on them never cuts a
 * canonical-deletion chain.
 */
inline bool cage_unlabeled_enum_feasible(
    int c, const std::vector<unsigned long long>& adj, unsigned long long s,
    int s_size, int n, int k) {
    const int r = n - c - 1;
    int total_def = k - s_size;
    if (total_def > r) return false;
    for (int u = 0; u < c; ++u) {
        int deg = ((s >> u) & 1ULL) ? 1 : 0;
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
        const int def = k - deg;
        if (def > r) return false;
        total_def += def;
    }
    if (total_def > k * r) return false;
    if ((k * r - total_def) % 2 != 0) return false;
    return true;
}

/**
 * @brief Extends the graph by a new vertex with neighborhood s and recurses
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., c-1
 * @param n Target number of vertices
 * @param k Target degree
 * @param g Girth lower bound
 * @param connected_only If true, emit only connected graphs
 * @param exists_only If true, stop the whole search at the first solution
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results (ignored when exists_only)
 * @return true when exists_only and a solution was found (abort the search)
 *
 * The child survives the canonical-parent test when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument.
 */
inline bool cage_unlabeled_enum_extend(
    int c, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, int k, int g, bool connected_only, bool exists_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<CageUnlabeledEnumeratedGraph>* results) {
    adj.push_back(s);
    for (int u = 0; u < c; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << c;
    }

    bool found = false;
    CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(c + 1, adj);
    if (((canon.last_orbit >> c) & 1ULL) &&
        child_forms.insert(canon.form).second) {
        found = cage_unlabeled_enum_dfs(c + 1, adj, n, k, g, connected_only,
                                        exists_only, results);
    }

    for (int u = 0; u < c; ++u) {
        if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << c);
    }
    adj.pop_back();
    return found;
}

/**
 * @brief Enumerates the candidate neighborhoods of the new vertex
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param available Vertices of degree < k, eligible as neighbors
 * @param start Next index into available to consider
 * @param s Neighborhood accumulated so far as a bitmask
 * @param s_size Number of vertices in s
 * @param allowed Vertices at distance >= g - 2 from every member of s
 * @param far_mask far_mask[u] = vertices at distance >= g - 2 from u
 * @param n Target number of vertices
 * @param k Target degree
 * @param g Girth lower bound
 * @param connected_only If true, emit only connected graphs
 * @param exists_only If true, stop the whole search at the first solution
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results (ignored when exists_only)
 * @return true when exists_only and a solution was found (abort the search)
 *
 * Combination DFS over the at-most-k-subsets of available whose members
 * are pairwise at distance >= g - 2 in the current graph (every new
 * cycle goes through the new vertex, so this is exactly girth
 * preservation); each subset that also passes the degree-completability
 * conditions becomes a candidate child.
 */
inline bool cage_unlabeled_enum_choose(
    int c, std::vector<unsigned long long>& adj,
    const std::vector<int>& available, std::size_t start,
    unsigned long long s, int s_size, unsigned long long allowed,
    const std::vector<unsigned long long>& far_mask,
    int n, int k, int g, bool connected_only, bool exists_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<CageUnlabeledEnumeratedGraph>* results) {
    if (cage_unlabeled_enum_feasible(c, adj, s, s_size, n, k)) {
        if (cage_unlabeled_enum_extend(c, adj, s, n, k, g, connected_only,
                                       exists_only, child_forms, results)) {
            return true;
        }
    }
    if (s_size == k) return false;
    for (std::size_t i = start; i < available.size(); ++i) {
        const int u = available[i];
        if (!((allowed >> u) & 1ULL)) continue;
        if (cage_unlabeled_enum_choose(c, adj, available, i + 1,
                                       s | (1ULL << u), s_size + 1,
                                       allowed & far_mask[u], far_mask,
                                       n, k, g, connected_only, exists_only,
                                       child_forms, results)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Canonical augmentation DFS from a c-vertex canonical representative
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param k Target degree
 * @param g Girth lower bound
 * @param connected_only If true, emit only connected graphs
 * @param exists_only If true, stop the whole search at the first solution
 * @param results Storage for results (ignored when exists_only)
 * @return true when exists_only and a solution was found (abort the search)
 *
 * A graph reaching level n is k-regular by construction (the
 * completability conditions force all degrees to k when no vertices
 * remain) and has girth >= g by the pairwise-distance invariant.
 * far_mask[u] is computed once per level by a BFS from u truncated at
 * depth g - 3 (the vertices within that distance are the forbidden
 * co-neighbors of u). Connectivity cannot be pruned during the search
 * (later vertices may join components), so `connected_only` filters at
 * emission.
 */
inline bool cage_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k, int g,
    bool connected_only, bool exists_only,
    std::vector<CageUnlabeledEnumeratedGraph>* results) {
    if (c == n) {
        if (!connected_only || bitmask_graph_connected(c, adj)) {
            if (exists_only) return true;
            results->push_back(cage_unlabeled_build_graph(c, adj));
        }
        return false;
    }
    std::vector<int> available;
    for (int u = 0; u < c; ++u) {
        int deg = 0;
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
        if (deg < k) available.push_back(u);
    }
    std::vector<unsigned long long> far_mask(c);
    for (int u = 0; u < c; ++u) {
        unsigned long long near = 1ULL << u;
        unsigned long long frontier = near;
        for (int step = 0; step < g - 3 && frontier != 0; ++step) {
            unsigned long long next = 0;
            for (int w = 0; w < c; ++w) {
                if ((frontier >> w) & 1ULL) next |= adj[w];
            }
            frontier = next & ~near;
            near |= frontier;
        }
        far_mask[u] = ~near;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    return cage_unlabeled_enum_choose(c, adj, available, 0, 0ULL, 0,
                                      ~0ULL, far_mask, n, k, g,
                                      connected_only, exists_only,
                                      child_forms, results);
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic k-regular graphs with girth >= g on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param k Target degree
 * @param girth Girth lower bound g (g <= 3 places no constraint; GENREG semantics)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return CageUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class. girth <= 3 reproduces
 * `enumerate_kregular_unlabeled_graphs` (OEIS triangle A051031); k = 2
 * yields the disjoint unions of cycles of length >= g; k <= 1 graphs
 * are acyclic, so the girth constraint is vacuous. The (k,g)-cages are
 * the output at the smallest n with nonempty output ((3,5): the
 * Petersen graph at n = 10, (3,6): the Heawood graph at n = 14). Odd
 * n*k, k < 0, k >= n >= 1, n below the Moore bound (for k >= 2) and
 * n >= 64 yield nothing; n = 0 with k = 0 yields the empty graph,
 * matching the k-regular enumerators.
 *
 * @note GENREG's throughput does not transfer: the per-child exact
 *       branch-and-bound canonicalization branches heavily on sparse
 *       high-girth graphs (locally tree-like, so many orderings tie),
 *       which bounds the practical range at about n = 14 for (3,5)
 *       (~4 s; n = 16 exceeds minutes) and n = 16 for (3,6) (~30 s) —
 *       canonicalizing the complement instead, the apollonian trick,
 *       was measured ~20x *slower* here. n below the Moore bound
 *       returns instantly for any k and g.
 */
inline CageUnlabeledEnumerationResult
enumerate_cage_unlabeled_graphs(
    int n, int k, int girth, bool connected_only = false,
    CageUnlabeledEnumAlgorithm algo =
        CageUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    CageUnlabeledEnumerationResult result;
    if (k < 0 || n < 0 || n >= 64) return result;
    if (n == 0) {
        if (k == 0) {
            CageUnlabeledEnumeratedGraph g;
            g.n = 0;
            result.graphs.push_back(g);
        }
        return result;
    }
    if ((long long)n * k % 2 != 0) return result;
    if (k >= n) return result;
    if (n < detail::cage_unlabeled_moore_bound(k, girth)) return result;
    std::vector<unsigned long long> adj(1, 0);
    detail::cage_unlabeled_enum_dfs(1, adj, n, k, girth, connected_only,
                                    false, &result.graphs);
    return result;
}

/**
 * @brief Does any k-regular graph with girth >= g on n vertices exist?
 * @param n Number of vertices (must be < 64)
 * @param k Target degree
 * @param girth Girth lower bound g (g <= 3 places no constraint)
 * @return true iff the enumeration for (n, k, girth) would be nonempty
 *
 * The same search with an early exit at the first graph reached
 * (disconnected graphs included), so a YES answer is usually far
 * cheaper than the enumeration while a NO answer costs the same — this
 * is the nonexistence side of a cage-order computation, and it is what
 * `check_cage` uses to certify minimality.
 */
inline bool cage_kg_graph_exists(int n, int k, int girth) {
    if (k < 0 || n < 0 || n >= 64) return false;
    if (n == 0) return k == 0;
    if ((long long)n * k % 2 != 0) return false;
    if (k >= n) return false;
    if (n < detail::cage_unlabeled_moore_bound(k, girth)) return false;
    std::vector<unsigned long long> adj(1, 0);
    return detail::cage_unlabeled_enum_dfs(1, adj, n, k, girth, false, true,
                                           0);
}

}  // namespace graph_recognition

#endif
