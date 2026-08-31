#ifndef GRAPH_RECOGNITION_DEGENERATE_H
#define GRAPH_RECOGNITION_DEGENERATE_H

/**
 * @file degenerate.h
 * @brief k-degenerate graph recognition
 *
 * A graph is k-degenerate if every (nonempty) induced subgraph has a
 * vertex of degree at most k (Lick-White 1970). Equivalently, the
 * vertices can be removed one by one so that each removed vertex has at
 * most k neighbors among those remaining; the smallest such k is the
 * degeneracy. 1-degenerate = forests; outerplanar graphs are
 * 2-degenerate, planar graphs 5-degenerate, and treewidth <= k implies
 * k-degenerate. Like partial_ktree.h (and unlike the other
 * recognizers), k is an input: every graph on n vertices is
 * (n-1)-degenerate, so nothing can be inferred.
 *
 * The recognizer is the classic minimum-degree peeling (the
 * "smallest-last ordering" of Matula-Beck 1983) with a bucket queue:
 * repeatedly remove a vertex of minimum degree in the remaining graph;
 * the maximum degree seen at a removal is exactly the degeneracy, in
 * O(n + m) time. The exact degeneracy therefore comes free and is
 * reported on both YES and NO. On YES the peeling order certifies the
 * answer; on NO the (k+1)-core — the unique maximal induced subgraph
 * of minimum degree >= k+1, obtained by exhaustively deleting vertices
 * of degree <= k — is a nonempty induced subgraph in which every vertex
 * has more than k neighbors, certifying that no removal order exists.
 *
 * References:
 *   Lick, White, "k-degenerate graphs," Canad. J. Math. 22, 1970
 *   (definition and basic properties);
 *   Matula, Beck, "Smallest-last ordering and clustering and graph
 *   coloring algorithms," J. ACM 30, 1983 (the linear-time peeling);
 *   Seidman, "Network structure and minimum degree," Social Networks 5,
 *   1983 (k-cores)
 */

#include "util/graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for k-degenerate recognition
 */
enum class DegenerateAlgorithm {
    MIN_DEGREE_PEELING /**< Bucket-queue smallest-last peeling (Matula-Beck) */
};

/**
 * @brief Result of k-degenerate recognition
 */
struct DegenerateResult {
    bool is_degenerate = false; /**< true if the graph is k-degenerate */
    /**
     * @brief The exact degeneracy of the graph
     *
     * Always filled (the peeling computes it regardless of the answer),
     * so is_degenerate == (degeneracy <= k) whenever k >= 0. The empty
     * graph gets -1 (consistent with degeneracy <= treewidth).
     */
    int degeneracy = -1;
    /**
     * @brief removal_order[i] = the i-th vertex removed, for i in [1, n] (size n+1)
     *
     * A smallest-last order: removing the vertices in this order, every
     * vertex has at most `degeneracy` (hence at most k) neighbors among
     * those not yet removed. Read backwards it is an order in which each
     * vertex has at most k earlier neighbors. Valid only when
     * is_degenerate == true.
     */
    std::vector<int> removal_order;
    /**
     * @brief The vertices of the (k+1)-core, sorted ascending
     *
     * The unique maximal set inducing a subgraph of minimum degree
     * >= k+1. Nonempty, and every vertex in it has more than k
     * neighbors inside it, so no removal order can dispose of it: a
     * NO certificate. Valid only when is_degenerate == false (empty
     * otherwise); this is a vertex list, not a vertex-indexed vector.
     */
    std::vector<int> core;
};

