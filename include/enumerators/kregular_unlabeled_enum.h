#ifndef GRAPH_RECOGNITION_KREGULAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_KREGULAR_UNLABELED_ENUM_H

/**
 * @file kregular_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic k-regular graphs
 *
 * Enumerates one representative per isomorphism class of k-regular graphs
 * (every degree exactly k) on n vertices by McKay's canonical construction
 * path method with the degree constraint driving the search, the
 * orderly-generation strategy of GENREG lifted onto the shared
 * canonical-augmentation machinery (and the degree-parameterized
 * generalization of `cubic_unlabeled_enum.h`, which is the k = 3 special
 * case). The class is not hereditary, but every induced subgraph of a
 * k-regular graph has maximum degree at most k, so the intermediate
 * levels range over the hereditary class of graphs with maximum degree
 * <= k: the new vertex's neighborhood runs only over the at-most-k-subsets
 * of the vertices of degree < k. Children are further pruned by necessary
 * completability conditions that every induced subgraph of a k-regular
 * n-vertex graph satisfies, with r vertices still to come and
 * def(u) = k - deg(u): def(u) <= r for every vertex (a vertex collects at
 * most one edge per future vertex), sum def <= k*r (future vertices
 * contribute at most k endpoints each), and k*r - sum def even (edges
 * among future vertices absorb endpoints in pairs). At the last level
 * (r = 0) these force all degrees to k, so every graph reaching level n
 * is k-regular by construction.
 *
 * Isomorph rejection is the canonical-parent test at every level: one
 * canonicalization of each candidate child yields both its canonical form
 * and the automorphism orbit of the vertex placed last by the canonical
 * labeling, and the child survives only when the newly added vertex lies
 * in that orbit (so each class accepts exactly one parent class), with
 * children of the same parent deduplicated by canonical form. The
 * canonicalization itself lives in `util/canonical_augmentation.h`.
 * Completeness holds because the pruning conditions are necessary: every
 * graph on the canonical-deletion chain of a k-regular graph is one of
 * its induced subgraphs and therefore passes them.
 *
 * Number of non-isomorphic k-regular graphs on n vertices = the triangle
 * OEIS A051031: row n = 4 is 1, 1, 1, 1 (k = 0..3), row n = 6, k = 3 is
 * 2, and column k = 2 counts the partitions of n into parts >= 3 (disjoint
 * unions of cycles). k = 3 reproduces A005638, 4-regular is A033301,
 * 5-regular A165626. No k-regular graph exists when n*k is odd or k >= n
 * (except the 0-vertex 0-regular empty graph, kept for consistency with
 * the labeled enumerator).
 *
 * References:
 *   Meringer, "Fast generation of regular graphs and construction of
 *   cages," J. Graph Theory 30, 1999 (GENREG's degree-constrained orderly
 *   generation); McKay, "Isomorph-free exhaustive generation," J.
 *   Algorithms 26, 1998 (canonical construction path); OEIS A051031,
 *   A005176, A033301, A165626
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic k-regular enumeration
 */
enum class KRegularUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path, degree-constrained */
};

/**
 * @brief An enumerated k-regular graph
 */
struct KRegularUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic k-regular enumeration
 */
struct KRegularUnlabeledEnumerationResult {
    std::vector<KRegularUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline KRegularUnlabeledEnumeratedGraph kregular_unlabeled_build_graph(
    int c, const std::vector<unsigned long long>& adj) {
    KRegularUnlabeledEnumeratedGraph g;
    g.n = c;
    g.edges = bitmask_graph_edges(c, adj);
    return g;
}

inline void kregular_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k,
    bool connected_only,
    std::vector<KRegularUnlabeledEnumeratedGraph>& results);

/**
 * @brief Tests the necessary completability conditions after adding a vertex
 * @param c Number of vertices before the addition
 * @param adj Adjacency bitmasks of the c-vertex graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., c-1
 * @param s_size Population count of s
 * @param n Target number of vertices
 * @param k Target degree
 * @return true if the (c+1)-vertex graph may still extend to a k-regular graph
 *
 * With r = n - c - 1 future vertices, requires def(u) <= r for every
 * vertex, sum def <= k*r, and k*r - sum def even. All three hold for
 * every induced subgraph of a k-regular n-vertex graph, so pruning on
 * them never cuts a canonical-deletion chain.
 */
