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
#include "graph.h"
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

    // 2. Critical clique computation
    // Groups vertices with identical closed neighborhoods N[v] into the same critical clique
    // by sorting the closed neighborhood lists and comparing them directly.

    // Create sorted closed neighborhood list for each vertex
    std::vector<std::vector<int>> closed_nbr(g.n + 1);
    for (int v = 1; v <= g.n; ++v) {
        closed_nbr[v] = g.adj[v];
        closed_nbr[v].push_back(v);
        std::sort(closed_nbr[v].begin(), closed_nbr[v].end());
    }

    // Group by closed neighborhood -> critical clique
    // Sort lexicographically and group
    std::vector<int> order(g.n);
    for (int i = 0; i < g.n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
              [&closed_nbr](int a, int b) {
                  return closed_nbr[a] < closed_nbr[b];
              });

    // cc_id[v] = ID of the critical clique that vertex v belongs to (0-indexed)
    std::vector<int> cc_id(g.n + 1, -1);
    int num_cc = 0;
    std::vector<std::vector<int>> cc_members; // cc_members[i] = members of clique i

    for (int i = 0; i < g.n; ) {
        int j = i;
        while (j < g.n && closed_nbr[order[j]] == closed_nbr[order[i]]) {
            cc_id[order[j]] = num_cc;
            ++j;
        }
        cc_members.push_back(std::vector<int>(order.begin() + i, order.begin() + j));
        num_cc++;
        i = j;
    }

    // 3. Build the critical clique graph
    // cc_adj[i] = set of cliques adjacent to clique i
    // Edges are based on edges of G: u, v adjacent and cc_id[u] != cc_id[v]
    std::vector<std::vector<int>> cc_adj(num_cc);
    // Record each cc pair only once to avoid duplicates
    std::vector<int> seen(num_cc, -1); // seen[j] = i means clique j was already added from clique i

    for (int ci = 0; ci < num_cc; ++ci) {
        int rep = cc_members[ci][0]; // representative vertex
        for (size_t k = 0; k < g.adj[rep].size(); ++k) {
            int w = g.adj[rep][k];
            int cj = cc_id[w];
            if (cj > ci && seen[cj] != ci) {
                seen[cj] = ci;
                cc_adj[ci].push_back(cj);
                cc_adj[cj].push_back(ci);
            }
        }
    }

    // 4. Determine if the critical clique graph is a forest (set of trees)
    // Forest <=> |E| == |V| - (number of connected components)
    int edge_count = 0;
    for (int ci = 0; ci < num_cc; ++ci) {
        edge_count += (int)cc_adj[ci].size();
    }
    edge_count /= 2;

    // Compute connected component count via BFS
    std::vector<int> visited(num_cc, 0);
    int components = 0;
    for (int start = 0; start < num_cc; ++start) {
        if (visited[start]) continue;
        ++components;
        std::vector<int> queue;
        queue.push_back(start);
        visited[start] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (size_t k = 0; k < cc_adj[u].size(); ++k) {
                int v = cc_adj[u][k];
                if (!visited[v]) {
                    visited[v] = 1;
                    queue.push_back(v);
                }
            }
        }
    }

    if (edge_count != num_cc - components) return res;

    // 5. Verify that edges between adjacent critical cliques form complete bipartite graphs
    // If clique ci (size a) and clique cj (size b) are adjacent in cc_adj,
    // all vertices of ci must be adjacent to all vertices of cj in G.
    //
    // degree of representative vertex rep - (own clique size - 1) = number of vertices in adjacent cliques
    // Meanwhile, the sum of sizes of adjacent cliques in cc_adj should match.

    for (int ci = 0; ci < num_cc; ++ci) {
        int rep = cc_members[ci][0];
        int my_size = (int)cc_members[ci].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);

        int expected = 0;
        for (size_t k = 0; k < cc_adj[ci].size(); ++k) {
            expected += (int)cc_members[cc_adj[ci][k]].size();
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
