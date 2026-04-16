#ifndef GRAPH_RECOGNITION_CIRCLE_H
#define GRAPH_RECOGNITION_CIRCLE_H

/**
 * @file circle.h
 * @brief Circle graph recognition (DOW backtracking)
 *
 * A circle graph is the intersection graph of chords of a circle.
 * G is a circle graph iff there exists a double occurrence word (DOW) of length 2n
 * such that vertices i, j are adjacent iff the occurrences of i, j interleave in the DOW.
 *
 * Determines the result by constructing the DOW via backtracking.
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "graph.h"

namespace graph_recognition {

enum class CircleAlgorithm {
    DOW_BACKTRACKING /**< DOW backtracking */
};

struct CircleResult {
    bool is_circle;
    std::vector<int> dow; /**< double occurrence word on success (length 2n) */
};

namespace detail_circle {

/**
 * @brief Internal state for DOW backtracking
 */
struct DowState {
    int n;
    std::vector<std::vector<char>> adj; /**< adjacency matrix (1-indexed) */
    std::vector<int> word;              /**< DOW (0-indexed positions) */
    std::vector<int> first_pos;         /**< first occurrence position of each vertex (-1: unplaced) */
    std::vector<int> second_pos;        /**< second occurrence position of each vertex (-1: unplaced) */
    std::vector<int> placement;         /**< 0: unplaced, 1: first placed, 2: completed */
    std::vector<int> order;             /**< placement order (descending by degree) */
    bool found;

    explicit DowState(int n_)
        : n(n_), adj(n_ + 1, std::vector<char>(n_ + 1, 0)),
          word(2 * n_, -1), first_pos(n_ + 1, -1),
          second_pos(n_ + 1, -1), placement(n_ + 1, 0),
          order(), found(false) {}
};

/**
 * @brief Constraint check when placing vertex v for the second time
 *
 * When v's occurrence positions are [f, p], checks whether the fully placed vertex u's
 * occurrence positions [a, b] interleave, and verifies consistency with adj[u][v].
 */
inline bool check_second_placement(const DowState& state, int v, int pos) {
    int f = state.first_pos[v];
    int lo = f < pos ? f : pos;
    int hi = f < pos ? pos : f;
    for (int u = 1; u <= state.n; ++u) {
        if (u == v || state.placement[u] != 2) continue;
        int a = state.first_pos[u];
        int b = state.second_pos[u];
        if (a > b) { int tmp = a; a = b; b = tmp; }
        // u and v interleave iff exactly one of {a, b} lies in (lo, hi)
        bool a_in = (lo < a && a < hi);
        bool b_in = (lo < b && b < hi);
        bool interleave = (a_in != b_in);
        bool adjacent = (state.adj[u][v] != 0);
        if (interleave != adjacent) return false;
    }
    return true;
}

/**
 * @brief Backtracking DFS for DOW construction
 */
inline void dow_dfs(DowState& state, int pos) {
    if (state.found) return;
    if (pos == 2 * state.n) {
        state.found = true;
        return;
    }

    // (A) Place the second occurrence of half-placed vertices (stronger constraints -> try first)
    for (size_t idx = 0; idx < state.order.size(); ++idx) {
        int v = state.order[idx];
        if (state.placement[v] != 1) continue;
        if (!check_second_placement(state, v, pos)) continue;
        state.word[pos] = v;
        state.second_pos[v] = pos;
        state.placement[v] = 2;
        dow_dfs(state, pos + 1);
        if (state.found) return;
        state.placement[v] = 1;
        state.second_pos[v] = -1;
        state.word[pos] = -1;
    }

    // (B) Place the first occurrence of unplaced vertices
    for (size_t idx = 0; idx < state.order.size(); ++idx) {
        int v = state.order[idx];
        if (state.placement[v] != 0) continue;
        state.word[pos] = v;
        state.first_pos[v] = pos;
        state.placement[v] = 1;
        dow_dfs(state, pos + 1);
        if (state.found) return;
        state.placement[v] = 0;
        state.first_pos[v] = -1;
        state.word[pos] = -1;
    }
}

/**
 * @brief Circle graph recognition via DOW backtracking
 */
inline CircleResult check_circle_dow(const Graph& g) {
    CircleResult res;
    res.is_circle = false;
    int n = g.n;

    if (n == 0) {
        res.is_circle = true;
        return res;
    }

    DowState state(n);
    for (int u = 1; u <= n; ++u)
        for (size_t j = 0; j < g.adj[u].size(); ++j) {
            int v = g.adj[u][j];
            state.adj[u][v] = 1;
        }

    // Determine placement order by descending degree (higher degree -> more constraints -> better pruning)
    std::vector<std::pair<int, int>> deg_v;
    for (int v = 1; v <= n; ++v) {
        deg_v.push_back(std::make_pair(-(int)g.adj[v].size(), v));
    }
    std::sort(deg_v.begin(), deg_v.end());
    for (size_t i = 0; i < deg_v.size(); ++i) {
        state.order.push_back(deg_v[i].second);
    }

    // Fix the first occurrence of vertex order[0] at position 0 (break circular symmetry)
    int first_v = state.order[0];
    state.word[0] = first_v;
    state.first_pos[first_v] = 0;
    state.placement[first_v] = 1;

    dow_dfs(state, 1);

    if (state.found) {
        res.is_circle = true;
        res.dow = state.word;
    }
    return res;
}

}  // namespace detail_circle

/**
 * @brief Circle graph recognition
 * @param g Input graph (1-indexed)
 * @param algo Algorithm selection
 * @return CircleResult
 */
inline CircleResult check_circle(const Graph& g,
    CircleAlgorithm algo = CircleAlgorithm::DOW_BACKTRACKING) {
    (void)algo;
    return detail_circle::check_circle_dow(g);
}

}  // namespace graph_recognition

#endif
