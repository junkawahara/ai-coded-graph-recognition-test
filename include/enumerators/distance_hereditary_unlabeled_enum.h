#ifndef GRAPH_RECOGNITION_DISTANCE_HEREDITARY_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_DISTANCE_HEREDITARY_UNLABELED_ENUM_H

/**
 * @file distance_hereditary_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic distance-hereditary graphs
 *
 * Enumerates one representative per isomorphism class of distance-hereditary
 * graphs on n vertices from the vertex-incremental characterization of the
 * class, with no recognizer call anywhere in the search.
 *
 * Bandelt and Mulder (1986): a graph is distance-hereditary if and only if it
 * can be built from K1 by one-vertex extensions, each of which either
 *   - attaches a pendant vertex to some vertex v, or
 *   - adds a true twin of v (neighborhood N(v) + {v}), or
 *   - adds a false twin of v (neighborhood N(v)),
 * equivalently: every distance-hereditary graph on at least two vertices has
 * a pendant vertex or a twin. Every one of the three extensions keeps the
 * graph distance-hereditary, and on a connected graph with at least two
 * vertices it also keeps it connected (a false twin of v inherits the
 * non-empty N(v)); conversely, deleting a pendant vertex or a twin from a
 * connected graph leaves it connected (a path through a deleted twin of v is
 * rerouted through v). So the connected members on k vertices are exactly the
 * extensions of the connected members on k - 1 vertices, and the search
 * generates level by level from K1: 3(k - 1) children per graph instead of
 * the 2^(k-1) neighborhoods a hereditary-class reverse search would test.
 *
 * Isomorph rejection is a canonical-form set per level
 * (`util/canonical_augmentation.h`): the same class is reached from several
 * parents and several vertices of one parent, so the canonical form of every
 * child decides whether it opens a new class. Unlike McKay's canonical
 * construction path this keeps one level in memory, which the class counts
 * below make affordable; the restricted children are what would otherwise
 * need a canonical *deletion* among prunable vertices rather than the
 * canonically last vertex, which is not what the shared canonicalization
 * reports.
 *
 * Disconnected members are composed from the connected ones: the class is
 * closed under disjoint union and a graph is distance-hereditary iff each of
 * its components is, so the graphs on n vertices are the multisets of
 * connected ones with sizes summing to n. Component sizes run over the
 * integer partitions of n, and equal-size parts take non-decreasing
 * representative indices so each multiset is built once.
 *
 * Number of connected non-isomorphic distance-hereditary graphs on n
 * vertices = OEIS A277862(n): 1, 1, 2, 6, 18, 73, 308, 1484, 7492, ...
 * Counting every member (the Euler transform of A277862): 1, 2, 4, 11, 31,
 * 114, 454, 2078, 10168, ... for n = 1, 2, ...
 *
 * References:
 *   Bandelt, Mulder, "Distance-hereditary graphs," J. Combin. Theory Ser. B
 *   41(2):182--208, 1986 (the one-vertex extensions);
 *   Nakano, Uno, ISAAC 2020 / Discrete Appl. Math. 2023 and Yamazaki, Qian,
 *   Uehara, Discrete Appl. Math. 342, 2024 (O(n^3)-delay non-isomorphic
 *   enumeration from the same characterization);
 *   Chauve, Fusy, Lumbroso, ANALCO 2017 (exact counting); OEIS A277862
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic distance-hereditary enumeration
 */
enum class DistanceHereditaryUnlabeledEnumAlgorithm {
    VERTEX_INCREMENTAL /**< Pendant/true-twin/false-twin extensions + isomorph rejection */
};

/**
 * @brief An enumerated distance-hereditary graph
 */
struct DistanceHereditaryUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic distance-hereditary enumeration
 */
