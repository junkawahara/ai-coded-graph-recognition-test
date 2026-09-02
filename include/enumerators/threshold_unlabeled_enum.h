#ifndef GRAPH_RECOGNITION_THRESHOLD_ENUM_H
#define GRAPH_RECOGNITION_THRESHOLD_ENUM_H

/**
 * @file threshold_unlabeled_enum.h
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
enum class ThresholdUnlabeledEnumAlgorithm {
    BINARY_STRING /**< Direct construction from binary string */
};

/**
 * @brief Enumerated graph
 */
struct ThresholdUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int>> edges;       /**< Edge list (sorted by u < v) */
};

/**
 * @brief Result of threshold graph enumeration
 */
struct ThresholdUnlabeledEnumerationResult {
    std::vector<ThresholdUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated threshold graphs */
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

/** @brief Callback appending each enumerated graph to a vector */
struct ThresholdUnlabeledAppend {
    std::vector<ThresholdUnlabeledEnumeratedGraph>* out;
    void operator()(const ThresholdUnlabeledEnumeratedGraph& g) const {
        out->push_back(g);
    }
};

}  // namespace detail

/**
 * @brief Streams all non-isomorphic threshold graphs on n vertices
 * @param n Number of vertices
 * @param cb Callback invoked as cb(const ThresholdUnlabeledEnumeratedGraph&)
 *           once per graph
 * @param algo Algorithm to use (default: BINARY_STRING)
 *
 * The same enumeration as enumerate_threshold_unlabeled_graphs() without
 * retaining the output: every binary string builds its graph independently,
 * so this keeps one graph instead of all 2^(n-1) of them. The callback
 * receives the enumerator's own buffer, so copy it to keep it.
 *
 * For n = 0 the single empty graph is emitted; n < 0 and n > 63 emit nothing.
 */
template <typename Callback>
inline void enumerate_threshold_unlabeled_graphs_cb(int n, Callback&& cb,
    ThresholdUnlabeledEnumAlgorithm algo = ThresholdUnlabeledEnumAlgorithm::BINARY_STRING) {
    (void)algo;
    if (n < 0 || n > 63) return;

    ThresholdUnlabeledEnumeratedGraph g;
    g.n = n;
    if (n == 0) {
        // The empty graph is a threshold graph (check_threshold(n=0) is YES)
        cb(g);
        return;
    }

    const unsigned long long total = 1ULL << (n - 1);  // 2^(n-1)
    for (unsigned long long mask = 0; mask < total; ++mask) {
        g.edges = detail::build_threshold_graph(n, mask);
        cb(g);
    }
}

/**
 * @brief Enumerates all non-isomorphic threshold graphs on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: BINARY_STRING)
 * @return ThresholdUnlabeledEnumerationResult
 *
 * Directly constructs 2^(n-1) non-isomorphic threshold graphs using
 * binary string characterization. Bit i of each string indicates whether
 * vertex i+2 is a dominating vertex (1) or isolated vertex (0).
 *
 * For n = 0 the single empty graph is returned; for n < 0 the result is empty.
 * Any @p algo value runs BINARY_STRING, the only algorithm.
 *
 * @note The 2^(n-1) bitmask requires n <= 63 (one less than the bit width
 *       of unsigned long long). For n > 63 an empty result is returned;
 *       allocating 2^63 graphs is infeasible in any case.
 * @note This entry point holds every one of the 2^(n-1) graphs at once, so it
 *       runs out of memory well before that representable bound (n = 18 is
 *       already 131,072 graphs). Callers that only need to see each graph
 *       once should use enumerate_threshold_unlabeled_graphs_cb(), which
 *       keeps one graph at a time.
 */
inline ThresholdUnlabeledEnumerationResult enumerate_threshold_unlabeled_graphs(int n,
    ThresholdUnlabeledEnumAlgorithm algo = ThresholdUnlabeledEnumAlgorithm::BINARY_STRING) {
    ThresholdUnlabeledEnumerationResult result;
    if (n < 0 || n > 63) return result;

    // 2^(n-1) is exact but not necessarily allocatable: reserving it for a
    // large n throws std::length_error before a single graph is built, which
    // hides the real failure (the output does not fit in memory).
    if (n >= 1 && n <= 26) {
        result.graphs.reserve(static_cast<std::size_t>(1ULL << (n - 1)));
    }
    detail::ThresholdUnlabeledAppend appender;
    appender.out = &result.graphs;
    enumerate_threshold_unlabeled_graphs_cb(n, appender, algo);
    return result;
}

}  // namespace graph_recognition

#endif
