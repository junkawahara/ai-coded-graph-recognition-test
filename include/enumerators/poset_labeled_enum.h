#ifndef GRAPH_RECOGNITION_POSET_ENUM_H
#define GRAPH_RECOGNITION_POSET_ENUM_H

/**
 * @file poset_labeled_enum.h
 * @brief Labeled enumeration of all posets
 *
 * Constructively enumerates all labeled partial orders (posets) on vertex set {1, ..., n}.
 * For each of the n(n-1)/2 unordered pairs {i, j}, three choices
 * (i < j / j < i / incomparable) are explored by DFS, with incremental
 * transitive closure management and pruning via cycle detection. Output is the Hasse diagram (covering relation) of each poset.
 *
 * References:
 *   - Brinkmann, McKay, "Posets on up to 16 Points," Order 19(2), 2002
 * OEIS:
 *   - A001035 (labeled poset count): 1, 1, 3, 19, 219, 4231, ...
 *   - A000112 (unlabeled poset count)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Enumerated partial order (Hasse diagram representation)
 */
struct PosetLabeledEnumeratedGraph {
    int n;                                        /**< Number of elements */
    std::vector<std::pair<int, int> > arcs;       /**< Hasse diagram arc (u, v) = u < v (covering relation), sorted */
};

/**
 * @brief Result of poset enumeration
 */
struct PosetLabeledEnumerationResult {
    std::vector<PosetLabeledEnumeratedGraph> graphs;     /**< Array of enumerated partial orders */
};

namespace detail_poset_enum {

/** @brief Internal state for constructive enumeration */
struct PosetLabeledEnumState {
    int n;                                         /**< Number of elements */
    int num_pairs;                                 /**< C(n,2): number of unordered pairs */
    std::vector<std::pair<int, int> > pairs;       /**< All unordered pairs (i<j, lexicographic order) */
    std::vector<std::vector<char> > rel;           /**< rel[i][j]=1 iff i <_P j (transitive closure) */
    std::vector<std::vector<char> > incomp;        /**< incomp[i][j]=1 iff i||j has been decided */
};

/**
 * @brief Initialize state: build all unordered pairs
 */
inline PosetLabeledEnumState poset_build_state(int n) {
    PosetLabeledEnumState state;
    state.n = n;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            state.pairs.push_back(std::make_pair(i, j));
        }
    }
    state.num_pairs = static_cast<int>(state.pairs.size());
    state.rel.assign(static_cast<size_t>(n + 1),
                     std::vector<char>(static_cast<size_t>(n + 1), 0));
    state.incomp.assign(static_cast<size_t>(n + 1),
                        std::vector<char>(static_cast<size_t>(n + 1), 0));
    return state;
}

/**
 * @brief Extract Hasse diagram (covering relation) from the rel matrix
 *
 * arc (u, v) is a covering relation when u < v and no k with u < k < v exists.
 */
inline PosetLabeledEnumeratedGraph poset_extract_hasse(const PosetLabeledEnumState& state) {
    PosetLabeledEnumeratedGraph g;
    g.n = state.n;
    for (int u = 1; u <= state.n; ++u) {
        for (int v = 1; v <= state.n; ++v) {
            if (u == v) continue;
            if (!state.rel[static_cast<size_t>(u)][static_cast<size_t>(v)]) continue;
            bool is_cover = true;
            for (int k = 1; k <= state.n; ++k) {
                if (k == u || k == v) continue;
                if (state.rel[static_cast<size_t>(u)][static_cast<size_t>(k)] &&
                    state.rel[static_cast<size_t>(k)][static_cast<size_t>(v)]) {
                    is_cover = false;
                    break;
                }
            }
            if (is_cover) {
                g.arcs.push_back(std::make_pair(u, v));
            }
        }
    }
    std::sort(g.arcs.begin(), g.arcs.end());
    return g;
}

inline void poset_labeled_enum_dfs(PosetLabeledEnumState& state, int pair_idx,
                           std::vector<PosetLabeledEnumeratedGraph>* out);

/**
 * @brief Add relation lo < hi, update transitive closure, and recurse
 *
 * Sets all pairs of predecessors(lo) x successors(hi).
 * Prunes on cycle detection. Records changed cells for restoration on backtrack.
 */
