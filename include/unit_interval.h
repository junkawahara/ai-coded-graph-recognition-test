#ifndef GRAPH_RECOGNITION_UNIT_INTERVAL_H
#define GRAPH_RECOGNITION_UNIT_INTERVAL_H

/**
 * @file unit_interval.h
 * @brief Unit interval graph recognition
 *
 * For simple graphs, unit interval graphs and proper interval graphs are equivalent.
 */

#include "graph.h"
#include "proper_interval.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for unit interval graph recognition
 */
enum class UnitIntervalAlgorithm {
    PROPER_INTERVAL /**< Proper interval recognition */
};

/**
 * @brief Result of unit interval graph recognition
 */
struct UnitIntervalResult {
    bool is_unit_interval = false; /**< true if the graph is a unit interval graph */
};

/**
 * @brief Determines whether the graph is a unit interval graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only PROPER_INTERVAL is implemented)
 * @return UnitIntervalResult
 */
inline UnitIntervalResult check_unit_interval(const Graph& g,
    UnitIntervalAlgorithm algo = UnitIntervalAlgorithm::PROPER_INTERVAL) {
    (void)algo;
    UnitIntervalResult res;
    res.is_unit_interval = false;

    ProperIntervalResult pres = check_proper_interval(g);
    if (!pres.is_proper_interval) return res;

    res.is_unit_interval = true;
    return res;
}

} // namespace graph_recognition

#endif
