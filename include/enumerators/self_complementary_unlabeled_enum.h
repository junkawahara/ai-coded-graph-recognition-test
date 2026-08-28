#ifndef GRAPH_RECOGNITION_SELF_COMPLEMENTARY_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_SELF_COMPLEMENTARY_UNLABELED_ENUM_H

/**
 * @file self_complementary_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic self-complementary graphs
 *
 * Enumerates one representative per isomorphism class of self-complementary
 * graphs on n vertices by the complementing-permutation construction with
 * isomorph rejection.
 *
 * A complementing permutation (antimorphism) of G is a permutation sigma
 * with sigma(E(G)) = E(complement(G)). Every self-complementary graph has
 * one of 2-power order (raise any antimorphism to its odd part; an odd
 * power of an antimorphism is again an antimorphism), and the cycle lengths
 * of such a sigma are powers of two: a cycle of odd length L > 1 would make
 * the pair orbit of {u, sigma(u)} have odd size L, and a cycle of length
 * L = 2t with t odd would do the same for {u, sigma^t(u)}, whereas along a
 * pair orbit edges and non-edges must alternate, so every pair orbit has
 * even size. The same argument on a pair of fixed points leaves at most one
 * fixed point. Hence the cycle type of sigma is a multiset of powers of two
 * >= 4 plus one fixed point when n = 1 (mod 4) — the same characterization
 * the labeled enumerator uses.
 *
 * The labeled enumerator runs over every permutation of every valid cycle
 * type; here one representative per cycle type is enough. Permutations of
 * the same cycle type are conjugate, and the graphs complemented by
 * tau sigma tau^-1 are exactly the tau-images of those complemented by
 * sigma, so a single sigma per cycle type already meets every isomorphism
 * class the type can produce.
 *
 * For a fixed sigma the graphs it complements are read off the orbits of
 * sigma on vertex pairs: membership alternates along an orbit, every orbit
 * has even size, and so each orbit admits exactly the two alternating
 * assignments, giving 2^r graphs from r orbits. Flipping every orbit
 * complements the graph, which sigma maps back to it, so the choice of the
 * first orbit can be fixed and only 2^(r-1) graphs need to be built.
 *
 * Isomorph rejection is a global canonical-form set over all cycle types
 * (`util/canonical_augmentation.h`): a graph may have antimorphisms of
 * several cycle types, and one cycle type may produce a class more than
 * once, so the canonical form of every candidate is what decides emission.
 *
 * Number of non-isomorphic self-complementary graphs on n vertices =
 * OEIS A000171(n): 1, 0, 0, 1, 2, 0, 0, 10, 36, 0, 0, 720, 5600, ...
 * (zero unless n = 0 or 1 (mod 4)).
 *
 * References:
 *   Sachs, Publ. Math. Debrecen 9, 1962; Ringel, Arch. Math. 14, 1963
 *   (the cycle structure of complementing permutations);
 *   Read, J. London Math. Soc. 38, 1963 (counting);
 *   Farrugia, Ph.D. thesis, Univ. Malta, 1999 (self-complementary
 *   graph generation);
 *   OEIS A000171
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic self-complementary enumeration
 */
enum class SelfComplementaryUnlabeledEnumAlgorithm {
    COMPLEMENTING_PERMUTATION /**< One complementing permutation per cycle type + isomorph rejection */
};

/**
 * @brief An enumerated self-complementary graph
 */
struct SelfComplementaryUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic self-complementary enumeration
 */
