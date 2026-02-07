#ifndef GRAPH_RECOGNITION_CHORDAL_BIPARTITE_H
#define GRAPH_RECOGNITION_CHORDAL_BIPARTITE_H

#include "bipartite.h"
#include "graph.h"
#include <climits>
#include <queue>
#include <vector>

namespace graph_recognition {

// Result of chordal bipartite graph recognition.
struct ChordalBipartiteResult {
    bool is_chordal_bipartite;
    // color from bipartite check when is_chordal_bipartite == true.
    std::vector<int> color;
};

namespace detail {

inline bool has_induced_even_cycle_ge6(
    const Graph& g,
    const std::vector<int>& color) {
    int n = g.n;
    std::vector<int> blocked_stamp(n + 1, 0);
    std::vector<int> seen(n + 1, 0), dist(n + 1, 0);
    int blocked_token = 0, seen_token = 0;

    // Check each edge u-v (u in color 0, v in color 1) as a candidate cycle edge.
    for (int u = 1; u <= n; ++u) {
        if (color[u] != 0) continue;
        if (g.adj[u].size() < 2) continue;

        for (size_t iv = 0; iv < g.adj[u].size(); ++iv) {
            int v = g.adj[u][iv];
            if (g.adj[v].size() < 2) continue;

            // Block N(u) union N(v) and {u, v} as internal path vertices.
            if (blocked_token == INT_MAX) {
                std::fill(blocked_stamp.begin(), blocked_stamp.end(), 0);
                blocked_token = 0;
            }
            blocked_token++;
            blocked_stamp[u] = blocked_token;
            blocked_stamp[v] = blocked_token;
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                blocked_stamp[g.adj[u][i]] = blocked_token;
            }
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                blocked_stamp[g.adj[v][i]] = blocked_token;
            }

            // Pick endpoints x in N(u)\{v}, y in N(v)\{u}.
            for (size_t ix = 0; ix < g.adj[u].size(); ++ix) {
                int x = g.adj[u][ix];
                if (x == v) continue;
                for (size_t iy = 0; iy < g.adj[v].size(); ++iy) {
                    int y = g.adj[v][iy];
                    if (y == u) continue;

                    if (seen_token == INT_MAX) {
                        std::fill(seen.begin(), seen.end(), 0);
                        seen_token = 0;
                    }
                    seen_token++;

                    std::queue<int> q;
                    seen[x] = seen_token;
                    dist[x] = 0;
                    q.push(x);

                    while (!q.empty() && seen[y] != seen_token) {
                        int cur = q.front();
                        q.pop();
                        for (size_t in = 0; in < g.adj[cur].size(); ++in) {
                            int nxt = g.adj[cur][in];
                            if (seen[nxt] == seen_token) continue;
                            if (blocked_stamp[nxt] == blocked_token &&
                                nxt != x && nxt != y) {
                                continue;
                            }
                            seen[nxt] = seen_token;
                            dist[nxt] = dist[cur] + 1;
                            q.push(nxt);
                        }
                    }

                    // x-y shortest path length >= 3 gives an induced cycle
                    // u-x-...-y-v-u of length at least 6.
                    if (seen[y] == seen_token && dist[y] >= 3) return true;
                }
            }
        }
    }
    return false;
}

} // namespace detail

// Check whether a graph is chordal bipartite.
// A graph is chordal bipartite iff it is bipartite and has no induced even
// cycle of length at least 6.
inline ChordalBipartiteResult check_chordal_bipartite(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    if (detail::has_induced_even_cycle_ge6(g, bip.color)) return res;

    res.is_chordal_bipartite = true;
    res.color = bip.color;
    return res;
}

} // namespace graph_recognition

#endif
