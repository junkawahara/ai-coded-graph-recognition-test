#ifndef GRAPH_RECOGNITION_CUBIC_PLANAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CUBIC_PLANAR_UNLABELED_ENUM_H

/**
 * @file cubic_planar_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic cubic planar graphs
 *
 * Enumerates one representative per isomorphism class of cubic planar
 * graphs (every degree exactly 3 and planar) on n vertices by McKay's
 * canonical construction path method, the degree-constrained search of
 * `cubic_unlabeled_enum.h` with a planarity prune on every candidate
 * child. Neither constraint breaks the one-parent-per-class argument:
 * every vertex-deleted subgraph of a cubic planar graph has maximum
 * degree at most 3 and is still planar (planarity is hereditary), so the
 * intermediate levels range over the hereditary class of planar graphs
 * with maximum degree <= 3 and no canonical-deletion chain is ever cut.
 *
 * The new vertex's neighborhood runs only over the at-most-3-subsets of
 * the vertices of degree < 3 (O(k^3) candidates instead of 2^k), and
 * children are pruned by the necessary completability conditions of the
 * cubic search — with r vertices still to come and def(u) = 3 - deg(u):
 * def(u) <= r for every vertex, sum def <= 3r, and 3r - sum def even —
 * plus one `check_planar` call (the linear-time left-right criterion)
 * per candidate child. A graph with maximum degree <= 3 on at most 5
 * vertices is always planar (K5 needs degree 4 and K3,3 needs 6
 * vertices), so the planarity check starts at 6-vertex children. At the
 * last level the completability conditions force all degrees to 3, so
 * every graph reaching level n is cubic planar by construction.
 *
 * Isomorph rejection is the canonical-parent test at every level: one
 * canonicalization of each candidate child yields both its canonical form
 * and the automorphism orbit of the vertex placed last by the canonical
 * labeling, and the child survives only when the newly added vertex lies
 * in that orbit (so each class accepts exactly one parent class), with
 * children of the same parent deduplicated by canonical form. The
 * canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic connected cubic planar graphs on n vertices =
 * OEIS A005964 (n = 4, 6, 8, ...): 1, 1, 3, 9, 32, 133, ... The total
 * counts allowing disconnected graphs (1, 1, 4, 10, 37, 146, ... for
 * n = 4, 6, 8, ...) are not in the OEIS. No cubic graph exists for odd n
 * or n < 4 (matching `check_cubic_planar`, which rejects the 0-vertex
 * graph).
 *
 * The dedicated generator for this class is plantri's canonical
 * construction path (Brinkmann-McKay 2007): generate plane triangulations
 * and dualize, or generate embedded cubic planar graphs directly; this
 * implementation instead reuses the shared canonical-augmentation
 * machinery, whose per-child exact canonicalization keeps it practical
 * only to about n = 14; plantri's throughput does not apply.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path); Brinkmann, McKay, "Fast generation of
 *   planar graphs," MATCH Commun. Math. Comput. Chem. 58, 2007 (plantri;
 *   the dedicated algorithm); Brandes, "The left-right planarity test,"
 *   2009 (the recognizer); OEIS A005964
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/planar.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic cubic planar enumeration
 */
enum class CubicPlanarUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path, degree- and planarity-constrained */
};

/**
 * @brief An enumerated cubic planar graph
 */
struct CubicPlanarUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic cubic planar enumeration
 */
struct CubicPlanarUnlabeledEnumerationResult {
    std::vector<CubicPlanarUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline CubicPlanarUnlabeledEnumeratedGraph cubic_planar_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    CubicPlanarUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

inline void cubic_planar_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<CubicPlanarUnlabeledEnumeratedGraph>& results);

/**
 * @brief Tests the necessary completability conditions after adding a vertex
 * @param k Number of vertices before the addition
 * @param adj Adjacency bitmasks of the k-vertex graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., k-1
 * @param s_size Population count of s
 * @param n Target number of vertices
 * @return true if the (k+1)-vertex graph may still extend to a cubic graph
 *
 * With r = n - k - 1 future vertices, requires def(u) <= r for every
 * vertex, sum def <= 3r, and 3r - sum def even. All three hold for every
 * induced subgraph of a cubic n-vertex graph, so pruning on them never
 * cuts a canonical-deletion chain.
 */
inline bool cubic_planar_unlabeled_enum_feasible(
    int k, const std::vector<unsigned long long>& adj, unsigned long long s,
    int s_size, int n) {
    const int r = n - k - 1;
    int total_def = 3 - s_size;
    if (total_def > r) return false;
    for (int u = 0; u < k; ++u) {
        int deg = ((s >> u) & 1ULL) ? 1 : 0;
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
        const int def = 3 - deg;
        if (def > r) return false;
        total_def += def;
    }
    if (total_def > 3 * r) return false;
    if ((3 * r - total_def) % 2 != 0) return false;
    return true;
}

