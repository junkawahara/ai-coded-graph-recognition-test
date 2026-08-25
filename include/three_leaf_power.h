#ifndef GRAPH_RECOGNITION_THREE_LEAF_POWER_H
#define GRAPH_RECOGNITION_THREE_LEAF_POWER_H

/**
 * @file three_leaf_power.h
 * @brief 3-leaf power graph recognition
 *
 * A graph G is a 3-leaf power if there exists a tree T whose leaves are the vertices of G,
 * and leaves u, v are adjacent in G if and only if d(u,v) <= 3 in T.
 *
 * Characterization (Brandstaedt & Le, 2006):
 *   G is a 3-leaf power <=> G is a (bull, dart, gem)-free chordal graph
 *                       <=> the critical clique graph of G is a forest
 *
 * Algorithm (critical cliques are grouped by sorting closed neighborhoods,
 * so the running time is dominated by O(sum deg * log) sorting, not linear):
 *   1. Check if the graph is chordal
 *   2. Compute critical cliques (maximal true twin classes)
 *   3. Build the critical clique graph
 *   4. Check if the critical clique graph is a forest
 *   5. Verify that edges between adjacent critical cliques form complete bipartite graphs
 *
 * References:
 *   - Brandstaedt & Le (2006). Structure and linear time recognition
 *     of 3-leaf powers. IPL 98:133-138.
 *   - Dom, Guo, Hüffner, Niedermeier (2006). Error compensation in
 *     leaf power problems. Algorithmica 44(4):363-381.
 *   - Nishimura, Ragde, Thilikos (2002). On graph powers for
 *     leaf-labeled trees. J. Algorithms 42(1):69-108.
 */

#include "chordal.h"
#include "components.h"
#include "graph.h"
#include "twins.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

struct ThreeLeafPowerResult {
    bool is_three_leaf_power = false;
};

namespace detail {

/**
 * @brief 3-leaf power recognition: determined by whether the critical clique graph is a forest
 *
 * Steps:
 *   1. Chordal graph check
 *   2. Compute critical cliques (maximal sets of vertices with identical closed neighborhoods)
 *   3. Build critical clique graph + forest check
 *   4. Verify that edges between adjacent critical cliques form complete bipartite subgraphs
 */
inline ThreeLeafPowerResult check_three_leaf_power_impl(const Graph& g) {
    ThreeLeafPowerResult res;
    res.is_three_leaf_power = false;

    if (g.n == 0) { res.is_three_leaf_power = true; return res; }

    // 1. Chordal graph check
    ChordalResult cr = check_chordal(g);
    if (!cr.is_chordal) return res;

    // 2./3. Critical cliques and the critical clique graph
    TwinQuotientResult cq = critical_clique_quotient(g);
    const Graph& cc_adj = cq.quotient;
    int num_cc = cc_adj.n;

    // 4. Determine if the critical clique graph is a forest (set of trees)
    // Forest <=> |E| == |V| - (number of connected components)
    int edge_count = 0;
    for (int ci = 1; ci <= num_cc; ++ci) {
        edge_count += (int)cc_adj.adj[ci].size();
    }
    edge_count /= 2;

    int components = connected_components(cc_adj).count;

    if (edge_count != num_cc - components) return res;

    // 5. Verify that edges between adjacent critical cliques form complete bipartite graphs
    // If clique ci (size a) and clique cj (size b) are adjacent in cc_adj,
    // all vertices of ci must be adjacent to all vertices of cj in G.
    //
    // degree of representative vertex rep - (own clique size - 1) = number of vertices in adjacent cliques
    // Meanwhile, the sum of sizes of adjacent cliques in cc_adj should match.

    for (int ci = 1; ci <= num_cc; ++ci) {
        int rep = cq.members[ci][0];
        int my_size = (int)cq.members[ci].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);

        int expected = 0;
        for (size_t k = 0; k < cc_adj.adj[ci].size(); ++k) {
            expected += (int)cq.members[cc_adj.adj[ci][k]].size();
        }

        if (external_edges != expected) return res;
    }

    res.is_three_leaf_power = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a 3-leaf power
 * @param g Input graph
 * @return ThreeLeafPowerResult
 */
inline ThreeLeafPowerResult check_three_leaf_power(const Graph& g) {
    return detail::check_three_leaf_power_impl(g);
}

} // namespace graph_recognition

#endif
