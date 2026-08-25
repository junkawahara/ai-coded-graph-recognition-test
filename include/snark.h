#ifndef GRAPH_RECOGNITION_SNARK_H
#define GRAPH_RECOGNITION_SNARK_H

/**
 * @file snark.h
 * @brief Snark recognition
 *
 * A snark is a cubic graph satisfying:
 *   - Bridgeless (2-edge-connected)
 *   - Girth >= 5
 *   - Cyclically 4-edge-connected
 *   - Chromatic index 4 (not 3-edge-colorable)
 */

#include "block_cut_tree.h"
#include "graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for snark recognition
 */
enum class SnarkAlgorithm {
    COMBINED /**< Combined check */
};

/**
 * @brief Result of snark recognition
 */
struct SnarkResult {
    bool is_snark = false; /**< true if the graph is a snark */
};

namespace detail {

/**
 * @brief Bridge detection
 * @return true if a bridge exists
 *
 * A bridge is exactly a block consisting of a single edge, so the shared
 * block decomposition answers this directly.
 */
inline bool has_bridge(const Graph& g) {
    return !compute_block_cut_tree(g).bridges.empty();
}

/**
 * @brief Compute the girth (length of shortest cycle)
 * @return girth (n+1 if no cycle exists)
 */
inline int compute_girth(const Graph& g) {
    int n = g.n;
    int girth = n + 1;

    for (int s = 1; s <= n; ++s) {
        /* Search for shortest cycle from s via BFS */
        std::vector<int> dist(n + 1, -1);
        std::vector<int> bfs;
        dist[s] = 0;
        bfs.push_back(s);

        for (size_t qi = 0; qi < bfs.size(); ++qi) {
            int v = bfs[qi];
            if (dist[v] >= girth / 2) break; /* No further improvement possible */
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    bfs.push_back(u);
                } else if (dist[u] >= dist[v]) {
                    /* Cycle found */
                    int len = dist[v] + dist[u] + 1;
                    if (len < girth) girth = len;
                }
            }
        }
    }
    return girth;
}

/**
 * @brief Cyclically 4-edge-connected check
 *
 * Verifies 3-edge-connectivity and that all 3-edge cuts are trivial (star of one vertex).
 */
