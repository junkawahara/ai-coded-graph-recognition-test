#ifndef GRAPH_RECOGNITION_THREE_LEAF_POWER_ENUM_H
#define GRAPH_RECOGNITION_THREE_LEAF_POWER_ENUM_H

/**
 * @file three_leaf_power_labeled_enum.h
 * @brief Labeled 3-leaf power enumeration by critical-clique-preserving extensions
 *
 * Enumerates all labeled 3-leaf power graphs on vertex set {1, ..., n} by
 * expanding the representatives from the structural unlabeled enumerator.
 * Connected representatives use true-twin additions and pendant additions
 * at vertices without a true twin (or at any vertex of a complete graph).
 * These operations keep the critical clique graph a tree. Canonical-form sets
 * remove isomorphic duplicates, including critical clique tree symmetries.
 * Disconnected representatives are composed as multisets of connected
 * components. Adjacent label swaps generate all n!/|Aut(G)| distinct
 * labelings, deduplicated by edge set, without recognition calls.
 *
 * A removable pendant/twin need not have the largest label, so fixing the
 * extension order to increasing labels would omit valid labeled graphs.
 *
 * REVERSE_SEARCH retains the previous chordal search with 3-leaf power
 * recognition. Its parent removes the simplicial vertex with largest label.
 *
 * References: Brandstaedt and Le, "Structure and linear-time recognition
 * of 3-leaf powers," 2006; Yamazaki, Qian and Uehara, "Efficient enumeration
 * of non-isomorphic distance-hereditary graphs and related graphs," 2024,
 * DOI:10.1016/j.dam.2023.09.002. The specialized canonical-parent search and
 * the paper's delay guarantee are not implemented here.
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/three_leaf_power_unlabeled_enum.h"
#include "util/graph.h"
#include "recognizers/three_leaf_power.h"
#include "util/labeled_expansion.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for labeled 3-leaf power enumeration
 */
enum class ThreeLeafPowerLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous chordal search with 3-leaf power recognition */
    VERTEX_INCREMENTAL = 1 /**< True-twin/restricted-pendant extensions + label expansion (default) */
};

/**
 * @brief Result of 3-leaf power graph enumeration
 */
struct ThreeLeafPowerLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated 3-leaf power graphs */
};

namespace detail {

/**
 * @brief Build a Graph from ChordalLabeledEnumState
 */
inline Graph tlp_state_to_graph(const ChordalLabeledEnumState& state) {
    // Remap alive vertices to [1..alive_count] to avoid dead vertex overhead
    std::vector<int> remap(state.total_n + 1, 0);
    int cnt = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) remap[v] = ++cnt;
    }
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= state.total_n; ++u) {
        if (!state.alive[u]) continue;
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (!state.alive[v]) continue;
            if (state.adj[u][v]) {
                edges.push_back(std::make_pair(remap[u], remap[v]));
            }
        }
    }
    return Graph(cnt, edges);
}

/**
 * @brief DFS for 3-leaf power reverse search
 *
 * Same structure as chordal reverse search, but verifies 3-leaf power property
 * at each node and prunes non-3-leaf-power subtrees.
 */
inline void three_leaf_power_reverse_search_dfs(const ChordalLabeledEnumState& state,
                                                 std::vector<EnumeratedGraph>* out) {
    // All vertices are alive -> completed graph
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    // Generate children (same as chordal reverse search)
    std::vector<ChordalLabeledEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        // 3-leaf power check: prune if child is not a 3-leaf power
        Graph g = tlp_state_to_graph(children[i]);
        ThreeLeafPowerResult tr = check_three_leaf_power(g);
        if (!tr.is_three_leaf_power) continue;

        three_leaf_power_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled 3-leaf power graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (default: VERTEX_INCREMENTAL)
 * @return ThreeLeafPowerLabeledEnumerationResult
 *
 * VERTEX_INCREMENTAL reuses the structural unlabeled generator, including
 * disconnected graphs, and expands each representative's distinct labelings.
 * It retains canonical-form sets and connected representatives by size, plus
 * the visited labelings of the current representative. All output graphs are
 * materialized in memory. REVERSE_SEARCH preserves the previous algorithm
 * and its output order. The algorithms can enumerate in different orders.
 * n = 0 yields one empty graph. The historical function name is retained
 * for source compatibility.
 * @throws std::invalid_argument if n < 0, n > 63 for VERTEX_INCREMENTAL
 *         (the unlabeled generator's bitmask limit), n > max_vertex_count
 *         for REVERSE_SEARCH (see util/vertex_count.h), or algo is unknown
 */
inline ThreeLeafPowerLabeledEnumerationResult enumerate_three_leaf_power_labeled_graphs_reverse_search(int n,
    ThreeLeafPowerLabeledEnumAlgorithm algo = ThreeLeafPowerLabeledEnumAlgorithm::VERTEX_INCREMENTAL) {
    ThreeLeafPowerLabeledEnumerationResult result;
    require_vertex_count("enumerate_three_leaf_power_labeled_graphs_reverse_search", n);
    switch (algo) {
        case ThreeLeafPowerLabeledEnumAlgorithm::VERTEX_INCREMENTAL: {
            require_vertex_count("enumerate_three_leaf_power_labeled_graphs_reverse_search", n, 63);
            const ThreeLeafPowerUnlabeledEnumerationResult representatives =
                enumerate_three_leaf_power_unlabeled_graphs(n);
            for (std::size_t i = 0; i < representatives.graphs.size(); ++i) {
                detail::expand_graph_labelings(n, representatives.graphs[i].edges,
                                              &result.graphs);
            }
            break;
        }
        case ThreeLeafPowerLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::ChordalLabeledEnumState root(n);
            detail::three_leaf_power_reverse_search_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_three_leaf_power_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
