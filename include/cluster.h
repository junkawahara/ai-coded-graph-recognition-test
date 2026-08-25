#ifndef GRAPH_RECOGNITION_CLUSTER_H
#define GRAPH_RECOGNITION_CLUSTER_H

/**
 * @file cluster.h
 * @brief Cluster graph (P3-free) recognition
 *
 * A cluster graph is a disjoint union of cliques, equivalent to a graph with no induced P3.
 * Determined by checking whether each connected component is a complete graph. O(n+m) time.
 */

#include "components.h"
#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for cluster graph recognition
 */
enum class ClusterAlgorithm {
    COMPONENT_CLIQUE /**< check whether each connected component is a clique */
};

/**
 * @brief Result of cluster graph recognition
 */
struct ClusterResult {
    bool is_cluster = false; /**< true if the graph is a cluster graph */
};

/**
 * @brief Determines whether a graph is a cluster graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only COMPONENT_CLIQUE is implemented)
 * @return ClusterResult
 *
 * G is a cluster graph iff each connected component is a complete graph.
 * A complete graph of component size k has k(k-1)/2 edges.
 */
inline ClusterResult check_cluster(const Graph& g,
    ClusterAlgorithm algo = ClusterAlgorithm::COMPONENT_CLIQUE) {
    (void)algo;
    ClusterResult res;
    res.is_cluster = false;

    ComponentsResult cc = connected_components(g);
    const std::vector<std::vector<int> >& components = cc.vertices;

    for (size_t c = 0; c < components.size(); ++c) {
        int k = static_cast<int>(components[c].size());
        // Count edges within the component
        long long edge_count = 0;
        for (size_t i = 0; i < components[c].size(); ++i) {
            int u = components[c][i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                ++edge_count;
            }
        }
        edge_count /= 2; // Each edge was counted twice
        long long expected = (long long)k * (k - 1) / 2;
        if (edge_count != expected) return res;
    }

    res.is_cluster = true;
    return res;
}

} // namespace graph_recognition

#endif
