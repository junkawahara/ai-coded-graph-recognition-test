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
#include <vector>

namespace graph_recognition {
namespace gtest_utils {

/** @brief Some vertex subset of size >= minlen induces a chordless cycle
 *         (a connected 2-regular induced subgraph). Exponential in n. */
inline bool bf_has_induced_cycle_ge(int n,
                                    const std::vector<std::vector<bool>>& adj,
                                    int minlen) {
    for (int mask = 0; mask < (1 << n); ++mask) {
        int k = __builtin_popcount(mask);
        if (k < minlen) continue;
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

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
