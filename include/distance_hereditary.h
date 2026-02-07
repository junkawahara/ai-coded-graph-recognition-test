#ifndef GRAPH_RECOGNITION_DISTANCE_HEREDITARY_H
#define GRAPH_RECOGNITION_DISTANCE_HEREDITARY_H

#include "graph.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace graph_recognition {

// Result of distance-hereditary graph recognition.
struct DistanceHereditaryResult {
    bool is_distance_hereditary;
};

// Check whether a graph is distance-hereditary.
// Characterization used:
//   Repeatedly removing a pendant vertex (degree 0/1) or one of a twin pair
//   (false twins or true twins) reduces the graph to one vertex.
inline DistanceHereditaryResult check_distance_hereditary(const Graph& g) {
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
            // Twin elimination.
            std::vector<int> verts;
            verts.reserve(remaining);
            for (int v = 1; v <= n; ++v) {
                if (alive[v]) verts.push_back(v);
            }

            std::unordered_map<std::string, int> open_seen;
            std::unordered_map<std::string, int> closed_seen;
            open_seen.reserve(verts.size() * 2 + 1);
            closed_seen.reserve(verts.size() * 2 + 1);

            for (size_t i = 0; i < verts.size() && pick == 0; ++i) {
                int v = verts[i];
                std::string open_sig;
                std::string closed_sig;
                open_sig.reserve(verts.size());
                closed_sig.reserve(verts.size());

                for (size_t j = 0; j < verts.size(); ++j) {
                    int u = verts[j];
                    if (u == v) {
                        open_sig.push_back('0');
                        closed_sig.push_back('1');
                    } else if (adj[v][u]) {
                        open_sig.push_back('1');
                        closed_sig.push_back('1');
                    } else {
                        open_sig.push_back('0');
                        closed_sig.push_back('0');
                    }
                }

                if (open_seen.find(open_sig) != open_seen.end()) {
                    pick = v;
                    break;
                }
                open_seen.insert(std::make_pair(open_sig, v));

                if (closed_seen.find(closed_sig) != closed_seen.end()) {
                    pick = v;
                    break;
                }
                closed_seen.insert(std::make_pair(closed_sig, v));
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
