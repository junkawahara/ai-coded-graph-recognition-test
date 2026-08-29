#ifndef GRAPH_RECOGNITION_PTOLEMAIC_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_PTOLEMAIC_UNLABELED_ENUM_H

/**
 * @file ptolemaic_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic Ptolemaic graphs
 *
 * Enumerates one representative per isomorphism class of Ptolemaic graphs on
 * n vertices from the vertex-incremental characterization of the class, with
 * no recognizer call anywhere in the search.
 *
 * Ptolemaic = chordal + distance-hereditary (Howorka 1981), so the three
 * one-vertex extensions of Bandelt and Mulder (1986) that generate the
 * distance-hereditary graphs -- attach a pendant vertex to v, add a true twin
 * of v (neighborhood N(v) + {v}), add a false twin of v (neighborhood N(v))
 * -- generate this class too once they are restricted to the ones that also
 * preserve chordality:
 *   - a pendant vertex is simplicial, so it never closes a hole;
 *   - a true twin substitutes v by a clique of size two, and chordal graphs
 *     are closed under that substitution;
 *   - a false twin u of v closes the induced 4-cycle u-a-v-b-u for any two
 *     non-adjacent a, b in N(v), and adds a second simplicial vertex when
 *     N(v) is a clique. So it preserves chordality **iff v is simplicial**,
 *     and that is the only restriction this class puts on the search.
 * Conversely, a Ptolemaic graph is distance-hereditary, so it has a pendant
 * vertex or a twin; deleting one leaves a Ptolemaic graph (the class is
 * hereditary), and if the deleted vertex was a false twin of v then N(v) was
 * already a clique -- otherwise the deleted vertex and two non-adjacent
 * neighbors of v would form an induced C4. So the connected members on k
 * vertices are exactly the restricted extensions of the connected members on
 * k - 1 vertices (the connectivity argument is the one of
 * `distance_hereditary_unlabeled_enum.h`: only the false twin of an isolated
 * vertex disconnects, which is K1 alone), and the search generates level by
 * level from K1.
 *
 * Isomorph rejection is a canonical-form set per level
 * (`util/canonical_augmentation.h`), for the reason spelled out in
 * `distance_hereditary_unlabeled_enum.h`: the children are restricted, so the
 * canonically last vertex need not be prunable and the canonical-parent test
 * of the geng-style enumerators would reject entire classes.
 *
 * Disconnected members are composed from the connected ones: the class is
 * closed under disjoint union and a graph is Ptolemaic iff each of its
 * components is, so the graphs on n vertices are the multisets of connected
 * ones with sizes summing to n. Component sizes run over the integer
 * partitions of n, and equal-size parts take non-decreasing representative
 * indices so each multiset is built once.
 *
 * Number of connected non-isomorphic Ptolemaic graphs on n vertices =
 * OEIS A287888(n): 1, 1, 2, 5, 14, 47, 170, 676, 2834, ...
 * Counting every member (the Euler transform of A287888): 1, 2, 4, 10, 26,
 * 82, 278, 1053, 4251, ... for n = 1, 2, ...
 *
 * References:
 *   Howorka, "A characterization of Ptolemaic graphs," J. Graph Theory
 *   5(3):323--331, 1981 (chordal + distance-hereditary);
 *   Bandelt, Mulder, "Distance-hereditary graphs," J. Combin. Theory Ser. B
 *   41(2):182--208, 1986 (the one-vertex extensions);
 *   Nakano, Uno, WALCOM 2020 / ISAAC 2020 and Yamazaki, Qian, Uehara,
 *   Discrete Appl. Math. 342, 2024 (O(n^3)-delay non-isomorphic enumeration
 *   from the same characterization); OEIS A287888
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic Ptolemaic enumeration
 */
enum class PtolemaicUnlabeledEnumAlgorithm {
    VERTEX_INCREMENTAL /**< Pendant/true-twin/simplicial-false-twin extensions + isomorph rejection */
};

/**
 * @brief An enumerated Ptolemaic graph
 */
struct PtolemaicUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic Ptolemaic enumeration
 */
