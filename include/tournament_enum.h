#ifndef GRAPH_RECOGNITION_TOURNAMENT_ENUM_H
#define GRAPH_RECOGNITION_TOURNAMENT_ENUM_H

/**
 * @file tournament_enum.h
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
struct TournamentEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > arcs;       /**< Directed edge list (u, v) = u->v, sorted */
};

/**
 * @brief Result of tournament enumeration
 */
struct TournamentEnumerationResult {
    std::vector<TournamentEnumeratedGraph> graphs; /**< Array of enumerated tournaments */
};

namespace detail_tournament_enum {

/** @brief Internal state for constructive enumeration */
struct TournamentEnumState {
    int n;                                         /**< Number of vertices */
    int num_pairs;                                 /**< C(n,2): number of edge pairs */
    std::vector<std::pair<int, int> > pairs;       /**< All pairs of K_n (i<j, lexicographic order) */
    std::vector<std::pair<int, int> > current_arcs; /**< Currently selected arc sequence */
};

/**
 * @brief Initialize state: build all edge pairs
 */
inline TournamentEnumState tournament_build_state(int n) {
    TournamentEnumState state;
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
inline void tournament_enum_dfs(TournamentEnumState& state, int pair_idx,
                                std::vector<TournamentEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        TournamentEnumeratedGraph g;
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
    tournament_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // Branch 2: arc j -> i
    state.current_arcs.push_back(std::make_pair(j, i));
    tournament_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();
}

} // namespace detail_tournament_enum

/**
 * @brief Enumerates all labeled tournaments on vertex set {1, ..., n}
 * @param n Number of vertices
 * @return TournamentEnumerationResult
 *
 * For each of the n(n-1)/2 edge pairs of K_n, selects one of 2 orientations
 * via DFS to construct all 2^(n(n-1)/2) labeled tournaments.
 */
inline TournamentEnumerationResult enumerate_tournaments(int n) {
    TournamentEnumerationResult result;
    if (n <= 0) return result;
    if (n == 1) {
        TournamentEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_tournament_enum::TournamentEnumState state =
        detail_tournament_enum::tournament_build_state(n);
    detail_tournament_enum::tournament_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
