#ifndef GRAPH_RECOGNITION_QUASI_THRESHOLD_H
#define GRAPH_RECOGNITION_QUASI_THRESHOLD_H

/**
 * @file quasi_threshold.h
 * @brief Quasi-threshold graph recognition
 *
 * A quasi-threshold graph is another name for a trivially perfect graph.
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "trivially_perfect.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for quasi-threshold graph recognition
 */
enum class QuasiThresholdAlgorithm {
    DFS /**< Recognition via DFS */
};

/**
 * @brief Result of quasi-threshold graph recognition
 */
struct QuasiThresholdResult {
    bool is_quasi_threshold = false; /**< true if the graph is a quasi-threshold graph */
    Obstruction obstruction; /**< NO certificate: a C4 or a P4, forwarded from
                                  trivially perfect. Valid only when
                                  is_quasi_threshold == false */
};

/**
 * @brief Determines whether the graph is a quasi-threshold graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only DFS is implemented)
 * @return QuasiThresholdResult
 */
inline QuasiThresholdResult check_quasi_threshold(const Graph& g,
    QuasiThresholdAlgorithm algo = QuasiThresholdAlgorithm::DFS) {
    (void)algo;
    QuasiThresholdResult res;
    res.is_quasi_threshold = false;

    TriviallyPerfectResult tres = check_trivially_perfect(g);
    if (!tres.is_trivially_perfect) {
        res.obstruction = tres.obstruction;
        return res;
    }

    res.is_quasi_threshold = true;
    return res;
}

} // namespace graph_recognition

#endif
