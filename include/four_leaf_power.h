#ifndef GRAPH_RECOGNITION_FOUR_LEAF_POWER_H
#define GRAPH_RECOGNITION_FOUR_LEAF_POWER_H

/**
 * @file four_leaf_power.h
 * @brief 4-leaf power graph recognition
 *
 * A graph G is a 4-leaf power if there exists a tree T whose leaves are
 * the vertices of G, where leaves u, v are adjacent in G iff d(u,v) <= 4 in T.
 *
 * Algorithms:
 *   1. Check if strongly chordal (necessary condition)
 *   2. Compute critical cliques (maximal sets of vertices with identical closed neighborhoods)
 *   3. Build quotient graph Q (verify edges between CCs are all-or-nothing)
 *   4. Check subdivision feasibility for all labeled trees on k nodes
 *      d'(i,j) = d(i,j) + Σ s_e <= 2 (yes pairs), >= 3 (no pairs)
 *
 * References:
 *   - Brandstädt, Le, Sritharan (2008). Structure and linear-time
 *     recognition of 4-leaf powers. ACM Trans. Algorithms 4(1).
 */

#include "graph.h"
#include "strongly_chordal.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

struct FourLeafPowerResult {
    bool is_four_leaf_power = false;
};

namespace detail_four_leaf_power {

/**
 * @brief Generate edge list of a labeled tree from a Prufer sequence (length k-2)
 * @param seq Prufer sequence (0-indexed, each element 0..k-1)
 * @param k Number of nodes
 * @param edges Output edge list
 */
inline void prufer_decode(const std::vector<int>& seq, int k,
                          std::vector<std::pair<int, int>>& edges) {
    edges.clear();
    if (k <= 1) return;
    if (k == 2) { edges.push_back(std::make_pair(0, 1)); return; }

    std::vector<int> degree(k, 1);
    for (int i = 0; i < (int)seq.size(); ++i) degree[seq[i]]++;

    for (int i = 0; i < (int)seq.size(); ++i) {
        int x = seq[i];
        for (int leaf = 0; leaf < k; ++leaf) {
            if (degree[leaf] == 1) {
                edges.push_back(std::make_pair(leaf, x));
                degree[leaf]--;
                degree[x]--;
                break;
            }
        }
    }
    int a = -1, b = -1;
    for (int i = 0; i < k; ++i) {
        if (degree[i] == 1) {
            if (a < 0) a = i; else b = i;
        }
    }
    edges.push_back(std::make_pair(a, b));
}

/**
 * @brief Compute all-pairs distances and edge indices on each pair's path in a tree via BFS
 */
inline void tree_distances_and_paths(
    const std::vector<std::pair<int, int>>& edges, int k,
    std::vector<std::vector<int>>& dist,
    std::vector<std::vector<std::vector<int>>>& paths) {

    // Adjacency list (node, edge index)
    std::vector<std::vector<std::pair<int, int>>> adj(k);
    for (int i = 0; i < (int)edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u].push_back(std::make_pair(v, i));
        adj[v].push_back(std::make_pair(u, i));
    }

    dist.assign(k, std::vector<int>(k, 0));
    paths.assign(k, std::vector<std::vector<int>>(k));

    std::vector<int> parent_edge(k);
    std::vector<int> parent_node(k);
    std::vector<char> visited(k);

    for (int s = 0; s < k; ++s) {
        for (int i = 0; i < k; ++i) { visited[i] = 0; parent_edge[i] = -1; }
        visited[s] = 1;
        std::vector<int> queue;
        queue.push_back(s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (size_t ei = 0; ei < adj[u].size(); ++ei) {
                int v = adj[u][ei].first;
                int eidx = adj[u][ei].second;
                if (!visited[v]) {
                    visited[v] = 1;
                    dist[s][v] = dist[s][u] + 1;
                    parent_edge[v] = eidx;
                    parent_node[v] = u;
                    queue.push_back(v);
                }
            }
        }
        // Path reconstruction
        for (int t = 0; t < k; ++t) {
            if (t == s) continue;
            paths[s][t].clear();
            int cur = t;
            while (cur != s) {
                paths[s][t].push_back(parent_edge[cur]);
                cur = parent_node[cur];
            }
        }
    }
}

/**
 * @brief Determines whether quotient graph Q is realizable by adding edge subdivisions to tree T (k nodes)
 *
 * Subdivision variables s_e >= 0 yield new distances d'(i,j) = d(i,j) + Σ_{e on path} s_e.
 * Q[i][j]=true => d'(i,j) <= 2, Q[i][j]=false => d'(i,j) >= 3
 */
