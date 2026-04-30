#ifndef GRAPH_RECOGNITION_DIGRAPH_ENUM_H
#define GRAPH_RECOGNITION_DIGRAPH_ENUM_H

/**
 * @file digraph_enum.h
 * @brief Labeled enumeration of all digraphs (directed graphs)
 *
 * Constructively enumerates all labeled simple digraphs (no self-loops)
 * on vertex set {1, ..., n}. For each of n(n-1)/2 unordered pairs {i, j},
 * a DFS selects from 4 choices (no edge / i->j / j->i / both directions),
 * enumerating all 4^(n(n-1)/2) = 2^(n(n-1)) labeled digraphs without duplicates.
 *
 * Each graph produced at every stage is by definition a valid digraph,
 * so no recognition filter is needed (constructive enumeration).
 *
 * References:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 *   - McKay, nauty User's Guide (directg)
 * OEIS:
 *   - A000273 (number of unlabeled digraphs)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief An enumerated digraph
 */
struct DigraphEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > arcs;       /**< Directed edge list (u, v) = u->v, sorted */
};

/**
 * @brief Result of digraph enumeration
 */
struct DigraphEnumerationResult {
    std::vector<DigraphEnumeratedGraph> graphs;   /**< Array of enumerated digraphs */
};

namespace detail_digraph_enum {

/** @brief Internal state for constructive enumeration */
struct DigraphEnumState {
    int n;                                         /**< Number of vertices */
    int num_pairs;                                 /**< C(n,2): number of unordered pairs */
    std::vector<std::pair<int, int> > pairs;       /**< All unordered pairs (i<j, lexicographic order) */
    std::vector<std::pair<int, int> > current_arcs; /**< Currently selected arc sequence */
};

/**
 * @brief State initialization: construct all unordered pairs
 */
inline DigraphEnumState digraph_build_state(int n) {
    DigraphEnumState state;
    state.n = n;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            state.pairs.push_back(std::make_pair(i, j));
        }
    }
    state.num_pairs = static_cast<int>(state.pairs.size());
    return state;
}

/**
 * @brief DFS for constructive enumeration
 *
 * For the pair_idx-th unordered pair (i, j), explores 4 branches:
 *   Branch 0: no edge
 *   Branch 1: arc i->j only
 *   Branch 2: arc j->i only
 *   Branch 3: both arcs i->j and j->i
 * Outputs a digraph once all pair selections are determined.
 */
inline void digraph_enum_dfs(DigraphEnumState& state, int pair_idx,
                             std::vector<DigraphEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        DigraphEnumeratedGraph g;
        g.n = state.n;
        g.arcs = state.current_arcs;
        std::sort(g.arcs.begin(), g.arcs.end());
        out->push_back(g);
        return;
    }

    int i = state.pairs[pair_idx].first;
    int j = state.pairs[pair_idx].second;

    // Branch 0: no edge
    digraph_enum_dfs(state, pair_idx + 1, out);

    // Branch 1: arc i -> j only
    state.current_arcs.push_back(std::make_pair(i, j));
    digraph_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // Branch 2: arc j -> i only
    state.current_arcs.push_back(std::make_pair(j, i));
    digraph_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // Branch 3: both arcs i -> j and j -> i
    state.current_arcs.push_back(std::make_pair(i, j));
    state.current_arcs.push_back(std::make_pair(j, i));
    digraph_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();
    state.current_arcs.pop_back();
}

} // namespace detail_digraph_enum

/**
 * @brief Enumerates all labeled digraphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @return DigraphEnumerationResult
 *
 * For n(n-1)/2 unordered pairs, a DFS selects from 4 edge choices per pair
 * to construct all 4^(n(n-1)/2) = 2^(n(n-1)) labeled digraphs.
 */
inline DigraphEnumerationResult enumerate_digraphs(int n) {
    DigraphEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_digraph accepts n = 0 with no arcs; emit the empty digraph.
        DigraphEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    if (n == 1) {
        DigraphEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_digraph_enum::DigraphEnumState state =
        detail_digraph_enum::digraph_build_state(n);
    detail_digraph_enum::digraph_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