struct SelfComplementaryUnlabeledEnumerationResult {
    std::vector<SelfComplementaryUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Collects the multisets of cycle lengths (powers of two >= 4) summing to `remaining`
 * @param remaining Vertices left to cover
 * @param min_len Smallest length allowed next (non-decreasing multisets)
 * @param current Lengths chosen so far (a leading 1 for the fixed point, if any)
 * @param out Storage for the completed cycle types
 */
inline void self_complementary_unlabeled_cycle_types(
    int remaining, int min_len, std::vector<int>& current,
    std::vector<std::vector<int> >& out) {
    if (remaining == 0) {
        out.push_back(current);
        return;
    }
    for (int len = (min_len < 4 ? 4 : min_len); len <= remaining; len *= 2) {
        current.push_back(len);
        self_complementary_unlabeled_cycle_types(remaining - len, len, current,
                                                 out);
        current.pop_back();
    }
}

/**
 * @brief Valid cycle types of a complementing permutation on n vertices
 * @param n Number of vertices
 * @return Each entry is a list of cycle lengths, a fixed point (length 1) first
 *
 * Empty unless n = 0 or 1 (mod 4).
 */
inline std::vector<std::vector<int> >
self_complementary_unlabeled_valid_cycle_types(int n) {
    std::vector<std::vector<int> > result;
    if (n % 4 != 0 && n % 4 != 1) return result;
    std::vector<int> current;
    int remaining = n;
    if (n % 4 == 1) {
        current.push_back(1);
        remaining = n - 1;
    }
    self_complementary_unlabeled_cycle_types(remaining, 4, current, result);
    return result;
}

/**
 * @brief The representative permutation of a cycle type
 * @param n Number of vertices
 * @param cycle_type Cycle lengths summing to n
 * @return sigma over vertices 0, ..., n-1 with `sigma[v]` the image of v
 *
 * Consecutive blocks of vertices form the cycles, in the order given. Any
 * other assignment is conjugate to this one and yields the same isomorphism
 * classes, which is why one representative per cycle type suffices.
 */
inline std::vector<int> self_complementary_unlabeled_permutation(
    int n, const std::vector<int>& cycle_type) {
    std::vector<int> sigma(n, 0);
    int base = 0;
    for (size_t i = 0; i < cycle_type.size(); ++i) {
        int len = cycle_type[i];
        for (int j = 0; j < len; ++j) {
            sigma[base + j] = base + (j + 1) % len;
        }
        base += len;
    }
    return sigma;
}

/**
 * @brief Orbits of sigma on the unordered vertex pairs
 * @param n Number of vertices
 * @param sigma Permutation over 0, ..., n-1
 * @return Each orbit as the cyclic sequence of pairs {u, v} (u < v) it visits
 *
 * Consecutive pairs of an orbit alternate between edges and non-edges of any
 * graph sigma complements, so the traversal order is what the two
 * alternating assignments are read off.
 */
inline std::vector<std::vector<std::pair<int, int> > >
self_complementary_unlabeled_pair_orbits(int n, const std::vector<int>& sigma) {
    std::vector<std::vector<char> > visited(n, std::vector<char>(n, 0));
    std::vector<std::vector<std::pair<int, int> > > orbits;
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (visited[u][v]) continue;
            std::vector<std::pair<int, int> > orbit;
            int a = u, b = v;
            do {
                visited[a][b] = 1;
                orbit.push_back(std::make_pair(a, b));
                int na = sigma[a], nb = sigma[b];
                a = na < nb ? na : nb;
                b = na < nb ? nb : na;
            } while (a != u || b != v);
            orbits.push_back(orbit);
        }
    }
    return orbits;
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic self-complementary graphs on n vertices
 * @param n Number of vertices (must be < 64)
 * @param algo Algorithm to use (default: COMPLEMENTING_PERMUTATION)
 * @return SelfComplementaryUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000171(n) graphs
 * (1, 0, 0, 1, 2, 0, 0, 10, 36, 0, 0, 720, 5600, ... for n = 1, 2, ...).
 * n = 0 yields the single empty graph, matching `check_self_complementary`
 * and the labeled enumerator; n < 0, n >= 64 and every n with
 * n mod 4 in {2, 3} yield nothing.
 *
 * @note Cost is 2^(r-1) exact canonicalizations per cycle type, where r is
 *       the number of pair orbits (about n(n-1)/2 divided by the smallest
 *       cycle length): r = 2, 3, 8/4, 10/5 for n = 4, 5, 8, 9, so those run
 *       in well under a second. The canonicalization is a branch and bound
 *       over vertex orderings and these graphs are highly symmetric, which
 *       is what bounds the range: at n = 12 the cycle type (4,4,4) alone has
 *       r = 18 and its 131072 candidates cost about 0.17 s apiece, some six
 *       hours in total. A cycle type whose orbit count reaches 63 would
 *       overflow the mask space (n >= 24, far beyond the practical range);
 *       the result is then empty rather than partial.
 */
inline SelfComplementaryUnlabeledEnumerationResult
enumerate_self_complementary_unlabeled_graphs(
    int n,
    SelfComplementaryUnlabeledEnumAlgorithm algo =
        SelfComplementaryUnlabeledEnumAlgorithm::COMPLEMENTING_PERMUTATION) {
    (void)algo;
    SelfComplementaryUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n <= 1) {
        SelfComplementaryUnlabeledEnumeratedGraph g;
        g.n = n;
        result.graphs.push_back(g);
        return result;
    }
    if (n % 4 != 0 && n % 4 != 1) return result;

    std::vector<std::vector<int> > cycle_types =
        detail::self_complementary_unlabeled_valid_cycle_types(n);
    std::set<std::vector<unsigned long long> > seen;

    for (size_t t = 0; t < cycle_types.size(); ++t) {
        std::vector<int> sigma =
            detail::self_complementary_unlabeled_permutation(n, cycle_types[t]);
        std::vector<std::vector<std::pair<int, int> > > orbits =
            detail::self_complementary_unlabeled_pair_orbits(n, sigma);
        const int r = (int)orbits.size();
        if (r >= 63) {
            result.graphs.clear();
            return result;
        }

        /* Bit i of `mask` picks the odd-indexed half of orbit i + 1; orbit 0
           always contributes its even-indexed half, since flipping every
           orbit only complements the graph. */
        const unsigned long long limit = 1ULL << (r - 1);
        for (unsigned long long mask = 0; mask < limit; ++mask) {
            std::vector<unsigned long long> adj(n, 0);
            for (int i = 0; i < r; ++i) {
                const std::vector<std::pair<int, int> >& orbit = orbits[i];
                const size_t parity = (i == 0 || !((mask >> (i - 1)) & 1ULL))
                                          ? 0u : 1u;
                for (size_t j = parity; j < orbit.size(); j += 2) {
                    adj[orbit[j].first] |= 1ULL << orbit[j].second;
                    adj[orbit[j].second] |= 1ULL << orbit[j].first;
                }
            }

            CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(n, adj);
            if (!seen.insert(canon.form).second) continue;

            SelfComplementaryUnlabeledEnumeratedGraph g;
            g.n = n;
            g.edges = bitmask_graph_edges(n, adj);
            result.graphs.push_back(g);
        }
    }
    return result;
}

}  // namespace graph_recognition

#endif
