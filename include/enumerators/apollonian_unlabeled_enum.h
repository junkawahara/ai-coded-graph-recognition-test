#ifndef GRAPH_RECOGNITION_APOLLONIAN_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_APOLLONIAN_UNLABELED_ENUM_H

/**
 * @file apollonian_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic Apollonian networks (planar 3-trees)
 *
 * Enumerates one representative per isomorphism class of Apollonian
 * networks on n vertices by the dedicated stacking construction: starting
 * from the triangle K3, every member arises by repeatedly inserting a new
 * vertex joined to the three corners of a face of the current stacked
 * triangulation (recursive triangle subdivision).
 *
 * Child generation exploits the class invariants instead of a recognizer
 * call. Along the construction m = 3k - 6 always holds, and the inserted
 * vertex is simplicial (its neighborhood is a triangle), so every
 * candidate child is chordal with the right edge count and is Apollonian
 * iff it is planar -- one `check_planar` call per candidate triangle
 * (insertion into a face keeps planarity, insertion into a separating
 * triangle never does).
 *
 * Isomorph rejection is McKay's canonical construction path method, with
 * the twist that the class is not hereditary: only *degree-3* vertices
 * are deletable (deleting one merges the three faces around it back into
 * a triangle, giving the Apollonian network one insertion earlier; every
 * Apollonian network on >= 4 vertices has such a vertex -- a simplicial
 * vertex of a maximal planar graph cannot have degree >= 4, which would
 * embed K5). The canonical parent therefore deletes the vertex at the
 * *last canonical position of degree 3*: position degrees are
 * reconstructible from the canonical form, and
 * `canonicalize_bitmask_graph_orbits` (util/canonical_augmentation.h)
 * reports the automorphism orbit of every position, so a child survives
 * iff the newly inserted vertex lies in that position's orbit, with
 * children of the same parent deduplicated by canonical form. What is
 * canonicalized is the *complement* (same automorphism group, same
 * position orbits): the lex-min row search then builds clique-in-G
 * prefixes, which die out after four positions since the class has no K5,
 * where canonicalizing G itself explodes on the ties of its large
 * independent sets (about 100x slower already at n = 11).
 *
 * Number of non-isomorphic Apollonian networks on n >= 4 vertices =
 * OEIS A027610(n - 3) ("Apollonian networks (planar 3-trees) with n+3
 * vertices"): 1, 1, 1, 3, 7, 24, 93, 434, 2110, 11002, 58713, 321776, ...
 * for n = 4, 5, 6, ... K0, K1, K2 and the seed K3 are emitted as the
 * single trivial member for n <= 3. Every member is connected, so there
 * is no `connected_only` flag. The rooted version (a marked outer face)
 * is counted by the generalized Catalan numbers C(3j, j)/(2j + 1)
 * (OEIS A001764); the quotient by the reinsertion symmetries is what the
 * canonical-parent test computes.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Hering, Koehler, Seifert, "The enumeration of stack polytopes and
 *   simplicial clusters," Discrete Math. 40, 1982 (the counting);
 *   Beineke, Pippert, "Enumerating dissectable polyhedra by their
 *   automorphism groups," Canad. J. Math. 26, 1974;
 *   OEIS A027610, A001764
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
 * @brief Algorithm selection for non-isomorphic Apollonian network enumeration
 */
enum class ApollonianUnlabeledEnumAlgorithm {
    FACE_INSERTION /**< Face insertions with degree-3 canonical deletion */
};

/**
 * @brief An enumerated Apollonian network
 */
struct ApollonianUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic Apollonian network enumeration
 */