inline bool is_cyclically_4_edge_connected(const Graph& g) {
    int n = g.n;
    if (n < 4) return false;

    /* 3-edge-connected check: remove each edge and check for bridges */
    /* If a bridge exists, a 2-edge cut exists -> not 3-edge-connected */
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (v <= u) continue;
            /* Is there a bridge after removing (u,v)? */
            /* Simple check: bridge detection via DFS */
            std::vector<int> disc(n + 1, -1), low(n + 1, 0);
            int timer = 0;
            bool bridge_found = false;

            struct DFS { int v, par, idx; };
            std::vector<DFS> stk;
            /* Start the bridge DFS from any vertex other than the edge
               endpoints; n >= 4 here guarantees such a vertex exists. */
            int start = 1;
            while (start == u || start == v) ++start;
            disc[start] = low[start] = timer++;
            stk.push_back({start, 0, 0});

            while (!stk.empty() && !bridge_found) {
                DFS& st = stk.back();
                if (st.idx < (int)g.adj[st.v].size()) {
                    int w = g.adj[st.v][st.idx++];
                    /* Skip edge */
                    if ((st.v == u && w == v) || (st.v == v && w == u)) continue;
                    if (disc[w] == -1) {
                        disc[w] = low[w] = timer++;
                        stk.push_back({w, st.v, 0});
                    } else if (w != st.par) {
                        if (disc[w] < low[st.v]) low[st.v] = disc[w];
                    }
                } else {
                    int cv = st.v, cp = st.par;
                    stk.pop_back();
                    if (!stk.empty()) {
                        if (low[cv] < low[stk.back().v])
                            low[stk.back().v] = low[cv];
                        if (low[cv] > disc[cp]) bridge_found = true;
                    }
                }
            }
            if (bridge_found) return false;
        }
    }

    /* Check for non-trivial 3-edge cuts. The graph is 3-edge-connected at
       this point, so for any 3-edge cut {e1, e2, e3}: G - e1 - e2 remains
       connected and e3 is a bridge of it. Conversely, a bridge e3 of
       G - e1 - e2 yields an edge cut contained in {e1, e2, e3}, which by
       3-edge-connectivity uses all three edges. In a cubic graph a cut
       side S with |S| >= 2 crosses >= 4 edges when G[S] is a forest
       (3|S| - 2(|S|-1) >= 4), so a side of a 3-edge cut with |S| >= 2
       contains a cycle: the cut is non-trivial iff both sides have >= 2
       vertices. */
    std::vector<std::pair<int, int> > edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) edges.push_back(std::make_pair(u, v));
        }
    }
    int m = (int)edges.size();

    for (int a = 0; a < m; ++a) {
        for (int b = a + 1; b < m; ++b) {
            /* Bridge DFS on G - edges[a] - edges[b] with subtree sizes */
            std::vector<int> disc(n + 1, -1), low(n + 1, 0), sz(n + 1, 1);
            int timer = 0;

            struct DFS { int v, par, idx; };
            std::vector<DFS> stk;
            disc[1] = low[1] = timer++;
            stk.push_back({1, 0, 0});

            while (!stk.empty()) {
                DFS& st = stk.back();
                if (st.idx < (int)g.adj[st.v].size()) {
                    int w = g.adj[st.v][st.idx++];
                    /* Skip the two removed edges */
                    int lo = st.v < w ? st.v : w;
                    int hi = st.v < w ? w : st.v;
                    if ((lo == edges[a].first && hi == edges[a].second) ||
                        (lo == edges[b].first && hi == edges[b].second))
                        continue;
                    if (disc[w] == -1) {
                        disc[w] = low[w] = timer++;
                        stk.push_back({w, st.v, 0});
                    } else if (w != st.par) {
                        if (disc[w] < low[st.v]) low[st.v] = disc[w];
                    }
                } else {
                    int cv = st.v, cp = st.par;
                    stk.pop_back();
                    if (!stk.empty()) {
                        sz[cp] += sz[cv];
                        if (low[cv] < low[cp]) low[cp] = low[cv];
                        if (low[cv] > disc[cp]) {
                            /* Bridge (cp, cv): cut {edges[a], edges[b],
                               (cp,cv)} with sides sz[cv] and n - sz[cv] */
                            if (sz[cv] >= 2 && n - sz[cv] >= 2) return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * @brief Determines whether 3-edge-coloring is possible (backtracking)
 * @return true if 3-edge-colorable
 */
inline bool is_3_edge_colorable(const Graph& g) {
    int n = g.n;
    /* Build edge list */
    struct UEdge { int u, v; };
    std::vector<UEdge> edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) edges.push_back({u, v});
        }
    }
    int m = (int)edges.size();
    if (m == 0) return true;

    /* Incident edge indices for each vertex */
    std::vector<std::vector<int>> inc(n + 1);
    for (int i = 0; i < m; ++i) {
        inc[edges[i].u].push_back(i);
        inc[edges[i].v].push_back(i);
    }

    std::vector<int> color(m, 0); /* 0 = uncolored, 1/2/3 = color */

    /* Manage used colors per vertex using bitmasks */
    std::vector<int> used_mask(n + 1, 0);

    /* Backtracking */
    struct State {
        std::vector<UEdge>& edges;
        std::vector<int>& color;
        std::vector<int>& used_mask;
        std::vector<std::vector<int>>& inc;
        int m;

        bool solve(int idx) {
            if (idx == m) return true;
            int u = edges[idx].u, v = edges[idx].v;
            int forbidden = used_mask[u] | used_mask[v];
            for (int c = 1; c <= 3; ++c) {
                int bit = 1 << c;
                if (forbidden & bit) continue;
                color[idx] = c;
                used_mask[u] |= bit;
                used_mask[v] |= bit;
                if (solve(idx + 1)) return true;
                used_mask[u] &= ~bit;
                used_mask[v] &= ~bit;
            }
            color[idx] = 0;
            return false;
        }
    };

    State state = {edges, color, used_mask, inc, m};
    return state.solve(0);
}

} // namespace detail

/**
 * @brief Determines whether the graph is a snark
 * @param g Input graph
 * @param algo Algorithm to use (default: COMBINED)
 * @return SnarkResult
 *
 * Snark <=> cubic AND bridgeless AND girth >= 5 AND
 *   cyclically 4-edge-connected AND chromatic index 4.
 */
inline SnarkResult check_snark(const Graph& g,
    SnarkAlgorithm algo = SnarkAlgorithm::COMBINED) {
    (void)algo;
    SnarkResult res;

    int n = g.n;
    if (n < 10) return res; /* minimum snark = Petersen (10 vertices) */

    /* Cubic (3-regular) check */
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != 3) return res;
    }

    /* n must be even (cubic -> 3n = 2m -> n even) */
    if (n % 2 != 0) return res;

    /* Connectivity check */
    std::vector<char> visited(n + 1, 0);
    std::vector<int> queue;
    queue.push_back(1);
    visited[1] = 1;
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int v = queue[qi];
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (!visited[u]) {
                visited[u] = 1;
                queue.push_back(u);
            }
        }
    }
    if ((int)queue.size() != n) return res;

    /* Bridgeless (2-edge-connected) */
    if (detail::has_bridge(g)) return res;

    /* Girth >= 5 */
    int girth = detail::compute_girth(g);
    if (girth < 5) return res;

    /* Cyclically 4-edge-connected */
    if (!detail::is_cyclically_4_edge_connected(g)) return res;

    /* Not 3-edge-colorable */
    if (detail::is_3_edge_colorable(g)) return res;

    res.is_snark = true;
    return res;
}

} // namespace graph_recognition

#endif
