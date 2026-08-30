#ifndef GRAPH_RECOGNITION_POSET_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_POSET_UNLABELED_ENUM_H

/**
 * @file poset_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic posets
 *
 * Enumerates one representative per isomorphism class of partial orders
 * on n elements by McKay's canonical construction path method. The
 * dedicated generator genposetg (Brinkmann-McKay 2002, in the nauty
 * suite) builds Hasse diagrams level by level, adding a whole antichain
 * of new maximal elements at a time; here the posets are instead grown
 * one element at a time as transitive closures, the scheme the
 * repository's other unlabeled enumerators share. Element-by-element
 * growth is sound because deleting any element of a poset again yields a
 * poset (the induced subrelation stays reflexive, antisymmetric and
 * transitive), so every class is reachable from the one-element poset.
 *
 * The object grown and canonicalized is the strict-order digraph (the
 * full transitive closure, arc u->v iff u <_P v), NOT the Hasse diagram:
 * deleting a vertex of the closure is again the closure of the induced
 * subposet, whereas deleting a vertex of a Hasse diagram is in general
 * not the Hasse diagram of the induced subposet (covers can appear when
 * a middle element disappears), which would break the canonical-parent
 * argument. A child adds a new element w together with a down-set of
 * predecessors s_in and an up-set of successors s_out: the closure
 * property forces s_out to contain the successors of its members, s_in
 * to contain the predecessors of its members, and every s_in member to
 * lie below every s_out member; under those constraints the extended
 * digraph is transitively closed and acyclic by construction, so no
 * recognition filter is needed. At most 3^k of the 4^k subset pairs
 * survive the disjointness alone, and the closure constraints prune far
 * below that.
 *
 * Isomorph rejection is the canonical-parent test of
 * `util/canonical_augmentation.h` using `canonicalize_bitmask_digraph`,
 * the genuinely directed canonical form (two bits per ordered pair);
 * poset isomorphism coincides with digraph isomorphism of the closures.
 *
 * Output is the Hasse diagram (covering relation) of each poset,
 * extracted from the closure at the leaves, matching the labeled
 * enumerator (`poset_labeled_enum.h`) and `check_poset`.
 *
 * Number of non-isomorphic posets on n elements = OEIS A000112(n):
 * 1, 1, 2, 5, 16, 63, 318, 2045, 16999, 183231, ... for n = 0, 1, 2, ...
 * No `connected_only` flag exists, matching the labeled enumerator
 * (A000608 counts the connected classes).
 *
 * References:
 *   Brinkmann, McKay, "Posets on up to 16 Points," Order 19(2), 2002
 *   (genposetg); McKay, "Isomorph-free exhaustive generation,"
 *   J. Algorithms 26(2), 1998 (canonical construction path);
 *   OEIS A000112
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic poset enumeration
 */
enum class PosetUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path */
};

/**
 * @brief An enumerated poset (Hasse diagram representation)
 */
struct PosetUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of elements */
    std::vector<std::pair<int, int> > arcs;       /**< Hasse diagram arc (u, v) = u < v (covering relation), sorted */
};

/**
 * @brief Result of non-isomorphic poset enumeration
 */
struct PosetUnlabeledEnumerationResult {
    std::vector<PosetUnlabeledEnumeratedGraph> graphs; /**< Array of enumerated posets */
};

