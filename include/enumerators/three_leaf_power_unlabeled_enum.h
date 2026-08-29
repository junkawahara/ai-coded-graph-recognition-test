#ifndef GRAPH_RECOGNITION_THREE_LEAF_POWER_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_THREE_LEAF_POWER_UNLABELED_ENUM_H

/**
 * @file three_leaf_power_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic 3-leaf power graphs
 *
 * Enumerates one representative per isomorphism class of 3-leaf power graphs
 * on n vertices from the vertex-incremental characterization of the class,
 * with no recognizer call anywhere in the search.
 *
 * A connected graph is a 3-leaf power iff its critical clique graph is a
 * tree whose adjacent cliques are joined completely (Brandstaedt and Le
 * 2006) -- equivalently, iff it is obtained from a tree by substituting a
 * clique for every vertex. That makes two of the three Bandelt--Mulder
 * distance-hereditary extensions generate the class:
 *   - a true twin of v grows v's critical clique by one vertex and leaves
 *     the critical clique tree unchanged, so it always stays in the class;
 *   - a pendant vertex at v stays in the class iff v's critical clique is
 *     a singleton (the new vertex becomes a leaf clique below it) or the
 *     graph is complete (one clique splits into a path of three). If v has
 *     a true twin w and the graph is not complete, the extension splits
 *     {v, w, ...} into {v} and the rest, both still joined to each other
 *     and to a former neighbor clique -- a triangle in the critical clique
 *     graph (concretely, a bull or dart through the new vertex).
 * The false-twin extension of the distance-hereditary and Ptolemaic
 * searches is dropped entirely: it is valid only at a simplicial vertex
 * with no true twin (or in a complete graph), and the completeness
 * argument below shows every member is already reached without it.
 *
 * Completeness: a connected 3-leaf power on k >= 2 vertices with a
 * non-singleton critical clique is the true-twin extension of the member
 * obtained by deleting one of the twins. If instead every critical clique
 * is a singleton, the graph *is* its critical clique tree -- a tree -- and
 * deleting a leaf shows it is the pendant extension of a smaller tree, at
 * a vertex that has no true twin there (trees on >= 3 vertices have none;
 * K1 and K2 are complete, so the pendant rule admits them too). Both
 * deletions keep the graph connected, so the connected members on k
 * vertices are exactly the restricted extensions of the connected members
 * on k - 1 vertices, and the search generates level by level from K1.
 *
 * Isomorph rejection is a canonical-form set per level
 * (`util/canonical_augmentation.h`), for the reason spelled out in
 * `distance_hereditary_unlabeled_enum.h`: the children are restricted, so
 * the canonically last vertex need not be prunable and the canonical-parent
 * test of the geng-style enumerators would reject entire classes.
 *
 * Disconnected members are composed from the connected ones: the class is
 * closed under disjoint union and a graph is a 3-leaf power iff each of its
 * components is (the critical clique forest is the union of the components'
 * trees), so the graphs on n vertices are the multisets of connected ones
 * with sizes summing to n. Component sizes run over the integer partitions
 * of n, and equal-size parts take non-decreasing representative indices so
 * each multiset is built once.
 *
 * Number of connected non-isomorphic 3-leaf powers on n vertices =
 * OEIS A277863(n): 1, 1, 2, 5, 12, 32, 82, 227, 629, ...
 * Counting every member (the Euler transform of A277863): 1, 2, 4, 10, 24,
 * 65, 171, 478, 1341, ... for n = 1, 2, ...
 *
 * References:
 *   Brandstaedt, Le, "Structure and linear time recognition of 3-leaf
 *   powers," Inform. Process. Lett. 98(4):133--138, 2006 (critical clique
 *   tree characterization);
 *   Bandelt, Mulder, "Distance-hereditary graphs," J. Combin. Theory Ser. B
 *   41(2):182--208, 1986 (the one-vertex extensions);
 *   Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 (O(n^3)-delay
 *   non-isomorphic enumeration from the same characterization);
 *   OEIS A277863
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic 3-leaf power enumeration
 */
enum class ThreeLeafPowerUnlabeledEnumAlgorithm {
    VERTEX_INCREMENTAL /**< True-twin/restricted-pendant extensions + isomorph rejection */
};

/**
 * @brief An enumerated 3-leaf power graph
 */
struct ThreeLeafPowerUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic 3-leaf power enumeration
 */