struct ApollonianUnlabeledEnumerationResult {
    std::vector<ApollonianUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline ApollonianUnlabeledEnumeratedGraph apollonian_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    ApollonianUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices (>= 3)
 * @param adj Adjacency bitmasks of the current Apollonian network
 * @param n Target number of vertices
 * @param results Storage for results
 *
 * Extends by a new vertex joined to each triangle {u, v, w} of the current
 * graph, keeping the children that stay planar (exactly the insertions
 * into faces; chordality and m = 3k - 6 hold by construction). A child
 * survives when the new vertex lies in the automorphism orbit of the last
 * canonical position of degree 3 -- the canonical-deletion orbit of the
 * degree-3-restricted parent rule -- and its canonical form was not
 * already produced by a sibling.
 */
inline void apollonian_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n,
    std::vector<ApollonianUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        results.push_back(apollonian_unlabeled_build_graph(k, adj));
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    for (int u = 0; u < k; ++u) {
        for (int v = u + 1; v < k; ++v) {
            if (!((adj[u] >> v) & 1ULL)) continue;
            unsigned long long common =
                adj[u] & adj[v] & ~((1ULL << (v + 1)) - 1ULL);
            for (int w = v + 1; w < k; ++w) {
                if (!((common >> w) & 1ULL)) continue;

                adj.push_back((1ULL << u) | (1ULL << v) | (1ULL << w));
                adj[u] |= 1ULL << k;
                adj[v] |= 1ULL << k;
                adj[w] |= 1ULL << k;

                Graph child(k + 1, bitmask_graph_edges(k + 1, adj));
                if (check_planar(child).is_planar) {
                    /* Canonicalize the COMPLEMENT: same automorphisms and
                     * position orbits, but the lex-min row search then
                     * prefers G-adjacent prefixes, and G has no K5, so tie
                     * classes die out after four positions -- orders of
                     * magnitude faster than canonicalizing G itself, whose
                     * large independent sets make huge zero-row ties. */
                    const unsigned long long full =
                        (k + 1 == 64) ? ~0ULL : (1ULL << (k + 1)) - 1ULL;
                    std::vector<unsigned long long> comp(k + 1);
                    for (int x = 0; x <= k; ++x) {
                        comp[x] = full & ~adj[x] & ~(1ULL << x);
                    }
                    CanonicalAugmentationCanonOrbits canon =
                        canonicalize_bitmask_graph_orbits(k + 1, comp);

                    /* Position degrees (in G) from the complement form:
                     * deg_G = k - deg_complement */
                    std::vector<int> deg(k + 1, 0);
                    for (int p = 1; p <= k; ++p) {
                        unsigned long long row = canon.form[p - 1];
                        for (int q = 0; q < p; ++q) {
                            if ((row >> q) & 1ULL) {
                                ++deg[p];
                                ++deg[q];
                            }
                        }
                    }
                    int pstar = -1;
                    for (int p = k; p >= 0; --p) {
                        if (k - deg[p] == 3) {
                            pstar = p;
                            break;
                        }
                    }
                    /* pstar >= 0 always: every Apollonian network on >= 4
                     * vertices has a degree-3 vertex */
                    if (pstar >= 0 &&
                        ((canon.position_orbits[pstar] >> k) & 1ULL) &&
                        child_forms.insert(canon.form).second) {
                        apollonian_unlabeled_enum_dfs(k + 1, adj, n, results);
                    }
                }

                adj[u] &= ~(1ULL << k);
                adj[v] &= ~(1ULL << k);
                adj[w] &= ~(1ULL << k);
                adj.pop_back();
            }
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic Apollonian networks on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: FACE_INSERTION)
 * @return ApollonianUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A027610(n - 3) graphs
 * for n >= 4 (1, 1, 1, 3, 7, 24, 93, 434, 2110, ...), and the single
 * trivial member K0 / K1 / K2 / K3 for n <= 3. Negative n yields nothing.
 * All emitted graphs are connected (the class contains no disconnected
 * graph), so there is no `connected_only` flag.
 *
 * @note Every candidate child costs one linear-time planarity check plus,
 *       when planar, one exact canonicalization of its complement (branch
 *       and bound over vertex orderings), so the enumeration is practical
 *       to about n = 13 (11,002 classes; n = 12 about 8 s, n = 13 about
 *       5 min).
 */
inline ApollonianUnlabeledEnumerationResult
enumerate_apollonian_unlabeled_graphs(
    int n,
    ApollonianUnlabeledEnumAlgorithm algo =
        ApollonianUnlabeledEnumAlgorithm::FACE_INSERTION) {
    (void)algo;
    ApollonianUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n <= 3) {
        ApollonianUnlabeledEnumeratedGraph g;
        g.n = n;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                g.edges.push_back(std::make_pair(u, v));
            }
        }
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj;
    adj.push_back((1ULL << 1) | (1ULL << 2));
    adj.push_back((1ULL << 0) | (1ULL << 2));
    adj.push_back((1ULL << 0) | (1ULL << 1));
    detail::apollonian_unlabeled_enum_dfs(3, adj, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
