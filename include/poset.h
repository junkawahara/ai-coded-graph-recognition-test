#ifndef GRAPH_RECOGNITION_POSET_H
#define GRAPH_RECOGNITION_POSET_H

/**
 * @file poset.h
 * @brief Poset recognition -- Hasse diagram validation
 *
 * Determines whether the input directed graph is a valid Hasse diagram (covering relation).
 * Conditions: DAG (acyclic) and transitive reduction.
 *
 * Input format: n m (number of vertices, number of arcs) followed by m arcs u v (covering relation u <_P v)
 */

#include <iostream>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for poset recognition
 */
enum class PosetAlgorithm {
    DAG_AND_REDUCTION /**< DAG check + transitive reduction check */
};

/**
 * @brief Result of poset recognition
 */
struct PosetResult {
    bool is_poset = false; /**< true if it is a valid Hasse diagram */
};

/**
 * @brief Determines whether the directed graph is a Hasse diagram
 * @param n Number of vertices
 * @param arcs Arc list (u, v) = u covers v (u <_P v)
 * @param algo Algorithm to use
 * @return PosetResult
 *
 * Hasse diagram <=> DAG and transitive reduction (for any arc u->v,
 * no directed path of length >= 2 from u to v exists).
 */
inline PosetResult check_poset(int n,
    const std::vector<std::pair<int, int>>& arcs,
    PosetAlgorithm algo = PosetAlgorithm::DAG_AND_REDUCTION) {
    (void)algo;
    PosetResult res;

    if (n < 0) return res;
    if (n == 0) {
        res.is_poset = arcs.empty();
        return res;
    }

    /* Build adjacency list + input validation */
    std::vector<std::vector<int>> adj_out(n + 1);
    std::set<std::pair<int, int>> arc_set;
    std::vector<int> in_deg(n + 1, 0);

    for (size_t i = 0; i < arcs.size(); ++i) {
        int u = arcs[i].first, v = arcs[i].second;
        if (u < 1 || u > n || v < 1 || v > n) return res;
        if (u == v) return res; /* Self-loop */
        if (!arc_set.insert(std::make_pair(u, v)).second) return res; /* Duplicate arc */
        adj_out[u].push_back(v);
        in_deg[v]++;
    }

    /* DAG check (topological sort) */
    std::vector<int> topo;
    std::vector<int> queue;
    for (int v = 1; v <= n; ++v) {
        if (in_deg[v] == 0) queue.push_back(v);
    }
    std::vector<int> tmp_in(in_deg);
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int v = queue[qi];
        topo.push_back(v);
        for (size_t i = 0; i < adj_out[v].size(); ++i) {
            int u = adj_out[v][i];
            if (--tmp_in[u] == 0) queue.push_back(u);
        }
    }
    if ((int)topo.size() != n) return res; /* Cycle detected */

    /* Transitive reduction check: for each arc u->v,
       verify that no path of length >= 2 from u to v exists.
       If v is reachable from any child w (w != v) of u, then u->v is redundant. */
    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < adj_out[u].size(); ++ei) {
            int v = adj_out[u][ei];

            /* Is v reachable from children of u (other than v) via BFS? */
            std::vector<char> reachable(n + 1, 0);
            std::vector<int> bfs;
            for (size_t j = 0; j < adj_out[u].size(); ++j) {
                int w = adj_out[u][j];
                if (w != v && !reachable[w]) {
                    reachable[w] = 1;
                    bfs.push_back(w);
                }
            }
            for (size_t qi = 0; qi < bfs.size(); ++qi) {
                int w = bfs[qi];
                if (w == v) {
                    return res; /* Redundant arc -> not a transitive reduction */
                }
                for (size_t j = 0; j < adj_out[w].size(); ++j) {
                    int x = adj_out[w][j];
                    if (!reachable[x]) {
                        reachable[x] = 1;
                        bfs.push_back(x);
                    }
                }
            }
            if (reachable[v]) return res;
        }
    }

    res.is_poset = true;
    return res;
}

} // namespace graph_recognition

#endif
