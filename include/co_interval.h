#ifndef GRAPH_RECOGNITION_CO_INTERVAL_H
#define GRAPH_RECOGNITION_CO_INTERVAL_H

#include "graph.h"
#include "graph_utils.h"
#include "interval.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for co-interval graph recognition
 */
enum class CoIntervalAlgorithm {
    COMPLEMENT /**< interval test on the complement graph */
};

// Result of co-interval graph recognition.
struct CoIntervalResult {
    bool is_co_interval = false;
};

// Check whether a graph is a co-interval graph.
// Characterization used:
//   G is co-interval iff complement(G) is interval.
inline CoIntervalResult check_co_interval(const Graph& g,
    CoIntervalAlgorithm algo = CoIntervalAlgorithm::COMPLEMENT) {
    (void)algo;
    CoIntervalResult res;
    res.is_co_interval = false;

    Graph gc = build_complement(g);
    IntervalResult ires = check_interval(gc);
    if (!ires.is_interval) return res;

    res.is_co_interval = true;
    return res;
}

} // namespace graph_recognition

#endif