namespace detail {

/**
 * @brief Smallest-last peeling: fills the removal order and returns the degeneracy
 * @param g Input graph (n >= 1)
 * @param order Output: order[i] = the i-th vertex removed (size n+1, 1-indexed)
 *
 * Bucket queue over current degrees; each step removes a vertex of
 * minimum remaining degree in O(1) amortized, for O(n + m) total.
 */
inline int degenerate_peel(const Graph& g, std::vector<int>& order) {
    const int n = g.n;
    std::vector<int> deg(n + 1, 0);
    for (int v = 1; v <= n; ++v) deg[v] = (int)g.adj[v].size();

    // bucket[d] lists the alive vertices of current degree d;
    // pos[v] is v's index inside bucket[deg[v]] for O(1) removal.
    std::vector<std::vector<int> > bucket(n);
    std::vector<int> pos(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        pos[v] = (int)bucket[deg[v]].size();
        bucket[deg[v]].push_back(v);
    }

    order.assign(n + 1, 0);
    std::vector<bool> removed(n + 1, false);
    int degeneracy = 0;
    int cur = 0;
    for (int i = 1; i <= n; ++i) {
        if (cur > 0) --cur;  // a neighbor update can lower the minimum by 1
        while (bucket[cur].empty()) ++cur;
        const int v = bucket[cur].back();
        bucket[cur].pop_back();
        removed[v] = true;
        order[i] = v;
        if (cur > degeneracy) degeneracy = cur;
        for (std::size_t j = 0; j < g.adj[v].size(); ++j) {
            const int u = g.adj[v][j];
            if (removed[u]) continue;
            const int d = deg[u];
            const int back = bucket[d].back();
            bucket[d][pos[u]] = back;
            pos[back] = pos[u];
            bucket[d].pop_back();
            deg[u] = d - 1;
            pos[u] = (int)bucket[d - 1].size();
            bucket[d - 1].push_back(u);
        }
    }
    return degeneracy;
}

/**
 * @brief The (k+1)-core: exhaustively delete vertices of degree <= k
 * @return The surviving vertices, sorted ascending (all of them when k < 0)
 */
inline std::vector<int> degenerate_core(const Graph& g, int k) {
    const int n = g.n;
    std::vector<int> deg(n + 1, 0);
    std::vector<bool> dead(n + 1, false);
    std::vector<int> queue;
    for (int v = 1; v <= n; ++v) {
        deg[v] = (int)g.adj[v].size();
        if (deg[v] <= k) {
            dead[v] = true;
            queue.push_back(v);
        }
    }
    while (!queue.empty()) {
        const int v = queue.back();
        queue.pop_back();
        for (std::size_t j = 0; j < g.adj[v].size(); ++j) {
            const int u = g.adj[v][j];
            if (!dead[u] && --deg[u] <= k) {
                dead[u] = true;
                queue.push_back(u);
            }
        }
    }
    std::vector<int> core;
    for (int v = 1; v <= n; ++v) {
        if (!dead[v]) core.push_back(v);
    }
    return core;
}

}  // namespace detail

/**
 * @brief Determines whether the given graph is k-degenerate
 * @param g Input graph
 * @param k Degeneracy bound (the class parameter)
 * @param algo Algorithm to use (default: MIN_DEGREE_PEELING)
 * @return DegenerateResult
 *
 * The empty graph is k-degenerate for every k (even negative); a
 * nonempty graph with k < 0 is not. k = 0 accepts exactly the edgeless
 * graphs, k = 1 the forests, and k >= n-1 every graph on n vertices.
 * O(n + m) time.
 */
inline DegenerateResult check_degenerate(const Graph& g, int k,
    DegenerateAlgorithm algo = DegenerateAlgorithm::MIN_DEGREE_PEELING) {
    (void)algo;
    DegenerateResult res;
    const int n = g.n;
    if (n == 0) {
        res.is_degenerate = true;
        res.degeneracy = -1;
        res.removal_order.assign(1, 0);
        return res;
    }
    std::vector<int> order;
    res.degeneracy = detail::degenerate_peel(g, order);
    if (k >= 0 && res.degeneracy <= k) {
        res.is_degenerate = true;
        res.removal_order.swap(order);
    } else {
        res.core = detail::degenerate_core(g, k);
    }
    return res;
}

}  // namespace graph_recognition

#endif
