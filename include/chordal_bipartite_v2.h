#ifndef GRAPH_RECOGNITION_CHORDAL_BIPARTITE_V2_H
#define GRAPH_RECOGNITION_CHORDAL_BIPARTITE_V2_H

#include "bipartite.h"
#include "chordal_bipartite.h"
#include "graph.h"
#include <vector>

namespace graph_recognition {

// Check whether a graph is chordal bipartite (improved version).
// Algorithm: bisimplicial edge elimination (one edge at a time).
//   A bisimplicial edge (x,y) is one where N(y) x N(x) forms a complete
//   bipartite subgraph.  A bisimplicial edge cannot be part of any induced
//   C_{2k} (k>=3), so removing it preserves any forbidden cycle.
//   Therefore: repeatedly remove any single bisimplicial edge. If we can
//   eliminate all edges, the graph is chordal bipartite; if we get stuck
//   (edges remain but no bisimplicial edge), it is not.
inline ChordalBipartiteResult check_chordal_bipartite_v2(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;

    // Build adjacency matrix.
    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    int edge_count = 0;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) {
                adj[u][v] = 1;
                adj[v][u] = 1;
                edge_count++;
            }
        }
    }

    // Iteratively find and remove a single bisimplicial edge.
    while (edge_count > 0) {
        bool found = false;

        for (int u = 1; u <= n && !found; ++u) {
            for (int v = u + 1; v <= n && !found; ++v) {
                if (!adj[u][v]) continue;
                // Check if (u,v) is bisimplicial:
                // N(v) x N(u) must be complete bipartite.
                // Collect N(v) and N(u).
                bool bisimplicial = true;
                for (int a = 1; a <= n && bisimplicial; ++a) {
                    if (!adj[v][a]) continue; // a in N(v)
                    for (int b = 1; b <= n && bisimplicial; ++b) {
                        if (!adj[u][b]) continue; // b in N(u)
                        if (!adj[a][b]) bisimplicial = false;
                    }
                }

                if (bisimplicial) {
                    found = true;
                    adj[u][v] = 0;
                    adj[v][u] = 0;
                    edge_count--;
                }
            }
        }

        if (!found) return res;
    }

    res.is_chordal_bipartite = true;
    res.color = bip.color;
    return res;
}

} // namespace graph_recognition

#endif