inline bool check_subdivision_feasibility(
    const std::vector<std::pair<int, int>>& tree_edges, int k,
    const std::vector<std::vector<char>>& Q) {

    if (k <= 1) return true;

    std::vector<std::vector<int>> dist;
    std::vector<std::vector<std::vector<int>>> paths;
    tree_distances_and_paths(tree_edges, k, dist, paths);

    int num_edges = (int)tree_edges.size();

    // Base distance check for "yes" pairs
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (Q[i][j] && dist[i][j] > 2) return false;
        }
    }

    // Compute upper bound for each edge
    std::vector<int> upper(num_edges, 2);

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (!Q[i][j]) continue;
            const std::vector<int>& path = paths[i][j];
            int slack = 2 - dist[i][j];
            if (slack == 0) {
                for (size_t p = 0; p < path.size(); ++p)
                    upper[path[p]] = 0;
            } else {
                // slack = 1, path has 1 edge (since dist=1)
                for (size_t p = 0; p < path.size(); ++p)
                    if (upper[path[p]] > slack) upper[path[p]] = slack;
            }
        }
    }

    // "no" constraint check
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (Q[i][j]) continue;
            const std::vector<int>& path = paths[i][j];
            int needed = 3 - dist[i][j];
            if (needed <= 0) continue;
            int available = 0;
            for (size_t p = 0; p < path.size(); ++p)
                available += upper[path[p]];
            if (available < needed) return false;
        }
    }

    return true;
}

/**
 * @brief Implementation of 4-leaf power recognition
 */
inline FourLeafPowerResult check_four_leaf_power_impl(const Graph& g) {
    FourLeafPowerResult res;
    res.is_four_leaf_power = false;

    if (g.n == 0) { res.is_four_leaf_power = true; return res; }

    // 1. Strongly chordal check (4-leaf power is a subclass of strongly chordal)
    StronglyChordalResult scr = check_strongly_chordal(g);
    if (!scr.is_strongly_chordal) return res;

    // 2. Compute critical cliques
    std::vector<std::vector<int>> closed_nbr(g.n + 1);
    for (int v = 1; v <= g.n; ++v) {
        closed_nbr[v] = g.adj[v];
        closed_nbr[v].push_back(v);
        std::sort(closed_nbr[v].begin(), closed_nbr[v].end());
    }

    std::vector<int> order(g.n);
    for (int i = 0; i < g.n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
              [&closed_nbr](int a, int b) {
                  return closed_nbr[a] < closed_nbr[b];
              });

    std::vector<int> cc_id(g.n + 1, -1);
    int num_cc = 0;
    std::vector<std::vector<int>> cc_members;

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

    int k = num_cc;

    // 3. Build quotient graph Q
    std::vector<std::vector<char>> Q(k, std::vector<char>(k, 0));
    for (int i = 0; i < k; ++i) Q[i][i] = 1;

    std::vector<int> seen(k, -1);
    for (int ci = 0; ci < k; ++ci) {
        int rep = cc_members[ci][0];
        for (size_t ei = 0; ei < g.adj[rep].size(); ++ei) {
            int w = g.adj[rep][ei];
            int cj = cc_id[w];
            if (cj != ci && seen[cj] != ci) {
                seen[cj] = ci;
                Q[ci][cj] = 1;
                Q[cj][ci] = 1;
            }
        }
    }

    // Consistency check: if Q[ci][cj]=1 then all pairs are adjacent
    for (int ci = 0; ci < k; ++ci) {
        int rep = cc_members[ci][0];
        int my_size = (int)cc_members[ci].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);
        int expected = 0;
        for (int cj = 0; cj < k; ++cj) {
            if (cj != ci && Q[ci][cj])
                expected += (int)cc_members[cj].size();
        }
        if (external_edges != expected) return res;
    }

    // k=1: complete graph -> always a 4-leaf power
    if (k == 1) { res.is_four_leaf_power = true; return res; }

    // 4. Try all labeled trees and check subdivision feasibility
    if (k == 2) {
        // k=2: unique tree (edge 0-1)
        std::vector<std::pair<int, int>> te;
        te.push_back(std::make_pair(0, 1));
        if (check_subdivision_feasibility(te, 2, Q)) {
            res.is_four_leaf_power = true;
        }
        return res;
    }

    // k >= 3: enumerate all trees via Prufer sequences
    int seq_len = k - 2;
    std::vector<int> seq(seq_len, 0);
    std::vector<std::pair<int, int>> te;

    while (true) {
        prufer_decode(seq, k, te);
        if (check_subdivision_feasibility(te, k, Q)) {
            res.is_four_leaf_power = true;
            return res;
        }

        // Next Prufer sequence
        int carry = seq_len - 1;
        while (carry >= 0) {
            seq[carry]++;
            if (seq[carry] < k) break;
            seq[carry] = 0;
            --carry;
        }
        if (carry < 0) break;
    }

    return res;
}

} // namespace detail_four_leaf_power

/**
 * @brief Determines whether the graph is a 4-leaf power
 * @param g Input graph
 * @return FourLeafPowerResult
 */
inline FourLeafPowerResult check_four_leaf_power(const Graph& g) {
    return detail_four_leaf_power::check_four_leaf_power_impl(g);
}

} // namespace graph_recognition

#endif
