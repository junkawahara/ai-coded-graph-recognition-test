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
#include "forbidden_subgraph.h"
#include "graph.h"
#include "obstruction_extract.h"

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
    Obstruction obstruction; /**< NO certificate: a P3. Cluster graphs are exactly
                                  the P3-free graphs, so a component that is not a
                                  clique always hides one. Valid only when
                                  is_cluster == false */
};

namespace detail {

/**
 * @brief Extracts an induced P3 from a component that is not a clique
 * @param g Input graph
 * @param comp Vertices of a connected component with fewer than k(k-1)/2 edges
 * @return A P3 obstruction, or an empty one if comp is a clique after all
 *
 * Runs in O(n+m), so the recognizer keeps its linear bound: a vertex missing
 * one of its component peers must exist, and the first three vertices of a
 * shortest path to a missed peer are pairwise at distance <= 2 with the ends
 * non-adjacent, which is exactly an induced P3.
 */
inline Obstruction p3_in_component(const Graph& g, const std::vector<int>& comp) {
    Obstruction o;
    int k = static_cast<int>(comp.size());
    if (k < 3) return o;

    int u = 0;
    for (size_t i = 0; i < comp.size(); ++i) {
        if (static_cast<int>(g.adj[comp[i]].size()) < k - 1) {
            u = comp[i];
            break;
        }
    }
    if (u == 0) return o;

    std::vector<unsigned char> closed(g.n + 1, 0);
    closed[u] = 1;
    for (size_t i = 0; i < g.adj[u].size(); ++i) closed[g.adj[u][i]] = 1;
    int y = 0;
    for (size_t i = 0; i < comp.size(); ++i) {
        if (!closed[comp[i]]) {
            y = comp[i];
            break;
        }
    }
    if (y == 0) return o;

    std::vector<unsigned char> allowed(g.n + 1, 1);
    allowed[0] = 0;
    std::vector<int> path = detail_obstruction::shortest_path_in_allowed(g, u, y, allowed);
    if (path.size() < 3) return o;

    std::vector<int> vs;
    vs.push_back(path[0]);
    vs.push_back(path[1]);
    vs.push_back(path[2]);
    return make_obstruction(ObstructionKind::P3, vs);
}

} // namespace detail

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
        if (edge_count != expected) {
            res.obstruction = detail::p3_in_component(g, components[c]);
            return res;
        }
    }

    res.is_cluster = true;
    return res;
}

} // namespace graph_recognition

#endif
