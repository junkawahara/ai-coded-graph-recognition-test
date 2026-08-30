#ifndef GRAPH_RECOGNITION_POLYHEDRAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_POLYHEDRAL_UNLABELED_ENUM_H

/**
 * @file polyhedral_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic polyhedral graphs (3-connected planar)
 *
 * Enumerates one representative per isomorphism class of polyhedral
 * graphs (3-connected planar graphs, by Steinitz's theorem the skeletons
 * of convex polyhedra; n >= 4) on n vertices by McKay's canonical
 * construction path method. The class itself is not hereditary — deleting
 * a vertex can destroy 3-connectivity — but every vertex-deleted subgraph
 * is still planar, so the search grows *planar* graphs one vertex at a
 * time exactly as in `planar_unlabeled_enum.h` and runs the full
 * `check_polyhedral` (3-connectivity + planarity) only on the n-vertex
 * graphs, mirroring the labeled enumerator's prune-then-verify scheme.
 *
 * Two prunings run on every candidate child before the isomorph
 * rejection: a `check_planar` call (the linear-time left-right planarity
 * criterion), and an edge-count window — an n-vertex polyhedral graph
 * has ceil(3n / 2) <= m <= 3n - 6 (minimum degree 3 and planarity), and
 * a child on k + 1 vertices with m' edges is kept only when
 * m' <= 3n - 6 and m' plus the maximum number of edges the remaining
 * n - k - 1 vertices can still add (vertex j can add at most j - 1
 * edges) reaches ceil(3n / 2). Both tests depend only on the child's
 * isomorphism class (planarity and the edge count are invariants), and
 * the canonical-deletion chain of every polyhedral graph stays inside
 * the window (edges only ever get removed along it, and the vertex
 * deleted at level j + 1 has degree at most j), so McKay's
 * one-parent-per-class argument is unaffected.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic polyhedral graphs on n >= 4 vertices =
 * number of combinatorial types of convex polyhedra with n vertices =
 * OEIS A000944(n): 1, 2, 7, 34, 257, 2606, 32300, ... for n = 4, 5, ...;
 * there are none below n = 4. Every output is connected (3-connected a
 * fortiori), so there is no `connected_only` flag.
 *
 * The dedicated generator for this class is plantri's canonical
 * construction path over the embedded 3-connected planar graphs
 * themselves (millions of graphs per second); this implementation
 * instead reuses the shared canonical-augmentation machinery, whose
 * per-child exact canonicalization keeps it practical only to about
 * n = 9.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Brinkmann, McKay, "Fast generation of planar graphs," MATCH Commun.
 *   Math. Comput. Chem. 58, 2007 (plantri; the dedicated algorithm);
 *   Brandes, "The left-right planarity test," 2009 (the recognizer);
 *   OEIS A000944
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/planar.h"
#include "recognizers/polyhedral.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic polyhedral enumeration
 */
enum class PolyhedralUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated polyhedral graph
 */
struct PolyhedralUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic polyhedral enumeration
 */
struct PolyhedralUnlabeledEnumerationResult {
    std::vector<PolyhedralUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline PolyhedralUnlabeledEnumeratedGraph polyhedral_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    PolyhedralUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param edge_count Number of edges of the current graph
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param min_edges Minimum edge count of an n-vertex polyhedral graph
 * @param max_edges Maximum edge count of an n-vertex polyhedral graph
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over the subsets of
 * the current vertex set whose size lies in the edge-count window (see
 * the file comment), keeping those for which the extended graph is still
 * planar. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. 3-connectivity is not hereditary,
 * so at the last level the full `check_polyhedral` decides emission.
 */
inline void polyhedral_unlabeled_enum_dfs(
    int k, int edge_count, std::vector<unsigned long long>& adj, int n,
    int min_edges, int max_edges,
    std::vector<PolyhedralUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        Graph g(k, bitmask_graph_edges(k, adj));
        if (check_polyhedral(g).is_polyhedral) {
            results.push_back(polyhedral_unlabeled_build_graph(k, adj));
        }
        return;
    }

    // Edge-count window for the new vertex's degree. Vertices still to be
    // added after this one are k + 2, ..., n (1-indexed sizes), and vertex
    // j can contribute at most j - 1 edges.
    long long remaining_after = 0;
    for (int j = k + 2; j <= n; ++j) remaining_after += j - 1;
    long long d_min_ll = min_edges - edge_count - remaining_after;
    int d_min = d_min_ll > 0 ? static_cast<int>(d_min_ll) : 0;
    int d_max = max_edges - edge_count;
    if (d_max > k) d_max = k;
    if (d_min > d_max) return;

    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        int deg = 0;
        for (unsigned long long t = s; t; t &= t - 1) ++deg;
        if (deg < d_min || deg > d_max) continue;

        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        Graph child(k + 1, bitmask_graph_edges(k + 1, adj));
        if (check_planar(child).is_planar) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                polyhedral_unlabeled_enum_dfs(k + 1, edge_count + deg, adj,
                                              n, min_edges, max_edges,
                                              results);
            }
        }

        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic polyhedral graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return PolyhedralUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: the A000944(n)
 * 3-connected planar graphs (1, 2, 7, 34, 257, 2606, ... for
 * n = 4, 5, ...). n < 4 yields nothing (no graph on fewer than 4
 * vertices is 3-connected), matching `enumerate_polyhedral_labeled_graphs`;
 * n >= 64 also yields nothing. Every emitted graph is connected, so no
 * `connected_only` flag exists.
 *
 * @note The search runs over all unlabeled planar graphs inside the
 *       edge-count window, and every candidate child costs one linear-time
 *       planarity recognition plus, when it is planar, one exact
 *       canonicalization (branch and bound over vertex orderings, worst
 *       case k! on vertex-transitive graphs), so the enumeration is
 *       practical to about n = 9.
 */
inline PolyhedralUnlabeledEnumerationResult
enumerate_polyhedral_unlabeled_graphs(
    int n,
    PolyhedralUnlabeledEnumAlgorithm algo =
        PolyhedralUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    PolyhedralUnlabeledEnumerationResult result;
    if (n < 4 || n >= 64) return result;
    const int min_edges = (3 * n + 1) / 2;
    const int max_edges = 3 * n - 6;
    std::vector<unsigned long long> adj(1, 0);
    detail::polyhedral_unlabeled_enum_dfs(1, 0, adj, n, min_edges, max_edges,
                                          result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