inline void poset_try_add_relation(PosetLabeledEnumState& state, int lo, int hi,
                                   int pair_idx,
                                   std::vector<PosetLabeledEnumeratedGraph>* out) {
    // Cycle detection: hi < lo already holds
    if (state.rel[static_cast<size_t>(hi)][static_cast<size_t>(lo)]) return;

    // predecessors of lo (including lo): {a : rel[a][lo] || a == lo}
    // successors of hi (including hi):  {b : rel[hi][b] || b == hi}
    std::vector<int> preds, succs;
    preds.push_back(lo);
    for (int a = 1; a <= state.n; ++a) {
        if (a != lo && state.rel[static_cast<size_t>(a)][static_cast<size_t>(lo)]) {
            preds.push_back(a);
        }
    }
    succs.push_back(hi);
    for (int b = 1; b <= state.n; ++b) {
        if (b != hi && state.rel[static_cast<size_t>(hi)][static_cast<size_t>(b)]) {
            succs.push_back(b);
        }
    }

    // Cycle detection: cycle if preds ∩ succs is non-empty
    for (size_t si = 0; si < succs.size(); ++si) {
        int b = succs[si];
        for (size_t pi = 0; pi < preds.size(); ++pi) {
            if (preds[pi] == b) return;
        }
    }

    // Record changed cells and set transitive closure
    std::vector<std::pair<int, int> > changed;
    bool violates_incomp = false;
    for (size_t pi = 0; pi < preds.size() && !violates_incomp; ++pi) {
        int a = preds[pi];
        for (size_t si = 0; si < succs.size() && !violates_incomp; ++si) {
            int b = succs[si];
            if (!state.rel[static_cast<size_t>(a)][static_cast<size_t>(b)]) {
                // Incomparable constraint violation check
                if (state.incomp[static_cast<size_t>(a)][static_cast<size_t>(b)]) {
                    violates_incomp = true;
                    break;
                }
                state.rel[static_cast<size_t>(a)][static_cast<size_t>(b)] = 1;
                changed.push_back(std::make_pair(a, b));
            }
        }
    }

    if (!violates_incomp) {
        poset_labeled_enum_dfs(state, pair_idx + 1, out);
    }

    // Restore
    for (size_t ci = 0; ci < changed.size(); ++ci) {
        state.rel[static_cast<size_t>(changed[ci].first)]
                 [static_cast<size_t>(changed[ci].second)] = 0;
    }
}

/**
 * @brief DFS for constructive enumeration
 *
 * For the pair_idx-th unordered pair (i, j):
 * - If already determined by transitive closure, recurse without branching
 * - If undecided, explore with 3 branches:
 *   Branch 0: incomparable (i || j)
 *   Branch 1: i < j
 *   Branch 2: j < i
 * When all pair choices are determined, extract the Hasse diagram and output.
 */
inline void poset_labeled_enum_dfs(PosetLabeledEnumState& state, int pair_idx,
                           std::vector<PosetLabeledEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        out->push_back(poset_extract_hasse(state));
        return;
    }

    int i = state.pairs[static_cast<size_t>(pair_idx)].first;
    int j = state.pairs[static_cast<size_t>(pair_idx)].second;

    // If relation is already determined by transitive closure: recurse without branching
    if (state.rel[static_cast<size_t>(i)][static_cast<size_t>(j)] ||
        state.rel[static_cast<size_t>(j)][static_cast<size_t>(i)]) {
        poset_labeled_enum_dfs(state, pair_idx + 1, out);
        return;
    }

    // Branch 0: incomparable (i || j)
    state.incomp[static_cast<size_t>(i)][static_cast<size_t>(j)] = 1;
    state.incomp[static_cast<size_t>(j)][static_cast<size_t>(i)] = 1;
    poset_labeled_enum_dfs(state, pair_idx + 1, out);
    state.incomp[static_cast<size_t>(i)][static_cast<size_t>(j)] = 0;
    state.incomp[static_cast<size_t>(j)][static_cast<size_t>(i)] = 0;

    // Branch 1: i < j
    poset_try_add_relation(state, i, j, pair_idx, out);

    // Branch 2: j < i
    poset_try_add_relation(state, j, i, pair_idx, out);
}

} // namespace detail_poset_enum

/**
 * @brief Enumerates all labeled partial orders on element set {1, ..., n}
 * @param n Number of elements
 * @return PosetLabeledEnumerationResult
 *
 * Selects from 3 choices for each of the n(n-1)/2 unordered pairs
 * to construct all labeled partial orders via DFS. Incremental transitive
 * closure management enables early cycle detection and pruning. Output is Hasse diagrams (covering relations).
 */
inline PosetLabeledEnumerationResult enumerate_posets(int n) {
    PosetLabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        /* The empty poset: A001035(0) = 1 */
        PosetLabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    if (n == 1) {
        PosetLabeledEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_poset_enum::PosetLabeledEnumState state =
        detail_poset_enum::poset_build_state(n);
    detail_poset_enum::poset_labeled_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
