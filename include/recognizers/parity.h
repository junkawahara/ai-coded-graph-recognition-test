#ifndef GRAPH_RECOGNITION_PARITY_H
#define GRAPH_RECOGNITION_PARITY_H

/**
 * @file parity.h
 * @brief Parity graph recognition
 *
 * A parity graph is a graph where all induced paths between any two vertices
 * have the same parity (all even length or all odd length).
 *
 * A superclass of distance-hereditary graphs.
 * Equivalent characterization: all prime components of the split decomposition are
 * complete graphs or bipartite graphs.
 *
 * Algorithms:
 *   - DIRECT_CHECK: verifies the BFS distance and induced path parity
 *     for all vertex pairs via DFS backtracking. Practical for small n.
 *
 * References:
 *   - Burlet, Uhry, "Parity graphs," Annals of Discrete Math., 1984
 *   - Bouchet, "Reducing prime graphs and recognizing circle graphs,"
 *     Combinatorica, 1987
 */

#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "certificates/obstruction_extract.h"

#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for parity graph recognition
 */
enum class ParityAlgorithm {
    DIRECT_CHECK /**< Direct definition check (BFS + backtracking) */
};

/**
 * @brief Result of parity graph recognition
 */
struct ParityResult {
    bool is_parity = false; /**< true if the graph is a parity graph */
    Obstruction obstruction; /**< NO certificate: INDUCED_PATH_WRONG_PARITY, the two
                                  induced u-v paths of different length parity that
                                  parity graphs forbid. Valid only when
                                  is_parity == false */
};

namespace detail_parity {

/**
 * @brief Internal state for induced path search
 *
 * blocked[w] = number of vertices on the path that are adjacent to w.
 * Condition for adding vertex w to the path end: blocked[w] == 1
 * (adjacent only to the current endpoint).
 */
struct ParityCheckState {
    const Graph& g;
    std::vector<int> blocked;
    std::vector<unsigned char> in_path;
    std::vector<int> path;    /**< the u..cur prefix currently being extended */
    std::vector<int> witness;  /**< the offending u-v path, once found */
    int target_v;
    int target_parity;
    bool found;

    ParityCheckState(const Graph& g_, int n, int v, int tp)
        : g(g_), blocked(n + 1, 0), in_path(n + 1, 0),
          target_v(v), target_parity(tp), found(false) {}
};

/**
 * @brief DFS backtracking for induced paths
 *
 * Searches for induced paths from cur to target_v, and sets found = true if a path
 * with parity different from target_parity is found.
 */
inline void parity_dfs(ParityCheckState& state, int cur, int depth) {
    if (state.found) return;
    for (size_t i = 0; i < state.g.adj[cur].size(); ++i) {
        if (state.found) return;
        int w = state.g.adj[cur][i];
        if (state.in_path[w]) continue;

        if (w == state.target_v) {
            if (state.blocked[w] == 1) {
                if ((depth + 1) % 2 != state.target_parity) {
                    state.found = true;
                    state.witness = state.path;
                    state.witness.push_back(state.target_v);
                    return;
                }
            }
            continue;
        }

        if (state.blocked[w] != 1) continue;

        state.in_path[w] = 1;
        state.path.push_back(w);
        for (size_t j = 0; j < state.g.adj[w].size(); ++j) {
            state.blocked[state.g.adj[w][j]]++;
        }

        parity_dfs(state, w, depth + 1);

        for (size_t j = 0; j < state.g.adj[w].size(); ++j) {
            state.blocked[state.g.adj[w][j]]--;
        }
        state.path.pop_back();
        state.in_path[w] = 0;
    }
}

/**
 * @brief Determines whether an induced path from u to v with parity different from target_parity exists
 */
inline bool has_induced_path_diff_parity(const Graph& g, int u, int v,
                                         int target_parity,
                                         std::vector<int>* witness = 0) {
    ParityCheckState state(g, g.n, v, target_parity);
    state.in_path[u] = 1;
    state.path.push_back(u);
    for (size_t i = 0; i < g.adj[u].size(); ++i) {
        state.blocked[g.adj[u][i]]++;
    }
    parity_dfs(state, u, 0);
    if (state.found && witness) *witness = state.witness;
    return state.found;
}

}  // namespace detail_parity

/**
 * @brief Parity graph recognition (direct definition check)
 *
 * Computes BFS distance d(u,v) for all vertex pairs (u,v),
 * and verifies that no induced u-v path with parity different from d(u,v) exists.
 */
inline ParityResult check_parity_direct(const Graph& g) {
    ParityResult res;
    res.is_parity = true;
    int n = g.n;
    if (n <= 2) return res;

    // Compute distances for all pairs via BFS
    std::vector<std::vector<int>> dist(n + 1, std::vector<int>(n + 1, -1));
    for (int s = 1; s <= n; ++s) {
        dist[s][s] = 0;
        std::queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (dist[s][w] == -1) {
                    dist[s][w] = dist[s][u] + 1;
                    q.push(w);
                }
            }
        }
    }

    // Verify induced path parity for each pair
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (dist[u][v] == -1) continue;  // Different connected components
            int target_parity = dist[u][v] % 2;
            std::vector<int> odd_one;
            if (detail_parity::has_induced_path_diff_parity(g, u, v,
                                                            target_parity, &odd_one)) {
                res.is_parity = false;
                // The second path is a shortest u-v path: shortest paths are
                // induced, and its length has the parity the first one misses.
                std::vector<unsigned char> allowed(n + 1, 1);
                allowed[0] = 0;
                std::vector<int> shortest =
                    detail_obstruction::shortest_path_in_allowed(g, u, v, allowed);
                if (!odd_one.empty() && shortest.size() >= 2) {
                    res.obstruction.kind = ObstructionKind::INDUCED_PATH_WRONG_PARITY;
                    res.obstruction.vertices.push_back(u);
                    res.obstruction.vertices.push_back(v);
                    res.obstruction.vertex_sets.push_back(odd_one);
                    res.obstruction.vertex_sets.push_back(shortest);
                }
                return res;
            }
        }
    }
    return res;
}

/**
 * @brief Parity graph recognition (default)
 */
inline ParityResult check_parity(
    const Graph& g,
    ParityAlgorithm algo = ParityAlgorithm::DIRECT_CHECK) {
    (void)algo;
    return check_parity_direct(g);
}

}  // namespace graph_recognition

#endif