namespace detail {

/**
 * @brief Extracts the Hasse diagram from closure out-adjacency bitmasks
 *
 * Arc (u, v) is a covering relation when u < v in the closure and no w
 * with u < w < v exists.
 */
inline PosetUnlabeledEnumeratedGraph poset_unlabeled_build_hasse(
    int k, const std::vector<unsigned long long>& out) {
    PosetUnlabeledEnumeratedGraph g;
    g.n = k;
    for (int u = 0; u < k; ++u) {
        for (int v = 0; v < k; ++v) {
            if (!((out[u] >> v) & 1ULL)) continue;
            bool is_cover = true;
            for (int w = 0; w < k; ++w) {
                if (((out[u] >> w) & 1ULL) && ((out[w] >> v) & 1ULL)) {
                    is_cover = false;
                    break;
                }
            }
            if (is_cover) {
                g.arcs.push_back(std::make_pair(u + 1, v + 1));
            }
        }
    }
    std::sort(g.arcs.begin(), g.arcs.end());
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-element canonical representative
 * @param k Current number of elements
 * @param out Out-adjacency bitmasks of the current strict-order closure
 * @param n Target number of elements
 * @param results Storage for results
 *
 * Extends by a new element w whose successor set s_out runs over the
 * up-sets of the current poset (subsets closed under taking successors)
 * and whose predecessor set s_in runs over the down-sets disjoint from
 * s_out whose every member lies below every s_out member; exactly those
 * pairs keep the digraph a transitively closed DAG, so every candidate
 * child is a poset closure by construction. A child survives when the
 * new element lies in its canonical-deletion orbit and its canonical
 * form was not already produced by a sibling; correctness of accepting
 * one parent per class is McKay's canonical construction path argument.
 */
inline void poset_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& out, int n,
    std::vector<PosetUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        results.push_back(poset_unlabeled_build_hasse(k, out));
        return;
    }
    std::vector<unsigned long long> in(static_cast<size_t>(k), 0);
    for (int u = 0; u < k; ++u) {
        for (int v = 0; v < k; ++v) {
            if ((out[u] >> v) & 1ULL) in[v] |= 1ULL << u;
        }
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long full = (1ULL << k) - 1;
    for (unsigned long long s_out = 0; s_out <= full; ++s_out) {
        // Up-set check: successors of s_out members stay inside s_out.
        bool up_closed = true;
        for (int v = 0; v < k && up_closed; ++v) {
            if (((s_out >> v) & 1ULL) && (out[v] & ~s_out) != 0) {
                up_closed = false;
            }
        }
        if (!up_closed) continue;
        const unsigned long long comp = full & ~s_out;
        for (unsigned long long s_in = comp;; s_in = (s_in - 1) & comp) {
            // Down-set check plus the s_in-below-s_out cross relations.
            bool valid = true;
            for (int u = 0; u < k && valid; ++u) {
                if (!((s_in >> u) & 1ULL)) continue;
                if ((in[u] & ~s_in) != 0 || (s_out & ~out[u]) != 0) {
                    valid = false;
                }
            }
            if (valid) {
                out.push_back(s_out);
                for (int u = 0; u < k; ++u) {
                    if ((s_in >> u) & 1ULL) out[u] |= 1ULL << k;
                }

                CanonicalAugmentationCanon canon =
                    canonicalize_bitmask_digraph(k + 1, out);
                if (((canon.last_orbit >> k) & 1ULL) &&
                    child_forms.insert(canon.form).second) {
                    poset_unlabeled_enum_dfs(k + 1, out, n, results);
                }

                for (int u = 0; u < k; ++u) {
                    out[u] &= ~(1ULL << k);
                }
                out.pop_back();
            }
            if (s_in == 0) break;
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic posets on n elements
 * @param n Number of elements (must be <= 33, the directed canonical
 *        form's width bound)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return PosetUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class, as Hasse diagrams:
 * A000112(n) posets (1, 2, 5, 16, 63, 318, 2045, 16999, ... for
 * n = 1, 2, ...). n = 0 yields the single empty poset, matching the
 * labeled enumerator and A000112(0) = 1; negative n and n > 33 yield
 * nothing.
 *
 * @note Every accepted k-element representative spawns at most 3^k
 *       candidate children (the closure constraints prune far below
 *       that), each costing one exact canonicalization, so the
 *       enumeration is practical to about n = 9 (183231 classes,
 *       ~33 s); n = 10 (2567284 classes) is on the order of ten
 *       minutes.
 */
inline PosetUnlabeledEnumerationResult
enumerate_poset_unlabeled_graphs(
    int n,
    PosetUnlabeledEnumAlgorithm algo =
        PosetUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    PosetUnlabeledEnumerationResult result;
    if (n < 0 || n > 33) return result;
    if (n == 0) {
        PosetUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> out(1, 0);
    detail::poset_unlabeled_enum_dfs(1, out, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
