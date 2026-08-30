#ifndef GRAPH_RECOGNITION_TOURNAMENT_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_TOURNAMENT_UNLABELED_ENUM_H

/**
 * @file tournament_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic tournaments
 *
 * Enumerates one representative per isomorphism class of tournaments
 * (complete directed graphs) on n vertices by McKay's canonical
 * construction path method, the same scheme as nauty's dedicated
 * tournament generator `gentourng`. Tournaments are grown one vertex at a
 * time, which is sound because deleting any vertex of a tournament again
 * yields a tournament, so every class is reachable from the one-vertex
 * tournament; a child adds a new vertex together with one of the 2^k
 * orientation patterns against the k existing vertices, and no recognition
 * filter is needed because every intermediate digraph is a tournament by
 * construction.
 *
 * Isomorph rejection is the canonical-parent test of
 * `util/canonical_augmentation.h`, reusing `canonicalize_bitmask_graph`
 * verbatim on the **out-adjacency** bitmasks (bit v of row u set iff arc
 * u -> v). That reuse is sound only because the underlying graph is
 * complete: the canonical form packs each vertex's row against the
 * positions placed before it, and in a tournament an unset bit is exactly
 * the reversed arc, so the form determines the orientation of every pair
 * and is a complete isomorphism invariant, with the tied orderings
 * differing by automorphisms of the tournament (this argument fails for
 * general digraphs, whose form would not record non-adjacency versus
 * reversed arc).
 *
 * Number of non-isomorphic tournaments on n vertices = OEIS A000568(n):
 * 1, 1, 1, 2, 4, 12, 56, 456, 6880, 191536, ... for n = 0, 1, 2, ...
 * Every tournament on n >= 1 vertices is (weakly) connected, so no
 * `connected_only` flag exists, matching the labeled enumerator.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26(2),
 *   1998 (canonical construction path; `gentourng` in the nauty suite is
 *   this method specialized to tournaments);
 *   Moon, "Topics on Tournaments," Holt, Rinehart & Winston, 1968;
 *   OEIS A000568
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic tournament enumeration
 */
enum class TournamentUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (gentourng style) */
};

/**
 * @brief An enumerated tournament
 */
struct TournamentUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > arcs;       /**< Directed arc list (u, v) = u->v, sorted */
};

/**
 * @brief Result of non-isomorphic tournament enumeration
 */
struct TournamentUnlabeledEnumerationResult {
    std::vector<TournamentUnlabeledEnumeratedGraph> graphs; /**< Array of enumerated tournaments */
};

namespace detail {

/** @brief Converts out-adjacency bitmasks to an enumerated tournament */
inline TournamentUnlabeledEnumeratedGraph tournament_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& out) {
    TournamentUnlabeledEnumeratedGraph g;
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
 * @param out Out-adjacency bitmasks of the current tournament
 * @param n Target number of vertices
 * @param results Storage for results
 *
 * Extends by a new vertex whose out-neighborhood s runs over all subsets
 * of the current vertex set (the in-neighborhood is the complement, so the
 * child is again a tournament). A child survives when the new vertex lies
 * in its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument.
 */
inline void tournament_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& out, int n,
    std::vector<TournamentUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        results.push_back(tournament_unlabeled_build_graph(k, out));
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        out.push_back(s);
        for (int u = 0; u < k; ++u) {
            if (!((s >> u) & 1ULL)) out[u] |= 1ULL << k;
        }

        CanonicalAugmentationCanon canon =
            canonicalize_bitmask_graph(k + 1, out);
        if (((canon.last_orbit >> k) & 1ULL) &&
            child_forms.insert(canon.form).second) {
            tournament_unlabeled_enum_dfs(k + 1, out, n, results);
        }

        for (int u = 0; u < k; ++u) {
            out[u] &= ~(1ULL << k);
        }
        out.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic tournaments on n vertices
 * @param n Number of vertices (must be < 64)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return TournamentUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000568(n) tournaments
 * (1, 1, 2, 4, 12, 56, 456, 6880, 191536, ... for n = 1, 2, ...). n = 0
 * yields the single empty tournament, matching the labeled enumerator and
 * A000568(0) = 1; negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one exact canonicalization (branch and
 *       bound over vertex orderings), so the enumeration is practical to
 *       about n = 9 (191536 classes); tournaments have few automorphisms,
 *       which keeps the branch and bound far from its k! worst case.
 */
inline TournamentUnlabeledEnumerationResult
enumerate_tournament_unlabeled_graphs(
    int n,
    TournamentUnlabeledEnumAlgorithm algo =
        TournamentUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    TournamentUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        TournamentUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> out(1, 0);
    detail::tournament_unlabeled_enum_dfs(1, out, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
