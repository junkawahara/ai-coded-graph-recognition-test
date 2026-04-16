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
 * @brief Bridge detection (DFS)
 * @return true if a bridge exists
 */
inline bool has_bridge(const Graph& g) {
    int n = g.n;
    if (n <= 1) return false;
    std::vector<int> disc(n + 1, -1), low(n + 1, 0);
    int timer = 0;
    bool found = false;

    struct DFSState {
        int v, parent, adj_idx;
    };
    std::vector<DFSState> stack;

    for (int s = 1; s <= n && !found; ++s) {
        if (disc[s] != -1) continue;
        disc[s] = low[s] = timer++;
        stack.push_back({s, 0, 0});

        while (!stack.empty() && !found) {
            DFSState& st = stack.back();
            if (st.adj_idx < (int)g.adj[st.v].size()) {
                int u = g.adj[st.v][st.adj_idx++];
                if (disc[u] == -1) {
                    disc[u] = low[u] = timer++;
                    stack.push_back({u, st.v, 0});
                } else if (u != st.parent) {
                    if (disc[u] < low[st.v]) low[st.v] = disc[u];
                }
            } else {
                int v = st.v;
                int p = st.parent;
                stack.pop_back();
                if (!stack.empty()) {
                    if (low[v] < low[stack.back().v])
                        low[stack.back().v] = low[v];
                    if (low[v] > disc[p]) found = true; /* Bridge */
                }
            }
        }
    }
    return found;
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
 * @brief Compute maximum flow between s-t in a graph with an edge removed (BFS)
 * @param g Original graph
 * @param skip_u, skip_v Endpoints of the edge to remove
 * @param s Source
 * @param t Sink
 * @param[out] cut_side Vertex set on the S side (optional)
 * @return Maximum flow value
 */
inline int max_flow_unit(const Graph& g, int skip_u, int skip_v,
    int s, int t, std::vector<char>* cut_side) {
    int n = g.n;
    /* Build edge list (bidirectional) */
    struct Edge { int to, cap; size_t rev; };
    std::vector<std::vector<Edge>> adj(n + 1);

    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u > v) continue;
            if ((u == skip_u && v == skip_v) || (u == skip_v && v == skip_u))
                continue;
            size_t iu = adj[u].size(), iv = adj[v].size();
            adj[u].push_back({v, 1, iv});
            adj[v].push_back({u, 0, iu});
            iv = adj[v].size();
            iu = adj[u].size();
            adj[v].push_back({u, 1, iu});
            adj[u].push_back({v, 0, iv});
        }
    }

    int flow = 0;
    while (flow < 3) {
        /* Find augmenting path via BFS */
        std::vector<int> prev_v(n + 1, -1);
        std::vector<size_t> prev_e(n + 1, 0);
        std::vector<int> bfs;
        prev_v[s] = s;
        bfs.push_back(s);
        bool found = false;
        for (size_t qi = 0; qi < bfs.size() && !found; ++qi) {
            int v = bfs[qi];
            for (size_t i = 0; i < adj[v].size() && !found; ++i) {
                Edge& e = adj[v][i];
                if (e.cap > 0 && prev_v[e.to] == -1) {
                    prev_v[e.to] = v;
                    prev_e[e.to] = i;
                    if (e.to == t) {
                        found = true;
                    } else {
                        bfs.push_back(e.to);
                    }
                }
            }
        }
        if (!found) break;

        /* Push flow along the path */
        for (int v = t; v != s;) {
            int p = prev_v[v];
            size_t ei = prev_e[v];
            adj[p][ei].cap--;
            adj[v][adj[p][ei].rev].cap++;
            v = p;
        }
        ++flow;
    }

    /* Return the cut side */
    if (cut_side) {
        cut_side->assign(n + 1, 0);
        std::vector<int> bfs2;
        std::vector<char>& cs = *cut_side;
        cs[s] = 1;
        bfs2.push_back(s);
        for (size_t qi = 0; qi < bfs2.size(); ++qi) {
            int v = bfs2[qi];
            for (size_t i = 0; i < adj[v].size(); ++i) {
                Edge& e = adj[v][i];
                if (e.cap > 0 && !cs[e.to]) {
                    cs[e.to] = 1;
                    bfs2.push_back(e.to);
                }
            }
        }
    }

    return flow;
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
            int start = (u == 1 && v == 2) ? ((n > 2) ? 3 : 1) : 1;
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

    /* Check for non-trivial 3-edge cuts:
       For each edge (u,v), when removing it and the max flow between u-v is 2,
       check if the cut side is non-trivial (>= 3 vertices on each side) */
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (v <= u) continue;

            std::vector<char> cut_side;
            int flow = max_flow_unit(g, u, v, u, v, &cut_side);

            if (flow == 2) {
                /* 3-edge cut found. Check the size of both sides */
                int s_side = 0;
                for (int w = 1; w <= n; ++w) {
                    if (cut_side[w]) ++s_side;
                }
                int t_side = n - s_side;
                if (s_side >= 3 && t_side >= 3) return false;
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
