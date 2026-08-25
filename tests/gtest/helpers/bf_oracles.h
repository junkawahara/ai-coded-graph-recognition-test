#ifndef GRAPH_RECOGNITION_TESTS_BF_ORACLES_H
#define GRAPH_RECOGNITION_TESTS_BF_ORACLES_H

/**
 * @file bf_oracles.h
 * @brief Brute-force oracles shared by the property tests
 *
 * These enumerate structures straight from the definitions and share no
 * code with the library under include/ (that independence is the point of
 * the property tests). They are hoisted here so a fix to the shared logic
 * lands once instead of drifting across per-class copies.
 *
 * All oracles take a 1-indexed adjacency matrix adj[(n+1) x (n+1)].
 */

#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace gtest_utils {

namespace bf_detail {

/** @brief Some vertex subset whose size satisfies accept_len induces a
 *         chordless cycle (a connected 2-regular induced subgraph).
 *         Exponential in n. */
template <typename LenPred>
inline bool has_chordless_cycle(int n,
                                const std::vector<std::vector<bool>>& adj,
                                LenPred accept_len) {
    for (int mask = 0; mask < (1 << n); ++mask) {
        int k = __builtin_popcount(mask);
        if (!accept_len(k)) continue;
        std::vector<int> vs;
        for (int i = 0; i < n; ++i)
            if (mask & (1 << i)) vs.push_back(i + 1);
        bool all_deg2 = true;
        for (int i = 0; i < k && all_deg2; ++i) {
            int d = 0;
            for (int j = 0; j < k; ++j)
                if (j != i && adj[vs[i]][vs[j]]) ++d;
            if (d != 2) all_deg2 = false;
        }
        if (!all_deg2) continue;
        std::vector<bool> vis(k, false);
        std::vector<int> stack;
        stack.push_back(0);
        vis[0] = true;
        int seen = 1;
        while (!stack.empty()) {
            int i = stack.back();
            stack.pop_back();
            for (int j = 0; j < k; ++j) {
                if (!vis[j] && adj[vs[i]][vs[j]]) {
                    vis[j] = true;
                    ++seen;
                    stack.push_back(j);
                }
            }
        }
        if (seen == k) return true;  // connected 2-regular = chordless cycle
    }
    return false;
}

}  // namespace bf_detail

/** @brief Some vertex subset of size >= minlen induces a chordless cycle */
inline bool bf_has_induced_cycle_ge(int n,
                                    const std::vector<std::vector<bool>>& adj,
                                    int minlen) {
    struct GeMin {
        int minlen;
        bool operator()(int k) const { return k >= minlen; }
    };
    GeMin pred = {minlen};
    return bf_detail::has_chordless_cycle(n, adj, pred);
}

/** @brief Some vertex subset induces an odd hole (chordless odd cycle of
 *         length >= 5) */
inline bool bf_has_odd_hole(int n, const std::vector<std::vector<bool>>& adj) {
    struct OddGe5 {
        bool operator()(int k) const { return k >= 5 && k % 2 == 1; }
    };
    return bf_detail::has_chordless_cycle(n, adj, OddGe5());
}

/** @brief BFS 2-coloring */
inline bool bf_is_bipartite(int n, const std::vector<std::vector<bool>>& adj) {
    std::vector<int> color(n + 1, -1);
    for (int s = 1; s <= n; ++s) {
        if (color[s] != -1) continue;
        color[s] = 0;
        std::vector<int> queue;
        queue.push_back(s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (int v = 1; v <= n; ++v) {
                if (!adj[u][v]) continue;
                if (color[v] == -1) {
                    color[v] = 1 - color[u];
                    queue.push_back(v);
                } else if (color[v] == color[u]) {
                    return false;
                }
            }
        }
    }
    return true;
}

/** @brief Exists an ordering of `side` such that N(y) (restricted to
 *         `side`) is consecutive for every y in `other`. All orderings are
 *         enumerated exhaustively. */
inline bool bf_side_orderable(const std::vector<int>& side,
                              const std::vector<int>& other,
                              const std::vector<std::vector<bool>>& adj) {
    std::vector<int> perm = side;
    std::sort(perm.begin(), perm.end());
    do {
        bool ok = true;
        for (size_t yi = 0; yi < other.size() && ok; ++yi) {
            int lo = -1, hi = -1, cnt = 0;
            for (size_t p = 0; p < perm.size(); ++p) {
                if (adj[other[yi]][perm[p]]) {
                    if (lo < 0) lo = (int)p;
                    hi = (int)p;
                    ++cnt;
                }
            }
            if (cnt > 0 && hi - lo + 1 != cnt) ok = false;
        }
        if (ok) return true;
    } while (std::next_permutation(perm.begin(), perm.end()));
    return false;
}

