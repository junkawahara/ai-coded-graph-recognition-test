#ifndef GRAPH_RECOGNITION_TOURNAMENT_ENUM_H
#define GRAPH_RECOGNITION_TOURNAMENT_ENUM_H

/**
 * @file tournament_labeled_enum.h
 * @brief Labeled enumeration of all tournaments
 *
 * Constructively enumerates all directed graphs (tournaments) obtained by
 * orienting all edges of the complete graph K_n. For each of the n(n-1)/2
 * edge pairs, two orientations are explored by DFS, producing all
 * 2^(n(n-1)/2) labeled tournaments without duplicates.
 *
 * Since the graph obtained at each stage of construction is by definition always
 * a tournament, no recognition filter is needed (constructive enumeration).
 *
 * References:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 *   - Moon, "Topics on Tournaments," Holt, Rinehart & Winston, 1968
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Enumerated tournament (directed complete graph)
 */
struct TournamentLabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > arcs;       /**< Directed edge list (u, v) = u->v, sorted */
};

/**
 * @brief Result of tournament enumeration
 */
struct TournamentLabeledEnumerationResult {
    std::vector<TournamentLabeledEnumeratedGraph> graphs; /**< Array of enumerated tournaments */
};

namespace detail_tournament_enum {

/** @brief Internal state for constructive enumeration */
struct TournamentLabeledEnumState {
    int n;                                         /**< Number of vertices */
    int num_pairs;                                 /**< C(n,2): number of edge pairs */
    std::vector<std::pair<int, int> > pairs;       /**< All pairs of K_n (i<j, lexicographic order) */
    std::vector<std::pair<int, int> > current_arcs; /**< Currently selected arc sequence */
};

/**
 * @brief Initialize state: build all edge pairs
 */
inline TournamentLabeledEnumState tournament_build_state(int n) {
    TournamentLabeledEnumState state;
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
 * Explores 2 branches (i->j / j->i) for the pair_idx-th edge pair (i, j).
 * Outputs the tournament when all pair orientations are determined.
 */
inline void tournament_labeled_enum_dfs(TournamentLabeledEnumState& state, int pair_idx,
                                std::vector<TournamentLabeledEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        TournamentLabeledEnumeratedGraph g;
        g.n = state.n;
        g.arcs = state.current_arcs;
        std::sort(g.arcs.begin(), g.arcs.end());
        out->push_back(g);
        return;
    }

    int i = state.pairs[pair_idx].first;
    int j = state.pairs[pair_idx].second;

    // Branch 1: arc i -> j
    state.current_arcs.push_back(std::make_pair(i, j));
    tournament_labeled_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // Branch 2: arc j -> i
    state.current_arcs.push_back(std::make_pair(j, i));
    tournament_labeled_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();
}

} // namespace detail_tournament_enum

/**
 * @brief Enumerates all labeled tournaments on vertex set {1, ..., n}
 * @param n Number of vertices
 * @return TournamentLabeledEnumerationResult
 *
 * For each of the n(n-1)/2 edge pairs of K_n, selects one of 2 orientations
 * via DFS to construct all 2^(n(n-1)/2) labeled tournaments.
 */
inline TournamentLabeledEnumerationResult enumerate_tournaments(int n) {
    TournamentLabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_tournament returns true for the empty tournament; emit it so
        // the enumerator agrees with OEIS A000568 (a(0) = 1).
        TournamentLabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    if (n == 1) {
        TournamentLabeledEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_tournament_enum::TournamentLabeledEnumState state =
        detail_tournament_enum::tournament_build_state(n);
    detail_tournament_enum::tournament_labeled_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