struct ThreeLeafPowerUnlabeledEnumerationResult {
    std::vector<ThreeLeafPowerUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief One connected representative: adjacency bitmasks over 0, ..., k-1 */
typedef std::vector<unsigned long long> ThreeLeafPowerUnlabeledBitmaskGraph;

/**
 * @brief Connected representatives on 1, ..., n vertices, one per isomorphism class
 * @param n Largest number of vertices to generate (n >= 1, n < 64)
 * @return `levels[k]` holds the k-vertex connected representatives (`levels[0]` is empty)
 *
 * Level k is generated from level k - 1 by the true-twin extension at every
 * vertex plus the pendant extension at every vertex without a true twin
 * (every vertex when the parent is complete), deduplicated by canonical
 * form. Both extensions attach the new vertex to an existing one, so
 * connectivity is preserved with no further test.
 */
inline std::vector<std::vector<ThreeLeafPowerUnlabeledBitmaskGraph> >
three_leaf_power_unlabeled_connected_levels(int n) {
    std::vector<std::vector<ThreeLeafPowerUnlabeledBitmaskGraph> > levels(n + 1);
    levels[1].push_back(ThreeLeafPowerUnlabeledBitmaskGraph(1, 0ULL));

    for (int k = 2; k <= n; ++k) {
        std::set<std::vector<unsigned long long> > seen;
        const std::vector<ThreeLeafPowerUnlabeledBitmaskGraph>& parents =
            levels[k - 1];
        std::vector<unsigned long long> closed(k - 1);
        for (std::size_t i = 0; i < parents.size(); ++i) {
            const unsigned long long full = (1ULL << (k - 1)) - 1ULL;
            bool complete = true;
            for (int v = 0; v + 1 < k; ++v) {
                closed[v] = parents[i][v] | (1ULL << v);
                if (closed[v] != full) complete = false;
            }

            ThreeLeafPowerUnlabeledBitmaskGraph adj = parents[i];
            adj.push_back(0ULL);
            for (int v = 0; v + 1 < k; ++v) {
                const unsigned long long nbr = parents[i][v];
                // Two vertices share a closed neighborhood iff they are
                // true twins (equality forces the adjacency).
                bool has_true_twin = false;
                if (!complete) {
                    for (int u = 0; u + 1 < k; ++u) {
                        if (u != v && closed[u] == closed[v]) {
                            has_true_twin = true;
                            break;
                        }
                    }
                }
                const unsigned long long masks[2] = {
                    nbr | (1ULL << v),     // true twin of v
                    1ULL << v              // pendant vertex at v
                };
                for (int t = 0; t < 2; ++t) {
                    // A pendant at a vertex with a true twin splits its
                    // critical clique into two cliques that keep a common
                    // neighbor clique: a triangle in the critical clique
                    // graph (complete parents have no neighbor clique).
                    if (t == 1 && has_true_twin && !complete) continue;
                    const unsigned long long mask = masks[t];
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
inline void three_leaf_power_unlabeled_combine(
    const std::vector<int>& parts, std::size_t idx, std::size_t prev_index,
    int vertex_offset, std::vector<std::pair<int, int> >& current_edges,
    const std::vector<std::vector<ThreeLeafPowerUnlabeledBitmaskGraph> >& connected,
    int n, std::vector<ThreeLeafPowerUnlabeledEnumeratedGraph>& results) {
    if (idx == parts.size()) {
        ThreeLeafPowerUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    const int size = parts[idx];
    const std::vector<ThreeLeafPowerUnlabeledBitmaskGraph>& reps = connected[size];
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

        three_leaf_power_unlabeled_combine(parts, idx + 1, i,
                                           vertex_offset + size, current_edges,
                                           connected, n, results);

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
inline void three_leaf_power_unlabeled_partition_dfs(
    int remaining, int max_part, std::vector<int>& parts,
    const std::vector<std::vector<ThreeLeafPowerUnlabeledBitmaskGraph> >& connected,
    int n, std::vector<ThreeLeafPowerUnlabeledEnumeratedGraph>& results) {
    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        three_leaf_power_unlabeled_combine(parts, 0, 0, 0, current_edges,
                                           connected, n, results);
        return;
    }
    const int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        three_leaf_power_unlabeled_partition_dfs(remaining - s, s, parts,
                                                 connected, n, results);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic 3-leaf power graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: VERTEX_INCREMENTAL)
 * @return ThreeLeafPowerUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A277863(n) graphs
 * (1, 1, 2, 5, 12, 32, 82, 227, 629, ... for n = 1, 2, ...) with
 * @p connected_only, and the Euler transform of that sequence
 * (1, 2, 4, 10, 24, 65, 171, 478, 1341, ...) without it. n = 0 yields the
 * single empty graph in both modes; negative n and n >= 64 yield nothing.
 *
 * @note The extensions themselves are cheap, but the isomorph rejection uses
 *       the shared canonicalization, an exact branch-and-bound over vertex
 *       orderings whose worst case is k! on highly symmetric graphs (this
 *       class contains K_k and K_{1,k-1}); that bounds the enumeration to
 *       about n = 11 (n = 9 takes well under a second, n = 10 about
 *       fifteen seconds, n = 11 about seven minutes).
 */
inline ThreeLeafPowerUnlabeledEnumerationResult
enumerate_three_leaf_power_unlabeled_graphs(
    int n, bool connected_only = false,
    ThreeLeafPowerUnlabeledEnumAlgorithm algo =
        ThreeLeafPowerUnlabeledEnumAlgorithm::VERTEX_INCREMENTAL) {
    (void)algo;
    ThreeLeafPowerUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        // check_three_leaf_power accepts n = 0; emit the empty graph.
        ThreeLeafPowerUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<std::vector<detail::ThreeLeafPowerUnlabeledBitmaskGraph> >
        connected = detail::three_leaf_power_unlabeled_connected_levels(n);

    if (connected_only) {
        for (std::size_t i = 0; i < connected[n].size(); ++i) {
            ThreeLeafPowerUnlabeledEnumeratedGraph g;
            g.n = n;
            g.edges = bitmask_graph_edges(n, connected[n][i]);
            result.graphs.push_back(g);
        }
        return result;
    }

    std::vector<int> parts;
    detail::three_leaf_power_unlabeled_partition_dfs(n, n, parts, connected, n,
                                                     result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