/** @brief Induced claw: a center adjacent to three pairwise non-adjacent
 *         vertices */
inline bool bf_has_claw(int n, const std::vector<std::vector<bool>>& adj) {
    for (int c = 1; c <= n; ++c) {
        for (int a = 1; a <= n; ++a) {
            if (a == c || !adj[c][a]) continue;
            for (int b = a + 1; b <= n; ++b) {
                if (b == c || !adj[c][b] || adj[a][b]) continue;
                for (int d = b + 1; d <= n; ++d) {
                    if (d == c || !adj[c][d] || adj[a][d] || adj[b][d]) continue;
                    return true;
                }
            }
        }
    }
    return false;
}

namespace bf_detail {

/** @brief Number of connected components of the subgraph induced on the
 *         vertices for which keep is true. */
inline int component_count(int n, const std::vector<std::vector<bool>>& adj,
                           const std::vector<bool>& keep) {
    std::vector<bool> seen(n + 1, false);
    int count = 0;
    for (int s = 1; s <= n; ++s) {
        if (!keep[s] || seen[s]) continue;
        ++count;
        std::vector<int> stack(1, s);
        seen[s] = true;
        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            for (int u = 1; u <= n; ++u) {
                if (!keep[u] || seen[u] || !adj[v][u]) continue;
                seen[u] = true;
                stack.push_back(u);
            }
        }
    }
    return count;
}

}  // namespace bf_detail

/** @brief Cut vertices, straight from the definition: removing v raises the
 *         number of components of the graph restricted to the other
 *         vertices. Isolated vertices never count. */
inline std::vector<int> bf_articulation_vertices(
    int n, const std::vector<std::vector<bool>>& adj) {
    std::vector<bool> all(n + 1, true);
    all[0] = false;
    int base = bf_detail::component_count(n, adj, all);
    std::vector<int> cuts;
    for (int v = 1; v <= n; ++v) {
        std::vector<bool> keep = all;
        keep[v] = false;
        // Removing v also removes its own component when v was isolated, so
        // compare against the base count minus that vertex's own component.
        int expected = base;
        bool isolated = true;
        for (int u = 1; u <= n && isolated; ++u)
            if (adj[v][u]) isolated = false;
        if (isolated) expected = base - 1;
        if (bf_detail::component_count(n, adj, keep) > expected) cuts.push_back(v);
    }
    return cuts;
}

/** @brief Bridges, straight from the definition: removing the edge raises
 *         the number of components. Returned with u < v, ascending. */
inline std::vector<std::pair<int, int>> bf_bridges(
    int n, const std::vector<std::vector<bool>>& adj) {
    std::vector<bool> all(n + 1, true);
    all[0] = false;
    int base = bf_detail::component_count(n, adj, all);
    std::vector<std::pair<int, int>> bridges;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!adj[u][v]) continue;
            std::vector<std::vector<bool>> cut = adj;
            cut[u][v] = cut[v][u] = false;
            if (bf_detail::component_count(n, cut, all) > base) {
                bridges.push_back(std::make_pair(u, v));
            }
        }
    }
    return bridges;
}

/** @brief Treewidth by DP over vertex subsets (exponential; n <= ~16)
 *
 * f(S) = min over v in S of max(f(S \ {v}), q(S \ {v}, v)), where q(S', v)
 * counts the vertices outside S' + {v} reachable from v through S'.
 * tw(G) = f(V). Takes a 1-indexed edge list rather than a matrix, matching
 * how the callers build their random graphs.
 */
inline int bf_treewidth(int n, const std::vector<std::pair<int, int>>& edges) {
    if (n <= 0) return 0;
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first - 1, v = edges[i].second - 1;
        adj[u][v] = adj[v][u] = true;
    }
    int full = 1 << n;
    std::vector<int> f(full, 0);
    for (int S = 1; S < full; ++S) {
        int best = -1;
        for (int v = 0; v < n; ++v) {
            if (!((S >> v) & 1)) continue;
            int Sp = S & ~(1 << v);
            int q = 0;
            std::vector<bool> vis(n, false);
            std::vector<int> stack;
            stack.push_back(v);
            vis[v] = true;
            while (!stack.empty()) {
                int x = stack.back();
                stack.pop_back();
                for (int w = 0; w < n; ++w) {
                    if (!adj[x][w] || vis[w]) continue;
                    vis[w] = true;
                    if ((Sp >> w) & 1) {
                        stack.push_back(w);
                    } else {
                        ++q;
                    }
                }
            }
            int cand = f[Sp] > q ? f[Sp] : q;
            if (best < 0 || cand < best) best = cand;
        }
        f[S] = best;
    }
    return f[full - 1];
}

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
