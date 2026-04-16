#ifndef GRAPH_RECOGNITION_OUTER_PLANAR_H
#define GRAPH_RECOGNITION_OUTER_PLANAR_H

/**
 * @file outer_planar.h
 * @brief Outerplanar graph recognition
 *
 * G is outerplanar iff G + K1 (adding a new vertex connected to all vertices) is planar.
 * Uses the de Fraysseix-Rosenstiehl interlacement method for planarity testing.
 *
 * K4 / K2,3 minor check is also available as a fallback.
 */

#include "graph.h"
#include "minor.h"
#include "planar.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for outerplanar graph recognition
 */
enum class OuterPlanarAlgorithm {
    AUGMENTED_PLANARITY, /**< Planarity check of G + K1 */
    MINOR_CHECK          /**< Minor check (exponential time) */
};

/**
 * @brief Result of outerplanar graph recognition
 */
struct OuterPlanarResult {
    bool is_outer_planar = false; /**< true if the graph is outerplanar */
};

/**
 * @brief Determines whether the graph is an outerplanar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: AUGMENTED_PLANARITY)
 * @return OuterPlanarResult
 */
inline OuterPlanarResult check_outer_planar(const Graph& g,
    OuterPlanarAlgorithm algo = OuterPlanarAlgorithm::AUGMENTED_PLANARITY) {
    OuterPlanarResult res;
    res.is_outer_planar = false;

    int n = g.n;
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    if (n <= 2) {
        res.is_outer_planar = true;
        return res;
    }

    // Edge count upper bound for simple outerplanar graphs.
    if (n >= 2 && m > 2LL * n - 3) return res;

    if (algo == OuterPlanarAlgorithm::MINOR_CHECK) {
        detail_minor::MinorState st = detail_minor::build_minor_state(g);

        detail_minor::MinorChecker k4_checker(detail_minor::MinorTarget::K4);
        if (k4_checker.has_minor(st)) return res;

        detail_minor::MinorChecker k23_checker(detail_minor::MinorTarget::K23);
        if (k23_checker.has_minor(st)) return res;

        res.is_outer_planar = true;
        return res;
    }

    // AUGMENTED_PLANARITY: Build G + K1 and check planarity
    // Connect new vertex n+1 to all existing vertices
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<size_t>(m + n));
    for (int v = 1; v <= n; ++v) {
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int w = g.adj[v][i];
            if (v < w) {
                edges.push_back(std::make_pair(v, w));
            }
        }
    }
    int new_v = n + 1;
    for (int v = 1; v <= n; ++v) {
        edges.push_back(std::make_pair(new_v, v));
    }

    Graph augmented(new_v, edges);
    PlanarResult pr = check_planar(augmented, PlanarAlgorithm::MINOR_CHECK);
    res.is_outer_planar = pr.is_planar;
    return res;
}

} // namespace graph_recognition

#endif