struct PtolemaicUnlabeledEnumerationResult {
    std::vector<PtolemaicUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief One connected representative: adjacency bitmasks over 0, ..., k-1 */
typedef std::vector<unsigned long long> PtolemaicUnlabeledBitmaskGraph;

/**
 * @brief Tests whether the neighborhood of a vertex is a clique
 * @param adj Adjacency bitmasks over 0, ..., k-1
 * @param k Number of vertices of @p adj
 * @param nbr Neighborhood mask of the vertex
 * @return true if every pair of vertices in @p nbr is adjacent
 *
 * The false-twin extension preserves chordality exactly at such a vertex.
 */
inline bool ptolemaic_unlabeled_is_simplicial(
    const PtolemaicUnlabeledBitmaskGraph& adj, int k, unsigned long long nbr) {
    for (int u = 0; u < k; ++u) {
        if (!((nbr >> u) & 1ULL)) continue;
        if ((adj[u] & nbr) != (nbr & ~(1ULL << u))) return false;
    }
    return true;
}

/**
 * @brief Connected representatives on 1, ..., n vertices, one per isomorphism class
 * @param n Largest number of vertices to generate (n >= 1, n < 64)
 * @return `levels[k]` holds the k-vertex connected representatives (`levels[0]` is empty)
 *
 * Level k is generated from level k - 1 by the pendant and true-twin
 * extensions at every vertex plus the false-twin extension at every
 * simplicial vertex, deduplicated by canonical form. The false twin of an
 * isolated vertex is the only extension that would disconnect the graph (it
 * applies just to K1) and is skipped by the empty-neighborhood test.
 */
inline std::vector<std::vector<PtolemaicUnlabeledBitmaskGraph> >
ptolemaic_unlabeled_connected_levels(int n) {
    std::vector<std::vector<PtolemaicUnlabeledBitmaskGraph> > levels(n + 1);
    levels[1].push_back(PtolemaicUnlabeledBitmaskGraph(1, 0ULL));

    for (int k = 2; k <= n; ++k) {
        std::set<std::vector<unsigned long long> > seen;
        const std::vector<PtolemaicUnlabeledBitmaskGraph>& parents =
            levels[k - 1];
        for (std::size_t i = 0; i < parents.size(); ++i) {
            PtolemaicUnlabeledBitmaskGraph adj = parents[i];
            adj.push_back(0ULL);
            for (int v = 0; v + 1 < k; ++v) {
                const unsigned long long nbr = parents[i][v];
                const unsigned long long masks[3] = {
                    1ULL << v,             // pendant vertex at v
                    nbr | (1ULL << v),     // true twin of v
                    nbr                    // false twin of v (v simplicial only)
                };
                for (int t = 0; t < 3; ++t) {
                    const unsigned long long mask = masks[t];
                    if (mask == 0) continue;  // false twin of an isolated vertex
                    // A false twin of a non-simplicial v closes an induced C4.
                    if (t == 2 && !ptolemaic_unlabeled_is_simplicial(
                                      parents[i], k - 1, nbr)) {
                        continue;
                    }
                    adj[k - 1] = mask;
                    for (int u = 0; u + 1 < k; ++u) {
                        if ((mask >> u) & 1ULL) adj[u] |= 1ULL << (k - 1);
                    }

                    CanonicalAugmentationCanon canon =
                        canonicalize_bitmask_graph(k, adj);
                    if (seen.insert(canon.form).second) levels[k].push_back(adj);

                    for (int u = 0; u + 1 < k; ++u) {
                        adj[u] &= ~(1ULL << (k - 1));
                    }
                }
            }
        }
    }
    return levels;
}

/**
 * @brief Chooses a connected representative for each component size
 * @param parts Component sizes (non-increasing)
 * @param idx Part currently being filled
 * @param prev_index Representative index used for the previous part of equal size
 * @param vertex_offset 0-based label offset of the current component
 * @param current_edges Edge list under construction (1-indexed)
 * @param connected Connected representatives by size
 * @param n Total number of vertices
 * @param results Storage for results
 *
 * Equal-size parts take non-decreasing representative indices, which is what
 * makes each multiset of components appear exactly once.
 */
inline void ptolemaic_unlabeled_combine(
    const std::vector<int>& parts, std::size_t idx, std::size_t prev_index,
    int vertex_offset, std::vector<std::pair<int, int> >& current_edges,
    const std::vector<std::vector<PtolemaicUnlabeledBitmaskGraph> >& connected,
    int n, std::vector<PtolemaicUnlabeledEnumeratedGraph>& results) {
    if (idx == parts.size()) {
        PtolemaicUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    const int size = parts[idx];
    const std::vector<PtolemaicUnlabeledBitmaskGraph>& reps = connected[size];
    std::size_t start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) start = prev_index;

    for (std::size_t i = start; i < reps.size(); ++i) {
        const std::size_t old_size = current_edges.size();
        std::vector<std::pair<int, int> > component_edges =
            bitmask_graph_edges(size, reps[i]);
        for (std::size_t e = 0; e < component_edges.size(); ++e) {
            current_edges.push_back(
                std::make_pair(component_edges[e].first + vertex_offset,
                               component_edges[e].second + vertex_offset));
        }

        ptolemaic_unlabeled_combine(parts, idx + 1, i, vertex_offset + size,
                                    current_edges, connected, n, results);

        current_edges.resize(old_size);
    }
}

/**
 * @brief Enumerates the integer partitions of n and builds a graph for each choice
 * @param remaining Vertices still to cover
 * @param max_part Largest part allowed next (parts stay non-increasing)
 * @param parts Parts chosen so far
 * @param connected Connected representatives by size
 * @param n Total number of vertices
 * @param results Storage for results
 */
inline void ptolemaic_unlabeled_partition_dfs(
    int remaining, int max_part, std::vector<int>& parts,
    const std::vector<std::vector<PtolemaicUnlabeledBitmaskGraph> >& connected,
    int n, std::vector<PtolemaicUnlabeledEnumeratedGraph>& results) {
    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        ptolemaic_unlabeled_combine(parts, 0, 0, 0, current_edges, connected, n,
                                    results);
        return;
    }
    const int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        ptolemaic_unlabeled_partition_dfs(remaining - s, s, parts, connected, n,
                                          results);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic Ptolemaic graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: VERTEX_INCREMENTAL)
 * @return PtolemaicUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A287888(n) graphs
 * (1, 1, 2, 5, 14, 47, 170, 676, 2834, ... for n = 1, 2, ...) with
 * @p connected_only, and the Euler transform of that sequence
 * (1, 2, 4, 10, 26, 82, 278, 1053, 4251, ...) without it. n = 0 yields the
 * single empty graph in both modes; negative n and n >= 64 yield nothing.
 *
 * @note The extensions themselves are cheap, but the isomorph rejection uses
 *       the shared canonicalization, an exact branch-and-bound over vertex
 *       orderings whose worst case is k! on highly symmetric graphs (this
 *       class contains K_k and K_{1,k-1}); that bounds the enumeration to
 *       about n = 10 (n = 8 takes about a third of a second, n = 9 about
 *       nine seconds, n = 10 about two and a half minutes).
 */
inline PtolemaicUnlabeledEnumerationResult enumerate_ptolemaic_unlabeled_graphs(
    int n, bool connected_only = false,
    PtolemaicUnlabeledEnumAlgorithm algo =
        PtolemaicUnlabeledEnumAlgorithm::VERTEX_INCREMENTAL) {
    (void)algo;
    PtolemaicUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        // check_ptolemaic accepts n = 0; emit the empty graph.
        PtolemaicUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<std::vector<detail::PtolemaicUnlabeledBitmaskGraph> > connected =
        detail::ptolemaic_unlabeled_connected_levels(n);

    if (connected_only) {
        for (std::size_t i = 0; i < connected[n].size(); ++i) {
            PtolemaicUnlabeledEnumeratedGraph g;
            g.n = n;
            g.edges = bitmask_graph_edges(n, connected[n][i]);
            result.graphs.push_back(g);
        }
        return result;
    }

    std::vector<int> parts;
    detail::ptolemaic_unlabeled_partition_dfs(n, n, parts, connected, n,
                                              result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
