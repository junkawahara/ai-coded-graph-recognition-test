#ifndef GRAPH_RECOGNITION_DIGRAPH_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_DIGRAPH_UNLABELED_ENUM_H

/**
 * @file digraph_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic digraphs
 *
 * Enumerates one representative per isomorphism class of simple digraphs
 * (no self-loops; both arcs of a pair allowed) on n vertices by McKay's
 * canonical construction path method. The nauty suite obtains the same
 * lists via `geng n | directg` (orient every unlabeled undirected graph in
 * all ways with isomorph suppression); here the digraphs are instead grown
 * one vertex at a time directly, the scheme the repository's other
 * unlabeled enumerators share. Vertex-by-vertex growth is sound because
 * deleting any vertex of a digraph again yields a digraph, so every class
 * is reachable from the one-vertex digraph; a child adds a new vertex
 * together with one of the 4^k combinations of an out-neighborhood and an
 * in-neighborhood against the k existing vertices, and no recognition
 * filter is needed because every intermediate digraph is simple by
 * construction.
 *
 * Isomorph rejection is the canonical-parent test of
 * `util/canonical_augmentation.h` using `canonicalize_bitmask_digraph`,
 * the genuinely directed canonical form (two bits per ordered pair). The
 * undirected canonicalizer that `tournament_unlabeled_enum.h` reuses on
 * out-adjacency bitmasks is NOT sound here: with an incomplete underlying
 * graph an unset packed bit would conflate non-adjacency with a reversed
 * arc.
 *
 * Number of non-isomorphic digraphs on n vertices = OEIS A000273(n):
 * 1, 1, 3, 16, 218, 9608, 1540944, 882033440, ... for n = 0, 1, 2, ...
 * The labeled enumerator (`digraph_labeled_enum.h`) produces all
 * 2^(n(n-1)) labeled digraphs; no `connected_only` flag exists, matching
 * it (A003085 counts the weakly connected classes).
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26(2),
 *   1998 (canonical construction path; `directg` in the nauty suite);
 *   Harary, Palmer, "Graphical Enumeration," Academic Press, 1973;
 *   OEIS A000273
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic digraph enumeration
 */
enum class DigraphUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path */
};

/**
 * @brief An enumerated digraph
 */
struct DigraphUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > arcs;       /**< Directed arc list (u, v) = u->v, sorted */
};

/**
 * @brief Result of non-isomorphic digraph enumeration
 */
struct DigraphUnlabeledEnumerationResult {
    std::vector<DigraphUnlabeledEnumeratedGraph> graphs; /**< Array of enumerated digraphs */
};

namespace detail {

/** @brief Converts out-adjacency bitmasks to an enumerated digraph */
inline DigraphUnlabeledEnumeratedGraph digraph_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& out) {
    DigraphUnlabeledEnumeratedGraph g;
    g.n = k;
    for (int u = 0; u < k; ++u) {
        for (int v = 0; v < k; ++v) {
            if ((out[u] >> v) & 1ULL) {
                g.arcs.push_back(std::make_pair(u + 1, v + 1));
            }
        }
    }
    std::sort(g.arcs.begin(), g.arcs.end());
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param out Out-adjacency bitmasks of the current digraph
 * @param n Target number of vertices
 * @param results Storage for results
 *
 * Extends by a new vertex whose out-neighborhood s_out and in-neighborhood
 * s_in independently run over all subsets of the current vertex set (a
 * vertex in both sets gets both arcs). A child survives when the new
 * vertex lies in its canonical-deletion orbit and its canonical form was
 * not already produced by a sibling; correctness of accepting one parent
 * per class is McKay's canonical construction path argument.
 */
inline void digraph_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& out, int n,
    std::vector<DigraphUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        results.push_back(digraph_unlabeled_build_graph(k, out));
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s_out = 0; s_out < limit; ++s_out) {
        for (unsigned long long s_in = 0; s_in < limit; ++s_in) {
            out.push_back(s_out);
            for (int u = 0; u < k; ++u) {
                if ((s_in >> u) & 1ULL) out[u] |= 1ULL << k;
            }

            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_digraph(k + 1, out);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                digraph_unlabeled_enum_dfs(k + 1, out, n, results);
            }

            for (int u = 0; u < k; ++u) {
                out[u] &= ~(1ULL << k);
            }
            out.pop_back();
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic digraphs on n vertices
 * @param n Number of vertices (must be <= 33, the directed canonical
 *        form's width bound)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return DigraphUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000273(n) digraphs
 * (1, 3, 16, 218, 9608, 1540944, ... for n = 1, 2, ...). n = 0 yields the
 * single empty digraph, matching the labeled enumerator and A000273(0) = 1;
 * negative n and n > 33 yield nothing.
 *
 * @note Every accepted k-vertex representative spawns 4^k candidate
 *       children, each costing one exact canonicalization (branch and
 *       bound over vertex orderings), so the enumeration is practical to
 *       about n = 6 (1540944 classes, ~10^7 canonicalizations, ~7 s);
 *       n = 7 (882033440 classes) is out of reach.
 */
inline DigraphUnlabeledEnumerationResult
enumerate_digraph_unlabeled_graphs(
    int n,
    DigraphUnlabeledEnumAlgorithm algo =
        DigraphUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    DigraphUnlabeledEnumerationResult result;
    if (n < 0 || n > 33) return result;
    if (n == 0) {
        DigraphUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> out(1, 0);
    detail::digraph_unlabeled_enum_dfs(1, out, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