/**
 * @brief Extends the graph by a new vertex with neighborhood s and recurses
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., k-1
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * A non-planar child is discarded before the canonicalization (safe:
 * planarity is an isomorphism invariant, and every vertex-deleted
 * subgraph of a planar graph is planar, so the canonical-deletion chain
 * of a cubic planar graph never leaves the pruned set). The planarity
 * check is skipped for children on at most 5 vertices, which are always
 * planar under the degree <= 3 constraint. The surviving child passes the
 * canonical-parent test when the new vertex lies in its canonical-deletion
 * orbit and its canonical form was not already produced by a sibling;
 * correctness of accepting one parent per class is McKay's canonical
 * construction path argument.
 */
inline void cubic_planar_unlabeled_enum_extend(
    int k, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, bool connected_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<CubicPlanarUnlabeledEnumeratedGraph>& results) {
    adj.push_back(s);
    for (int u = 0; u < k; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
    }

    bool planar = true;
    if (k + 1 >= 6) {
        Graph child(k + 1, bitmask_graph_edges(k + 1, adj));
        planar = check_planar(child).is_planar;
    }
    if (planar) {
        CanonicalAugmentationCanon canon =
            canonicalize_bitmask_graph(k + 1, adj);
        if (((canon.last_orbit >> k) & 1ULL) &&
            child_forms.insert(canon.form).second) {
            cubic_planar_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
                                            results);
        }
    }

    for (int u = 0; u < k; ++u) {
        if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
    }
    adj.pop_back();
}

/**
 * @brief Enumerates the candidate neighborhoods of the new vertex
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param available Vertices of degree < 3, eligible as neighbors
 * @param start Next index into available to consider
 * @param s Neighborhood accumulated so far as a bitmask
 * @param s_size Number of vertices in s
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * Standard combination DFS over the at-most-3-subsets of available; each
 * subset that passes the completability conditions becomes a candidate
 * child.
 */
inline void cubic_planar_unlabeled_enum_choose(
    int k, std::vector<unsigned long long>& adj,
    const std::vector<int>& available, std::size_t start,
    unsigned long long s, int s_size, int n, bool connected_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<CubicPlanarUnlabeledEnumeratedGraph>& results) {
    if (cubic_planar_unlabeled_enum_feasible(k, adj, s, s_size, n)) {
        cubic_planar_unlabeled_enum_extend(k, adj, s, n, connected_only,
                                           child_forms, results);
    }
    if (s_size == 3) return;
    for (std::size_t i = start; i < available.size(); ++i) {
        cubic_planar_unlabeled_enum_choose(
            k, adj, available, i + 1, s | (1ULL << available[i]), s_size + 1,
            n, connected_only, child_forms, results);
    }
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * A graph reaching level n is cubic planar by construction (the
 * completability conditions force all degrees to 3 when no vertices
 * remain, and every level passed the planarity prune). Connectivity
 * cannot be pruned during the search (later vertices may join
 * components), so `connected_only` filters at emission.
 */
inline void cubic_planar_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<CubicPlanarUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(cubic_planar_unlabeled_build_graph(k, adj));
        }
        return;
    }
    std::vector<int> available;
    for (int u = 0; u < k; ++u) {
        int deg = 0;
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
        if (deg < 3) available.push_back(u);
    }
    std::set<std::vector<unsigned long long> > child_forms;
    cubic_planar_unlabeled_enum_choose(k, adj, available, 0, 0ULL, 0, n,
                                       connected_only, child_forms, results);
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic cubic planar graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return CubicPlanarUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: 1, 1, 4, 10, 37, 146,
 * ... graphs for n = 4, 6, 8, ... (not in the OEIS), or A005964
 * (1, 1, 3, 9, 32, 133, ...) with @p connected_only. Odd n, n < 4
 * (including the 0-vertex graph, which `check_cubic_planar` rejects) and
 * n >= 64 yield nothing.
 *
 * @note The intermediate levels range over the planar graphs with maximum
 *       degree <= 3 passing the completability pruning, with one
 *       `check_planar` call and one exact branch-and-bound
 *       canonicalization per candidate child. The canonicalization's
 *       ordering search branches heavily on the sparse low-degree
 *       intermediates, so the enumeration is practical to about n = 14,
 *       as for `enumerate_cubic_unlabeled_graphs`.
 */
inline CubicPlanarUnlabeledEnumerationResult
enumerate_cubic_planar_unlabeled_graphs(
    int n, bool connected_only = false,
    CubicPlanarUnlabeledEnumAlgorithm algo =
        CubicPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    CubicPlanarUnlabeledEnumerationResult result;
    if (n < 4 || n >= 64 || n % 2 != 0) return result;
    std::vector<unsigned long long> adj(1, 0);
    detail::cubic_planar_unlabeled_enum_dfs(1, adj, n, connected_only,
                                            result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
