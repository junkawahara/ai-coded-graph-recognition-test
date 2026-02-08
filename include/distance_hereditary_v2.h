#ifndef GRAPH_RECOGNITION_DISTANCE_HEREDITARY_V2_H
#define GRAPH_RECOGNITION_DISTANCE_HEREDITARY_V2_H

#include "distance_hereditary.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

// Check whether a graph is distance-hereditary.
// Improved version: uses sorted neighbor list comparison instead of string
// signatures for twin detection.  Avoids hash-map overhead and string
// allocation.
inline DistanceHereditaryResult check_distance_hereditary_v2(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = true;
    int n = g.n;
    if (n <= 1) return res;

    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    std::vector<int> degree(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) continue;
            adj[u][v] = 1;
            adj[v][u] = 1;
            degree[u]++;
            degree[v]++;
        }
    }

    std::vector<unsigned char> alive(n + 1, 1);
    int remaining = n;

    while (remaining > 1) {
        int pick = 0;

        // Pendant (or isolated) vertex elimination.
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] <= 1) {
                pick = v;
                break;
            }
        }

        if (pick == 0) {
            // Twin elimination using sorted neighbor lists.
            // Build sorted neighbor list for each alive vertex.
            // Open neighbors (excluding self) and closed neighbors (including self)
            // are compared to find twins.
            std::vector<int> verts;
            verts.reserve(remaining);
            for (int v = 1; v <= n; ++v) {
                if (alive[v]) verts.push_back(v);
            }

            // Compute sorted open-neighbor list for each vertex.
            std::vector<std::vector<int>> open_nb(n + 1);
            for (size_t i = 0; i < verts.size(); ++i) {
                int v = verts[i];
                open_nb[v].reserve(degree[v]);
                for (size_t j = 0; j < verts.size(); ++j) {
                    int u = verts[j];
                    if (u != v && adj[v][u]) {
                        open_nb[v].push_back(u);
                    }
                }
                // Already sorted since verts is sorted.
            }

            // Sort vertices by open-neighbor list to find false twins.
            std::vector<int> by_open(verts);
            std::sort(by_open.begin(), by_open.end(),
                      [&](int a, int b) { return open_nb[a] < open_nb[b]; });
            for (size_t i = 1; i < by_open.size(); ++i) {
                if (open_nb[by_open[i]] == open_nb[by_open[i - 1]]) {
                    pick = by_open[i];
                    break;
                }
            }

            if (pick == 0) {
                // Check for true twins: closed neighborhood = open + self.
                // Two vertices u, v are true twins if N[u] = N[v],
                // i.e., they are adjacent and N(u)\{v} = N(v)\{u}.
                // Equivalently, sort by closed-neighbor list.
                std::vector<std::vector<int>> closed_nb(n + 1);
                for (size_t i = 0; i < verts.size(); ++i) {
                    int v = verts[i];
                    closed_nb[v] = open_nb[v];
                    // Insert v in sorted position.
                    std::vector<int>::iterator it =
                        std::lower_bound(closed_nb[v].begin(), closed_nb[v].end(), v);
                    closed_nb[v].insert(it, v);
                }

                std::vector<int> by_closed(verts);
                std::sort(by_closed.begin(), by_closed.end(),
                          [&](int a, int b) { return closed_nb[a] < closed_nb[b]; });
                for (size_t i = 1; i < by_closed.size(); ++i) {
                    if (closed_nb[by_closed[i]] == closed_nb[by_closed[i - 1]]) {
                        pick = by_closed[i];
                        break;
                    }
                }
            }
        }

        if (pick == 0) {
            res.is_distance_hereditary = false;
            return res;
        }

        alive[pick] = 0;
        remaining--;
        for (int u = 1; u <= n; ++u) {
            if (!alive[u]) continue;
            if (adj[pick][u]) degree[u]--;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
