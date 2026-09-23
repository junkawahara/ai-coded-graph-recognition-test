#ifndef GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_ENUM_H
#define GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_ENUM_H

/**
 * @file bipartite_permutation_labeled_enum.h
 * @brief Labeled bipartite permutation enumeration from canonical line strings
 *
 * Enumerates all labeled bipartite permutation graphs on vertex set {1, ..., n}
 * by expanding the unlabeled string generator's representatives, including
 * disconnected component multisets, into distinct labelings. Adjacent label
 * swaps and a visited edge-set collection remove automorphism duplicates.
 * The previous largest-label vertex-addition search with recognition remains
 * available as REVERSE_SEARCH. Neither mode has the paper's O(1) delay bound.
 *
 * References: Saitoh, Otachi, Yamanaka, Uehara,
 * J. Discrete Algorithms 10, 2012 (ISAAC 2009)
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "recognizers/bipartite_permutation.h"
#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/bipartite_permutation_unlabeled_enum.h"
#include "util/graph.h"
#include "util/labeled_expansion.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for bipartite permutation enumeration
 */
enum class BipartitePermutationLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous vertex additions with recognition */
    STRING_REPRESENTATION = 1 /**< Canonical line strings plus label expansion (default) */
};

/**
 * @brief Result of bipartite permutation enumeration
 */
struct BipartitePermutationLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated bipartite permutation graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct BPEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit BPEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for bipartite permutation reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not bipartite permutation.
 */
inline void bp_enum_dfs(BPEnumState& state,
                        std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    // L10: silently stops enumeration; 64+ vertices would overflow 2^k
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    // M23: pre-extract base edges (between vertices 1..k, excluding x)
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
        }
        state.alive_count = x;

        std::vector<std::pair<int, int>> edges = base_edges;
        for (int u = 1; u <= k; ++u)
            if (state.adj[x][u])
                edges.push_back(std::make_pair(u, x));
        Graph g(x, edges);
        BipartitePermutationResult res = check_bipartite_permutation(g);

        if (res.is_bipartite_permutation) {
            bp_enum_dfs(state, out);
        }

        for (int u = 1; u <= k; ++u) {
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled bipartite permutation graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (default: STRING_REPRESENTATION)
 * @return BipartitePermutationLabeledEnumerationResult
 *
 * The default expands each non-isomorphic representative into n!/|Aut(G)|
 * labelings, including twins and repeated components. REVERSE_SEARCH retains
 * the original recognition-based search and output order. Both emit one empty
 * graph at n = 0 and keep all results in memory; their output orders may differ.
 * The historical function name and 64-vertex limit remain for compatibility.
 * @throws std::invalid_argument if n < 0 or n > 64
 *         (the existing API limit), or algo is unknown
 */
inline BipartitePermutationLabeledEnumerationResult
enumerate_bipartite_permutation_labeled_graphs_reverse_search(int n,
    BipartitePermutationLabeledEnumAlgorithm algo =
        BipartitePermutationLabeledEnumAlgorithm::STRING_REPRESENTATION) {
    BipartitePermutationLabeledEnumerationResult result;
    require_vertex_count("enumerate_bipartite_permutation_labeled_graphs_reverse_search", n, 64);
    switch (algo) {
        case BipartitePermutationLabeledEnumAlgorithm::STRING_REPRESENTATION: {
            const auto representatives = detail::bipartite_permutation_unlabeled_string_graphs(n, false);
            for (const auto& graph : representatives.graphs)
                detail::expand_graph_labelings(graph.n, graph.edges, &result.graphs);
            break;
        }
        case BipartitePermutationLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::BPEnumState root(n);
            detail::bp_enum_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_bipartite_permutation_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
