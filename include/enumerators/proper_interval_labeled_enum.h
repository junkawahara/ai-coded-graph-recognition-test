#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_ENUM_H

/**
 * @file proper_interval_labeled_enum.h
 * @brief Labeled proper interval graph enumeration from bracket strings
 *
 * Enumerates all labeled proper interval graphs on vertex set {1, ..., n}
 * by expanding each representative from the unlabeled bracket-string
 * enumerator into its distinct labelings. The reused generator handles
 * reflection symmetry and disconnected component multisets. Label expansion
 * traverses the orbit under adjacent label swaps, retaining each edge set
 * once, including when twins or equal components create automorphisms.
 *
 * The previous chordal reverse search with proper-interval recognition
 * remains selectable as REVERSE_SEARCH. Its parent removes the simplicial
 * vertex with the largest label.
 *
 * Reference: Saitoh, Yamanaka, Kiyomi and Uehara, "Random Generation and
 * Enumeration of Proper Interval Graphs," IEICE Trans. E93-D(7), 2010.
 * DOI: 10.1587/transinf.E93.D.1816
 * The bracket generator uses Dyck-word DFS, not the paper's optimized reverse
 * search. The paper's O(1) bound per unlabeled representation does not apply here.
 */

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/proper_interval_unlabeled_enum.h"
#include "util/graph.h"
#include "util/labeled_expansion.h"
#include "recognizers/proper_interval.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for proper interval graph enumeration
 */
enum class ProperIntervalLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous chordal search with recognition */
    STRING_REPRESENTATION = 1 /**< Bracket strings plus label expansion (default) */
};

/**
 * @brief Result of proper interval graph enumeration
 */
struct ProperIntervalLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated proper interval graphs */
};

namespace detail {

/**
 * @brief Appends all distinct labelings of one unlabeled representative
 *
 * Swaps (1,2), (2,3), ..., (n-1,n) generate every vertex permutation. Exploring
 * their action on edge sets therefore reaches every labeling, while `seen`
 * identifies permutations differing by an automorphism. Exactly n!/|Aut(G)|
 * graphs are appended without enumerating all n! permutations first.
 * Different unlabeled representatives have disjoint orbits.
 *
 * The appended portion of `out` doubles as the traversal queue. For an orbit
 * of L graphs with m edges and n >= 1, there are L(n-1) swap attempts, each sorting m
 * edges and looking up a sorted edge vector in a set of at most L entries.
 * The set uses O(L(m+1)) extra space and is released after this representative.
 */
inline void proper_interval_labeled_expand(
    const ProperIntervalUnlabeledEnumeratedGraph& representative,
    std::vector<EnumeratedGraph>* out) {
    expand_graph_labelings(representative.n, representative.edges, out);
}

/**
 * @brief Build a Graph from ChordalLabeledEnumState (for proper interval)
 */
inline Graph proper_interval_state_to_graph(const ChordalLabeledEnumState& state) {
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
 * @brief DFS for proper interval reverse search
 *
 * Same structure as chordal reverse search, but verifies proper interval property
 * at each node and prunes non-proper-interval subtrees.
 */
inline void proper_interval_reverse_search_dfs(const ChordalLabeledEnumState& state,
                                               std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    std::vector<ChordalLabeledEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        Graph g = proper_interval_state_to_graph(children[i]);
        ProperIntervalResult pr = check_proper_interval(g);
        if (!pr.is_proper_interval) continue;

        proper_interval_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled proper interval graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (default: STRING_REPRESENTATION)
 * @return ProperIntervalLabeledEnumerationResult
 *
 * STRING_REPRESENTATION reuses the bracket-string unlabeled generator, including
 * disconnected graphs, and expands every representative's labelings exactly
 * once. REVERSE_SEARCH retains the previous chordal-tree recognition filter
 * and its output order. The two algorithms can enumerate in different orders.
 * n = 0 yields one empty graph. All results are materialized in memory.
 * The historical function name is retained for source compatibility.
 * @throws std::invalid_argument if n < 0 or n > max_vertex_count
 *         (n + 1 must fit in int, see util/vertex_count.h), or algo is unknown
 */
inline ProperIntervalLabeledEnumerationResult enumerate_proper_interval_labeled_graphs_reverse_search(int n,
    ProperIntervalLabeledEnumAlgorithm algo = ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION) {
    ProperIntervalLabeledEnumerationResult result;
    require_vertex_count("enumerate_proper_interval_labeled_graphs_reverse_search", n);
    switch (algo) {
        case ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION: {
            const ProperIntervalUnlabeledEnumerationResult representatives =
                enumerate_proper_interval_unlabeled_graphs(n);
            for (std::size_t i = 0; i < representatives.graphs.size(); ++i) {
                detail::proper_interval_labeled_expand(representatives.graphs[i], &result.graphs);
            }
            break;
        }
        case ProperIntervalLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::ChordalLabeledEnumState root(n);
            detail::proper_interval_reverse_search_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_proper_interval_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