inline bool kregular_unlabeled_enum_feasible(
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
 * @param connected_only If true, emit only connected graphs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * The child survives the canonical-parent test when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument.
 */
inline void kregular_unlabeled_enum_extend(
    int c, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, int k, bool connected_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<KRegularUnlabeledEnumeratedGraph>& results) {
    adj.push_back(s);
    for (int u = 0; u < c; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << c;
    }

    CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(c + 1, adj);
    if (((canon.last_orbit >> c) & 1ULL) &&
        child_forms.insert(canon.form).second) {
        kregular_unlabeled_enum_dfs(c + 1, adj, n, k, connected_only,
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
 * @param connected_only If true, emit only connected graphs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * Standard combination DFS over the at-most-k-subsets of available; each
 * subset that passes the completability conditions becomes a candidate
 * child.
 */
inline void kregular_unlabeled_enum_choose(
    int c, std::vector<unsigned long long>& adj,
    const std::vector<int>& available, std::size_t start,
    unsigned long long s, int s_size, int n, int k, bool connected_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<KRegularUnlabeledEnumeratedGraph>& results) {
    if (kregular_unlabeled_enum_feasible(c, adj, s, s_size, n, k)) {
        kregular_unlabeled_enum_extend(c, adj, s, n, k, connected_only,
                                       child_forms, results);
    }
    if (s_size == k) return;
    for (std::size_t i = start; i < available.size(); ++i) {
        kregular_unlabeled_enum_choose(c, adj, available, i + 1,
                                       s | (1ULL << available[i]), s_size + 1,
                                       n, k, connected_only, child_forms,
                                       results);
    }
}

/**
 * @brief Canonical augmentation DFS from a c-vertex canonical representative
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param k Target degree
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * A graph reaching level n is k-regular by construction (the
 * completability conditions force all degrees to k when no vertices
 * remain). Connectivity cannot be pruned during the search (later
 * vertices may join components), so `connected_only` filters at emission.
 */
inline void kregular_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k,
    bool connected_only,
    std::vector<KRegularUnlabeledEnumeratedGraph>& results) {
    if (c == n) {
        if (!connected_only || bitmask_graph_connected(c, adj)) {
            results.push_back(kregular_unlabeled_build_graph(c, adj));
        }
        return;
    }
    std::vector<int> available;
    for (int u = 0; u < c; ++u) {
        int deg = 0;
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
        if (deg < k) available.push_back(u);
    }
    std::set<std::vector<unsigned long long> > child_forms;
    kregular_unlabeled_enum_choose(c, adj, available, 0, 0ULL, 0, n, k,
                                   connected_only, child_forms, results);
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic k-regular graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param k Target degree
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return KRegularUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: row n, column k of the
 * OEIS triangle A051031. k = 0 yields the empty graph (one class),
 * k = 2 the disjoint unions of cycles (partitions of n into parts >= 3),
 * k = 3 reproduces A005638 (matching `enumerate_cubic_unlabeled_graphs`),
 * k = 4 is A033301. Odd n*k, k < 0, k >= n >= 1 and n >= 64 yield
 * nothing; n = 0 with k = 0 yields the empty graph, matching
 * `enumerate_kregular_labeled_graphs_reverse_search`.
 *
 * @note The intermediate levels range over the graphs with maximum degree
 *       <= k passing the completability pruning, with one exact
 *       branch-and-bound canonicalization per candidate child. The
 *       canonicalization's ordering search branches heavily on the sparse
 *       low-degree intermediates, so the enumeration is practical to
 *       about n = 14 for k = 3 and to about n = 12 for k = 4 or 5
 *       (n = 12: the 1547 4-regular graphs take about 20 seconds, the
 *       7849 5-regular ones about 2.5 minutes).
 */
inline KRegularUnlabeledEnumerationResult
enumerate_kregular_unlabeled_graphs(
    int n, int k, bool connected_only = false,
    KRegularUnlabeledEnumAlgorithm algo =
        KRegularUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    KRegularUnlabeledEnumerationResult result;
    if (k < 0 || n < 0 || n >= 64) return result;
    if (n == 0) {
        if (k == 0) {
            KRegularUnlabeledEnumeratedGraph g;
            g.n = 0;
            result.graphs.push_back(g);
        }
        return result;
    }
    if ((long long)n * k % 2 != 0) return result;
    if (k >= n) return result;
    std::vector<unsigned long long> adj(1, 0);
    detail::kregular_unlabeled_enum_dfs(1, adj, n, k, connected_only,
                                        result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
