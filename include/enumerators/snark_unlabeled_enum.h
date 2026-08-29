#ifndef GRAPH_RECOGNITION_SNARK_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_SNARK_UNLABELED_ENUM_H

/**
 * @file snark_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic snarks
 *
 * Enumerates one representative per isomorphism class of snarks (cubic,
 * girth >= 5, cyclically 4-edge-connected, chromatic index 4) on n
 * vertices, snarkhunter-style: the degree-constrained McKay canonical
 * construction path of `cubic_unlabeled_enum.h` with the girth
 * constraint folded into the search. Girth >= 5 is hereditary under
 * vertex deletion, so the intermediate levels range over the hereditary
 * class of graphs with maximum degree <= 3 and girth >= 5: a candidate
 * neighborhood of the new vertex may not contain two vertices at
 * distance <= 2 (that would close a cycle of length <= 4), pruned via
 * precomputed radius-2 ball bitmasks. The cubic completability
 * conditions (def(u) <= r, sum def <= 3r, 3r - sum def even) prune as
 * before, so every graph reaching level n is cubic with girth >= 5 by
 * construction; the remaining snark conditions (cyclic
 * 4-edge-connectivity, which subsumes connectivity and bridgelessness,
 * and non-3-edge-colorability) are not monotone along the construction
 * and are checked at emission via `check_snark`.
 *
 * Isomorph rejection is the canonical-parent test of
 * `util/canonical_augmentation.h` at every level, exactly as in the
 * cubic enumerator: the canonical deletion of a graph with girth >= 5
 * and maximum degree <= 3 stays in that class, so pruning never cuts
 * the canonical-deletion chain of a snark.
 *
 * Number of snarks on n vertices = OEIS A130315 (n = 10, 12, 14, ...):
 * 1, 0, 0, 0, 2, 6, 20, 38, 280, ... The Petersen graph is the unique
 * snark on 10 vertices and the smallest; no snark exists for odd n or
 * n < 10 (matching `check_snark`).
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path); Brinkmann, Goedgebeur, Hägglund,
 *   Markström, "Generation and properties of snarks," J. Combin. Theory
 *   Ser. B 103, 2013 (snarkhunter generation, the A130315 counts);
 *   Brinkmann, Goedgebeur, McKay, "Generation of cubic graphs and
 *   snarks with large girth," J. Graph Theory 86, 2017 (canonical
 *   deletion with girth constraints); OEIS A130315
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/snark.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic snark enumeration
 */
enum class SnarkUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path, degree- and girth-constrained */
};

/**
 * @brief An enumerated snark
 */
struct SnarkUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic snark enumeration
 */
struct SnarkUnlabeledEnumerationResult {
    std::vector<SnarkUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

inline void snark_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n,
    std::vector<SnarkUnlabeledEnumeratedGraph>& results);

/**
 * @brief Tests the necessary completability conditions after adding a vertex
 * @param k Number of vertices before the addition
 * @param adj Adjacency bitmasks of the k-vertex graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., k-1
 * @param s_size Population count of s
 * @param n Target number of vertices
 * @return true if the (k+1)-vertex graph may still extend to a cubic graph
 *
 * Identical to the cubic enumerator: with r = n - k - 1 future vertices
 * and def(u) = 3 - deg(u), requires def(u) <= r for every vertex,
 * sum def <= 3r, and 3r - sum def even. All three hold for every induced
 * subgraph of a cubic n-vertex graph, so pruning on them never cuts a
 * canonical-deletion chain.
 */
