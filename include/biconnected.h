#ifndef GRAPH_RECOGNITION_BICONNECTED_H
#define GRAPH_RECOGNITION_BICONNECTED_H

/**
 * @file biconnected.h
 * @brief Biconnected graph recognition
 *
 * Determines whether a graph is biconnected.
 * A biconnected graph is a connected graph with at least 3 vertices and no cut vertices.
 *
 * Algorithms:
 *   - DFS: Tarjan's cut vertex detection O(n+m) (default)
 *   - BLOCK_CUT_TREE: shared block decomposition; biconnected iff the whole
 *     vertex set is a single block
 */

#include "block_cut_tree.h"
#include "forbidden_subgraph.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for biconnected recognition
 */
enum class BiconnectedAlgorithm {
    DFS,            /**< Tarjan's cut vertex detection (default) */
    BLOCK_CUT_TREE  /**< derived from the shared block decomposition */
};

/**
 * @brief Result of biconnected recognition
 */
struct BiconnectedResult {
    bool is_biconnected = false; /**< true if the graph is biconnected */
    Obstruction obstruction;     /**< NO certificate: a CUT_VERTEX, or a
                                      DISCONNECTED_PAIR when the graph falls apart
                                      without removing anything. Valid only when
                                      is_biconnected == false, and left empty for
                                      n < 3, where the graph fails on size alone and
                                      no such witness need exist (K2 has neither) */
};

namespace detail {

/** @brief Two vertices in different components, or an empty vector if connected */
inline std::vector<int> find_disconnected_pair(const Graph& g) {
    std::vector<int> pair;
    if (g.n < 2) return pair;

    std::vector<unsigned char> seen(g.n + 1, 0);
    std::queue<int> q;
    seen[1] = 1;
    q.push(1);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (seen[u]) continue;
            seen[u] = 1;
            q.push(u);
        }
    }
    for (int v = 2; v <= g.n; ++v) {
        if (seen[v]) continue;
        pair.push_back(1);
        pair.push_back(v);
        return pair;
    }
    return pair;
}

/** @brief Biconnectivity via Tarjan's cut vertex detection (original algorithm) */
inline BiconnectedResult check_biconnected_dfs(const Graph& g) {
    BiconnectedResult res;
    res.is_biconnected = false;

    int n = g.n;
    if (n < 3) return res;

    // Tarjan's algorithm for articulation point detection (iterative DFS)
    std::vector<int> tin(n + 1, 0);
    std::vector<int> low(n + 1, 0);
    int timer = 0;

    bool has_artic = false;
    int artic_vertex = 0;

    // DFS stack frame
    struct Frame {
        int v, parent;
        size_t i;       // index into adj[v]
        int children;   // DFS children count (for root check)
    };
    std::vector<Frame> stack;

    // Start DFS from vertex 1
    timer++;
    tin[1] = low[1] = timer;
    Frame f0;
    f0.v = 1;
    f0.parent = -1;
    f0.i = 0;
    f0.children = 0;
    stack.push_back(f0);

    while (!stack.empty() && !has_artic) {
        Frame& f = stack.back();
        int v = f.v;

        if (f.i < g.adj[v].size()) {
            int u = g.adj[v][f.i];
            f.i++;

            if (u == f.parent) continue;

            if (tin[u] == 0) {
                // Tree edge
                f.children++;
                timer++;
                tin[u] = low[u] = timer;
                Frame fn;
                fn.v = u;
                fn.parent = v;
                fn.i = 0;
                fn.children = 0;
                stack.push_back(fn);
            } else {
                // Back edge
                low[v] = std::min(low[v], tin[u]);
            }
        } else {
            // Finished processing vertex v
            int v_children = f.children;
            stack.pop_back();
            if (!stack.empty()) {
                Frame& pf = stack.back();
                int pv = pf.v;
                low[pv] = std::min(low[pv], low[v]);

                // Articulation point check (non-root)
                if (pf.parent != -1 && low[v] >= tin[pv]) {
                    has_artic = true;
                    artic_vertex = pv;
                }
            } else {
                // Root: articulation if >= 2 DFS children
                if (v_children >= 2) {
                    has_artic = true;
                    artic_vertex = v;
                }
            }
        }
    }

    if (has_artic) {
        res.obstruction = make_obstruction(ObstructionKind::CUT_VERTEX,
                                           std::vector<int>(1, artic_vertex));
        return res;
    }

    // Check connectivity: all vertices must be visited
    int visited = 0;
    for (int v = 1; v <= n; ++v) {
        if (tin[v] != 0) visited++;
    }
    if (visited != n) {
        res.obstruction = make_obstruction(ObstructionKind::DISCONNECTED_PAIR,
                                           find_disconnected_pair(g));
        if (res.obstruction.vertices.empty()) res.obstruction = Obstruction();
        return res;
    }

    res.is_biconnected = true;
    return res;
}

/**
 * @brief Biconnectivity read off the block decomposition
 *
 * A graph with at least 3 vertices is biconnected exactly when its whole
 * vertex set forms one block: a cut vertex or a second component would split
 * the decomposition into several blocks.
 */
inline BiconnectedResult check_biconnected_block_cut_tree(const Graph& g) {
    BiconnectedResult res;
    if (g.n < 3) return res;
    BlockCutTreeResult bct = compute_block_cut_tree(g);
    res.is_biconnected = bct.blocks.size() == 1 &&
                         (int)bct.blocks[0].size() == g.n;
    if (res.is_biconnected) return res;

    // A graph on >= 3 vertices that is connected and has no cut vertex is
    // biconnected, so exactly one of the two witnesses is available here.
    for (int v = 1; v <= g.n; ++v) {
        if (!bct.is_cut[v]) continue;
        res.obstruction = make_obstruction(ObstructionKind::CUT_VERTEX,
                                           std::vector<int>(1, v));
        return res;
    }
    std::vector<int> pair = find_disconnected_pair(g);
    if (!pair.empty()) {
        res.obstruction = make_obstruction(ObstructionKind::DISCONNECTED_PAIR, pair);
    }
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is biconnected
 * @param g Input graph
 * @param algo Algorithm to use (default: DFS)
 * @return BiconnectedResult
 *
 * Biconnected graph: at least 3 vertices, connected, no cut vertices.
 * O(n+m) time using Tarjan's DFS.
 */
inline BiconnectedResult check_biconnected(const Graph& g,
    BiconnectedAlgorithm algo = BiconnectedAlgorithm::DFS) {
    switch (algo) {
        case BiconnectedAlgorithm::DFS:
            return detail::check_biconnected_dfs(g);
        case BiconnectedAlgorithm::BLOCK_CUT_TREE:
            return detail::check_biconnected_block_cut_tree(g);
        default:
            break;
    }
    return BiconnectedResult();
}

} // namespace graph_recognition

#endif
