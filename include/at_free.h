#ifndef GRAPH_RECOGNITION_AT_FREE_H
#define GRAPH_RECOGNITION_AT_FREE_H

/**
 * @file at_free.h
 * @brief AT-free graph recognition
 *
 * Determines AT-free if no asteroidal triple exists.
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for AT-free graph recognition
 */
enum class ATFreeAlgorithm {
    BRUTE_FORCE /**< brute-force search over all triples */
};

/**
 * @brief Result of AT-free graph recognition
 */
struct ATFreeResult {
    bool is_at_free = false; /**< true if the graph is AT-free */
    Obstruction obstruction; /**< NO certificate: an ASTEROIDAL_TRIPLE. Valid only
                                  when is_at_free == false */
};

namespace detail {

/**
 * @brief Finds an asteroidal triple (AT) in the graph
 * @param g Input graph
 * @return The three vertices of an AT, or an empty vector if g is AT-free
 *
 * The three vertices are the whole certificate: a verifier re-derives the
 * connecting paths by removing each closed neighbourhood in turn, so there is
 * nothing else worth carrying.
 */
inline std::vector<int> find_asteroidal_triple(const Graph& g) {
    std::vector<int> triple;
    int n = g.n;
    if (n < 3) return triple;

    // O(n^2) memory + O(n^3) time. No size guard: silently reporting
    // "AT exists" for large n would turn AT-free graphs (e.g. long paths)
    // into false NOs. Very large inputs are simply slow / allocation-bound.

    // Flat array for component labels: comp[v*(n+1)+u] = component of u in G-N[v].
    // Uses O(n^2) ints in a single allocation, avoiding per-row vector overhead.
    size_t stride = (size_t)(n + 1);
    std::vector<int> comp(stride * stride, -1);

    for (int v = 1; v <= n; ++v) {
        std::vector<unsigned char> blocked(n + 1, 0);
        blocked[v] = 1;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            blocked[g.adj[v][i]] = 1;
        }

        int label = 0;
        std::queue<int> q;
        for (int u = 1; u <= n; ++u) {
            if (blocked[u]) continue;
            if (comp[v * stride + u] >= 0) continue;
            comp[v * stride + u] = label;
            q.push(u);
            while (!q.empty()) {
                int cur = q.front();
                q.pop();
                for (size_t i = 0; i < g.adj[cur].size(); ++i) {
                    int w = g.adj[cur][i];
                    if (blocked[w]) continue;
                    if (comp[v * stride + w] >= 0) continue;
                    comp[v * stride + w] = label;
                    q.push(w);
                }
            }
            label++;
        }
    }

    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                if (comp[c * stride + a] >= 0 && comp[c * stride + b] >= 0 &&
                    comp[c * stride + a] == comp[c * stride + b] &&
                    comp[b * stride + a] >= 0 && comp[b * stride + c] >= 0 &&
                    comp[b * stride + a] == comp[b * stride + c] &&
                    comp[a * stride + b] >= 0 && comp[a * stride + c] >= 0 &&
                    comp[a * stride + b] == comp[a * stride + c]) {
                    triple.push_back(a);
                    triple.push_back(b);
                    triple.push_back(c);
                    return triple;
                }
            }
        }
    }

    return triple;
}

/**
 * @brief Determines whether an asteroidal triple (AT) exists in the graph
 * @param g Input graph
 * @return true if an AT exists
 */
inline bool has_asteroidal_triple(const Graph& g) {
    return !find_asteroidal_triple(g).empty();
}

} // namespace detail

/**
 * @brief Determines whether a graph is AT-free
 * @param g Input graph
 * @param algo Algorithm selector (currently only BRUTE_FORCE is implemented)
 * @return ATFreeResult
 *
 * AT-free if no asteroidal triple exists.
 */
inline ATFreeResult check_at_free(const Graph& g,
    ATFreeAlgorithm algo = ATFreeAlgorithm::BRUTE_FORCE) {
    (void)algo;
    ATFreeResult res;
    std::vector<int> triple = detail::find_asteroidal_triple(g);
    res.is_at_free = triple.empty();
    if (!res.is_at_free) {
        res.obstruction = make_obstruction(ObstructionKind::ASTEROIDAL_TRIPLE, triple);
    }
    return res;
}

} // namespace graph_recognition

#endif
