#ifndef GRAPH_RECOGNITION_THRESHOLD_ENUM_H
#define GRAPH_RECOGNITION_THRESHOLD_ENUM_H

/**
 * @file threshold_enum.h
 * @brief Threshold graph enumeration
 *
 * Enumerates all non-isomorphic threshold graphs on n vertices
 * via direct construction from binary strings. At each step, an isolated
 * vertex (0) or dominating vertex (1) is added, generating 2^(n-1) graphs.
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for threshold graph enumeration
 */
enum class ThresholdEnumAlgorithm {
    BINARY_STRING /**< Direct construction from binary string */
};

/**
 * @brief Enumerated graph
 */
struct ThresholdEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int>> edges;       /**< Edge list (sorted by u < v) */
};

/**
 * @brief Result of threshold graph enumeration
 */
struct ThresholdEnumerationResult {
    std::vector<ThresholdEnumeratedGraph> graphs;  /**< Array of enumerated threshold graphs */
};

namespace detail {

/**
 * @brief Construct threshold graph from binary string (bitmask)
 * @param n Number of vertices
 * @param mask Bitmask of length n-1 (bit i: 1 if vertex i+2 is a dominating vertex)
 * @return Edge list (sorted by u < v)
 */
inline std::vector<std::pair<int, int>> build_threshold_graph(int n, unsigned long long mask) {
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < n - 1; ++i) {
        if (mask & (1ULL << i)) {
            // vertex i+2 is a dominating vertex: adjacent to existing vertices 1..i+1
            for (int j = 1; j <= i + 1; ++j) {
                edges.push_back(std::make_pair(j, i + 2));
            }
        }
    }
    return edges;
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic threshold graphs on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: BINARY_STRING)
 * @return ThresholdEnumerationResult
 *
 * Directly constructs 2^(n-1) non-isomorphic threshold graphs using
 * binary string characterization. Bit i of each string indicates whether
 * vertex i+2 is a dominating vertex (1) or isolated vertex (0).
 *
 * @note The 2^(n-1) bitmask requires n <= 63 (one less than the bit width
 *       of unsigned long long). For n > 63 an empty result is returned;
 *       allocating 2^63 graphs is infeasible in any case.
 */
inline ThresholdEnumerationResult enumerate_threshold_graphs(int n,
    ThresholdEnumAlgorithm algo = ThresholdEnumAlgorithm::BINARY_STRING) {
    (void)algo;
    ThresholdEnumerationResult result;
    if (n <= 0) return result;

    // n > 63 would overflow the bitmask; see the @note above.
    if (n > 63) return result;
    unsigned long long total = 1ULL << (n - 1);  // 2^(n-1)
    result.graphs.reserve(static_cast<std::size_t>(total));

    for (unsigned long long mask = 0; mask < total; ++mask) {
        ThresholdEnumeratedGraph g;
        g.n = n;
        g.edges = detail::build_threshold_graph(n, mask);
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