inline bool snark_unlabeled_enum_feasible(
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
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * The child survives the canonical-parent test when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling (McKay's canonical construction path argument).
 */
inline void snark_unlabeled_enum_extend(
    int k, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<SnarkUnlabeledEnumeratedGraph>& results) {
    adj.push_back(s);
    for (int u = 0; u < k; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
    }

    CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(k + 1, adj);
    if (((canon.last_orbit >> k) & 1ULL) &&
        child_forms.insert(canon.form).second) {
        snark_unlabeled_enum_dfs(k + 1, adj, n, results);
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
 * @param ball2 Per-vertex bitmask of the vertices at distance <= 2
 * @param start Next index into available to consider
 * @param s Neighborhood accumulated so far as a bitmask
 * @param s_size Number of vertices in s
 * @param n Target number of vertices
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * Combination DFS over the at-most-3-subsets of available. Two chosen
 * neighbors u, v would close a cycle of length dist(u, v) + 2 through
 * the new vertex, so girth >= 5 admits only pairwise distance >= 3:
 * a vertex whose radius-2 ball meets s is skipped.
 */
inline void snark_unlabeled_enum_choose(
    int k, std::vector<unsigned long long>& adj,
    const std::vector<int>& available,
    const std::vector<unsigned long long>& ball2, std::size_t start,
    unsigned long long s, int s_size, int n,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<SnarkUnlabeledEnumeratedGraph>& results) {
    if (snark_unlabeled_enum_feasible(k, adj, s, s_size, n)) {
        snark_unlabeled_enum_extend(k, adj, s, n, child_forms, results);
    }
    if (s_size == 3) return;
    for (std::size_t i = start; i < available.size(); ++i) {
        const int v = available[i];
        if ((s & ball2[v]) != 0) continue;
        snark_unlabeled_enum_choose(k, adj, available, ball2, i + 1,
                                    s | (1ULL << v), s_size + 1, n,
                                    child_forms, results);
    }
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param results Storage for results
 *
 * A graph reaching level n is cubic with girth >= 5 by construction. The
 * remaining snark conditions (cyclic 4-edge-connectivity and chromatic
 * index 4) are not preserved along the construction chain, so they only
 * filter at emission, via `check_snark` (which also re-verifies the
 * constructed properties).
 */
inline void snark_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n,
    std::vector<SnarkUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        SnarkUnlabeledEnumeratedGraph cand;
        cand.n = k;
        cand.edges = bitmask_graph_edges(k, adj);
        Graph g(cand.n, cand.edges);
        if (check_snark(g).is_snark) {
            results.push_back(cand);
        }
        return;
    }
    std::vector<int> available;
    std::vector<unsigned long long> ball2(k, 0);
    for (int u = 0; u < k; ++u) {
        int deg = 0;
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
        if (deg < 3) available.push_back(u);
        unsigned long long ball = (1ULL << u) | adj[u];
        for (unsigned long long b = adj[u]; b != 0; b &= b - 1) {
            int w = 0;
            unsigned long long low = b & (~b + 1);
            while ((low >> w) != 1ULL) ++w;
            ball |= adj[w];
        }
        ball2[u] = ball;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    snark_unlabeled_enum_choose(k, adj, available, ball2, 0, 0ULL, 0, n,
                                child_forms, results);
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic snarks on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return SnarkUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A130315 graphs
 * (1, 0, 0, 0, 2, 6, 20, 38, ... for n = 10, 12, 14, ...). Snarks are
 * cyclically 4-edge-connected, hence connected, so there is no
 * connected_only flag. Odd n, n < 10 (the Petersen graph is the
 * smallest snark) and n >= 64 yield nothing.
 *
 * @note The girth constraint shrinks the search far below the cubic
 *       enumerator's (the intermediates range over graphs with maximum
 *       degree <= 3 and girth >= 5), but one exact branch-and-bound
 *       canonicalization per candidate child still bounds the practical
 *       range at about n = 16 (n = 14 takes about four seconds, n = 16
 *       about six minutes; n = 18, the level of the two Blanusa snarks,
 *       extrapolates to hours).
 */
inline SnarkUnlabeledEnumerationResult
enumerate_snark_unlabeled_graphs(
    int n,
    SnarkUnlabeledEnumAlgorithm algo =
        SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    SnarkUnlabeledEnumerationResult result;
    if (n < 10 || n >= 64 || n % 2 != 0) return result;
    std::vector<unsigned long long> adj(1, 0);
    detail::snark_unlabeled_enum_dfs(1, adj, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
