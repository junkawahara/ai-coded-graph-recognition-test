#ifndef GRAPH_RECOGNITION_CATERPILLAR_ENUM_H
#define GRAPH_RECOGNITION_CATERPILLAR_ENUM_H

/**
 * @file caterpillar_unlabeled_enum.h
 * @brief Caterpillar tree enumeration
 *
 * Enumerates all non-isomorphic caterpillar trees on n vertices
 * by constructive enumeration via spine + leaf allocation.
 *
 * A caterpillar tree is a tree in which all vertices are within distance 1 from a central path (spine).
 * It coincides with graphs of pathwidth 1. The isomorphism class of a caterpillar is uniquely
 * determined by the leaf count sequence (d_1, ..., d_s) and its reverse.
 *
 * Number of non-isomorphic caterpillar trees: OEIS A005418 with the index
 * shifted by two for n >= 3.
 *   1, 2, 3, 6, 10, 20, 36, 72, ... (n=3,4,5,6,7,8,9,10,...)
 *
 * References:
 *   Harary, Schwenk, "The number of caterpillars,"
 *   Discrete Math. 6(4), 1973
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for caterpillar enumeration
 */
enum class CaterpillarUnlabeledEnumAlgorithm {
    CONSTRUCTIVE /**< constructive enumeration by spine + leaf allocation */
};

/**
 * @brief Enumerated graph
 */
struct CaterpillarUnlabeledEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of caterpillar enumeration
 */
struct CaterpillarUnlabeledEnumerationResult {
    std::vector<CaterpillarUnlabeledEnumeratedGraph> graphs; /**< array of enumerated caterpillars */
};

namespace detail {

/**
 * @brief Determines whether the leaf count sequence is in palindrome canonical form
 *
 * To eliminate duplicates due to the reversal symmetry of the spine,
 * only consider it canonical when seq <= reverse(seq) in lexicographic order.
 */
inline bool is_palindrome_canonical(const std::vector<int>& seq) {
    int s = (int)seq.size();
    for (int i = 0; i < s / 2; ++i) {
        if (seq[i] < seq[s - 1 - i]) return true;
        if (seq[i] > seq[s - 1 - i]) return false;
    }
    return true;  // palindrome
}

/**
 * @brief Builds a caterpillar graph from the leaf count sequence
 *
 * Assigns spine vertices as 1, 2, ..., s and leaves starting from s+1.
 */
inline CaterpillarUnlabeledEnumeratedGraph build_caterpillar(
    int n, const std::vector<int>& leaf_counts) {

    int s = (int)leaf_counts.size();
    CaterpillarUnlabeledEnumeratedGraph g;
    g.n = n;

    // Spine edges: 1-2, 2-3, ..., (s-1)-s
    for (int i = 1; i < s; ++i) {
        g.edges.push_back(std::make_pair(i, i + 1));
    }

    // Leaf edges
    int next_leaf = s + 1;
    for (int i = 0; i < s; ++i) {
        int spine_v = i + 1;
        for (int j = 0; j < leaf_counts[i]; ++j) {
            g.edges.push_back(std::make_pair(spine_v, next_leaf));
            ++next_leaf;
        }
    }

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief Recursively enumerates constrained compositions
 *
 * Distributes r leaves among s spine vertices.
 * Constraints: d_1 >= 1, d_s >= 1, d_i >= 0 (2 <= i <= s-1).
 * Only outputs those in palindrome canonical form.
 */
inline void enumerate_compositions_dfs(
    int pos, int remaining, int s,
    std::vector<int>& current,
    std::vector<CaterpillarUnlabeledEnumeratedGraph>& out,
    int n) {

    if (pos == s) {
        if (remaining != 0) return;
        if (!is_palindrome_canonical(current)) return;
        out.push_back(build_caterpillar(n, current));
        return;
    }

    int min_val = (pos == 0 || pos == s - 1) ? 1 : 0;

    // Future positions need at least this much total
    int future_min = (pos < s - 1) ? 1 : 0;
    int max_val = remaining - future_min;

    for (int d = min_val; d <= max_val; ++d) {
        current[pos] = d;
        enumerate_compositions_dfs(pos + 1, remaining - d, s, current, out, n);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic caterpillar trees on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: CONSTRUCTIVE)
 * @return CaterpillarUnlabeledEnumerationResult
 *
 * Iterates spine length s from 1 to n-2, and for each spine length
 * enumerates leaf allocations. Only non-isomorphic graphs are output
 * using palindrome canonical form that accounts for spine reversal symmetry.
 * For n = 0 the single empty graph is returned; for n < 0 the result is empty.
 */
inline CaterpillarUnlabeledEnumerationResult enumerate_caterpillar_unlabeled_graphs(int n,
    CaterpillarUnlabeledEnumAlgorithm algo = CaterpillarUnlabeledEnumAlgorithm::CONSTRUCTIVE) {
    (void)algo;
    CaterpillarUnlabeledEnumerationResult result;
    if (n < 0) return result;

    if (n == 0) {
        // The empty graph is a caterpillar (check_caterpillar(n=0) is YES)
        CaterpillarUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 1) {
        CaterpillarUnlabeledEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 2) {
        CaterpillarUnlabeledEnumeratedGraph g;
        g.n = 2;
        g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }

    // n >= 3
    for (int s = 1; s <= n - 2; ++s) {
        int r = n - s;  // number of leaves

        if (s == 1) {
            // Star K_{1,r}
            std::vector<int> leaf_counts(1, r);
            result.graphs.push_back(detail::build_caterpillar(n, leaf_counts));
        } else {
            // s >= 2: need r >= 2 (d_1 >= 1, d_s >= 1)
            if (r < 2) continue;
            std::vector<int> current(s, 0);
            detail::enumerate_compositions_dfs(
                0, r, s, current, result.graphs, n);
        }
    }

    return result;
}

}  // namespace graph_recognition

#endif
