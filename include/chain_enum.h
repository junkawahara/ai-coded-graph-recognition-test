#ifndef GRAPH_RECOGNITION_CHAIN_ENUM_H
#define GRAPH_RECOGNITION_CHAIN_ENUM_H

/**
 * @file chain_enum.h
 * @brief Chain graph enumeration
 *
 * Enumerates all non-isomorphic chain graphs on n vertices by direct construction
 * of staircase adjacency matrices. Since chain graphs are unigraphs
 * (uniquely determined by their degree sequence), deduplication by degree sequence works correctly.
 *
 * Number of non-isomorphic types: OEIS A005418
 *   a(n) = 2^(n-2) + 2^(floor(n/2)-1) (n >= 2)
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for chain graph enumeration
 */
enum class ChainEnumAlgorithm {
    STAIRCASE /**< direct construction of staircase adjacency matrix */
};

/**
 * @brief Enumerated graph
 */
struct ChainEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int>> edges;       /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of chain graph enumeration
 */
struct ChainEnumerationResult {
    std::vector<ChainEnumeratedGraph> graphs;     /**< array of enumerated chain graphs */
};

namespace detail {

/**
 * @brief Recursively enumerates non-decreasing sequences and builds chain graphs from each
 * @param pos Current position (0-indexed)
 * @param min_val Minimum value at current position (ensures non-decreasing property)
 * @param p Number of vertices on the X side
 * @param q Number of vertices on the Y side
 * @param current Non-decreasing sequence currently being built
 * @param seen Set of previously seen degree sequences (for deduplication)
 * @param out Output destination
 * @param n Total number of vertices
 */
inline void enumerate_staircases(
    int pos, int min_val, int p, int q,
    std::vector<int>& current,
    std::set<std::vector<int> >& seen,
    std::vector<ChainEnumeratedGraph>& out,
    int n)
{
    if (pos == p) {
        // Compute Y-side degrees: e_j = |{i : current[i] >= j}|
        // Since current is non-decreasing, binary search speeds this up
        std::vector<int> deg;
        deg.reserve(n);
        for (int i = 0; i < p; ++i) {
            deg.push_back(current[i]);
        }
        for (int j = 1; j <= q; ++j) {
            // Number of i with current[i] >= j = p - (lower_bound of j)
            int cnt = p - static_cast<int>(
                std::lower_bound(current.begin(), current.begin() + p, j)
                - current.begin());
            deg.push_back(cnt);
        }
        std::sort(deg.begin(), deg.end());

        if (seen.count(deg)) return;
        seen.insert(deg);

        // Build graph: x_i = vertex (i+1), y_j = vertex (p+j)
        // x_i is adjacent to y_1, ..., y_{d_i}
        ChainEnumeratedGraph g;
        g.n = n;
        for (int i = 0; i < p; ++i) {
            for (int j = 1; j <= current[i]; ++j) {
                g.edges.push_back(std::make_pair(i + 1, p + j));
            }
        }
        out.push_back(g);
        return;
    }

    for (int val = min_val; val <= q; ++val) {
        current[pos] = val;
        enumerate_staircases(pos + 1, val, p, q, current, seen, out, n);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic chain graphs on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: STAIRCASE)
 * @return ChainEnumerationResult
 *
 * Scans staircase adjacency matrices for all bipartition candidates (p, q)
 * and outputs only non-isomorphic graphs using degree sequence deduplication.
 */
inline ChainEnumerationResult enumerate_chain_graphs(int n,
    ChainEnumAlgorithm algo = ChainEnumAlgorithm::STAIRCASE) {
    (void)algo;
    ChainEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        /* The empty graph is a chain graph (check_chain(n=0) is YES) */
        ChainEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    std::set<std::vector<int> > seen;

    for (int p = 0; p <= n / 2; ++p) {
        int q = n - p;
        std::vector<int> current(p, 0);
        detail::enumerate_staircases(0, 0, p, q, current, seen, result.graphs, n);
    }

    return result;
}

}  // namespace graph_recognition

#endif