struct DistanceHereditaryUnlabeledEnumerationResult {
    std::vector<DistanceHereditaryUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief One connected representative: adjacency bitmasks over 0, ..., k-1 */
typedef std::vector<unsigned long long> DistanceHereditaryUnlabeledBitmaskGraph;

/**
 * @brief Connected representatives on 1, ..., n vertices, one per isomorphism class
 * @param n Largest number of vertices to generate (n >= 1, n < 64)
 * @return `levels[k]` holds the k-vertex connected representatives (`levels[0]` is empty)
 *
 * Level k is generated from level k - 1 by the three one-vertex extensions at
 * every vertex, deduplicated by canonical form. The false twin of an isolated
 * vertex is the only extension that would disconnect the graph (it applies
 * just to K1) and is skipped by the empty-neighborhood test.
 */
inline std::vector<std::vector<DistanceHereditaryUnlabeledBitmaskGraph> >
distance_hereditary_unlabeled_connected_levels(int n) {
    std::vector<std::vector<DistanceHereditaryUnlabeledBitmaskGraph> > levels(
        n + 1);
    levels[1].push_back(DistanceHereditaryUnlabeledBitmaskGraph(1, 0ULL));

    for (int k = 2; k <= n; ++k) {
        std::set<std::vector<unsigned long long> > seen;
        const std::vector<DistanceHereditaryUnlabeledBitmaskGraph>& parents =
            levels[k - 1];
        for (std::size_t i = 0; i < parents.size(); ++i) {
            DistanceHereditaryUnlabeledBitmaskGraph adj = parents[i];
            adj.push_back(0ULL);
            for (int v = 0; v + 1 < k; ++v) {
                const unsigned long long nbr = parents[i][v];
                const unsigned long long masks[3] = {
                    1ULL << v,             // pendant vertex at v
                    nbr | (1ULL << v),     // true twin of v
                    nbr                    // false twin of v
                };
                for (int t = 0; t < 3; ++t) {
                    const unsigned long long mask = masks[t];
                    if (mask == 0) continue;  // false twin of an isolated vertex
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
inline void distance_hereditary_unlabeled_combine(
    const std::vector<int>& parts, std::size_t idx, std::size_t prev_index,
    int vertex_offset, std::vector<std::pair<int, int> >& current_edges,
    const std::vector<std::vector<DistanceHereditaryUnlabeledBitmaskGraph> >&
        connected,
    int n,
    std::vector<DistanceHereditaryUnlabeledEnumeratedGraph>& results) {
    if (idx == parts.size()) {
        DistanceHereditaryUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    const int size = parts[idx];
    const std::vector<DistanceHereditaryUnlabeledBitmaskGraph>& reps =
        connected[size];
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

        distance_hereditary_unlabeled_combine(parts, idx + 1, i,
                                              vertex_offset + size,
                                              current_edges, connected, n,
                                              results);

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
inline void distance_hereditary_unlabeled_partition_dfs(
    int remaining, int max_part, std::vector<int>& parts,
    const std::vector<std::vector<DistanceHereditaryUnlabeledBitmaskGraph> >&
        connected,
    int n,
    std::vector<DistanceHereditaryUnlabeledEnumeratedGraph>& results) {
    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        distance_hereditary_unlabeled_combine(parts, 0, 0, 0, current_edges,
                                              connected, n, results);
        return;
    }
    const int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        distance_hereditary_unlabeled_partition_dfs(remaining - s, s, parts,
                                                    connected, n, results);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic distance-hereditary graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: VERTEX_INCREMENTAL)
 * @return DistanceHereditaryUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A277862(n) graphs
 * (1, 1, 2, 6, 18, 73, 308, 1484, 7492, ... for n = 1, 2, ...) with
 * @p connected_only, and the Euler transform of that sequence
 * (1, 2, 4, 11, 31, 114, 454, 2078, 10168, ...) without it. n = 0 yields the
 * single empty graph in both modes; negative n and n >= 64 yield nothing.
 *
 * @note The extensions themselves are cheap, but the isomorph rejection uses
 *       the shared canonicalization, an exact branch-and-bound over vertex
 *       orderings whose worst case is k! on highly symmetric graphs (this
 *       class contains K_k and K_{1,k-1}); that bounds the enumeration to
 *       about n = 9 (n = 8 takes about a third of a second, n = 9 about ten
 *       seconds, n = 10 several minutes).
 */
inline DistanceHereditaryUnlabeledEnumerationResult
enumerate_distance_hereditary_unlabeled_graphs(
    int n, bool connected_only = false,
    DistanceHereditaryUnlabeledEnumAlgorithm algo =
        DistanceHereditaryUnlabeledEnumAlgorithm::VERTEX_INCREMENTAL) {
    (void)algo;
    DistanceHereditaryUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        // check_distance_hereditary accepts n = 0; emit the empty graph.
        DistanceHereditaryUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<std::vector<detail::DistanceHereditaryUnlabeledBitmaskGraph> >
        connected = detail::distance_hereditary_unlabeled_connected_levels(n);

    if (connected_only) {
        for (std::size_t i = 0; i < connected[n].size(); ++i) {
            DistanceHereditaryUnlabeledEnumeratedGraph g;
            g.n = n;
            g.edges = bitmask_graph_edges(n, connected[n][i]);
            result.graphs.push_back(g);
        }
        return result;
    }

    std::vector<int> parts;
    detail::distance_hereditary_unlabeled_partition_dfs(n, n, parts, connected,
                                                        n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
