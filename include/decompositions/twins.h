#ifndef GRAPH_RECOGNITION_TWINS_H
#define GRAPH_RECOGNITION_TWINS_H

/**
 * @file twins.h
 * @brief Twin classes and twin quotient graphs
 *
 * Two distinct vertices are *true twins* when N[u] = N[v] (they are then
 * adjacent) and *false twins* when N(u) = N(v) (they are then non-adjacent).
 * Being a twin of either kind is an equivalence relation: a vertex cannot be a
 * true twin of one vertex and a false twin of another, because u ~ v with
 * N[u] = N[v] and N(v) = N(w) would force w in N(v), contradicting v !~ w.
 * Every class is therefore a clique (true twins) or an independent set (false
 * twins), and the quotient is well defined.
 *
 * The true-twin classes are exactly the *critical cliques* used by the leaf
 * power recognizers; the true/false fixpoint contraction is the reduction that
 * shrinks circle graph instances before the Naji system is built.
 *
 * A single contraction round already reaches a fixpoint when only one kind is
 * contracted: two distinct true-twin classes with equal closed neighbourhoods
 * in the quotient would have equal closed neighbourhoods in the original graph
 * as well, so they would not be distinct. Mixing the two kinds does need
 * iteration -- contracting the false twins of C4 yields K2, whose two vertices
 * are true twins.
 */

#include "util/dsu.h"
#include "util/graph.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Which twin relation to contract
 */
enum class TwinKind {
    TRUE_TWINS,  /**< N[u] = N[v] only (critical cliques) */
    FALSE_TWINS, /**< N(u) = N(v) only */
    BOTH         /**< either relation, contracted to a fixpoint (default) */
};

/**
 * @brief A graph quotiented by twin classes
 */
struct TwinQuotientResult {
    Graph quotient;                        /**< quotient graph, vertices 1..k */
    std::vector<int> block_of;             /**< block_of[v] = quotient vertex of v, in [1, k] (size n+1) */
    std::vector<std::vector<int>> members; /**< members[i] = original vertices of quotient vertex i, ascending (size k+1) */
};

namespace detail {

/**
 * @brief Assigns twin class ids to the vertices of a graph
 * @param g Input graph
 * @param kind Twin relation to use
 * @param num_classes Set to the number of classes
 * @return class_of[v] in [1, num_classes] (size n+1)
 *
 * Classes are numbered by increasing smallest member, which makes the
 * numbering independent of the traversal order.
 */
inline std::vector<int> twin_classes(const Graph& g, TwinKind kind, int& num_classes) {
    int n = g.n;
    DSU dsu(n);

    std::vector<std::vector<int>> nbr(n + 1);
    for (int v = 1; v <= n; ++v) {
        nbr[v] = g.adj[v];
        std::sort(nbr[v].begin(), nbr[v].end());
    }

    if (kind == TwinKind::FALSE_TWINS || kind == TwinKind::BOTH) {
        std::map<std::vector<int>, int> seen;
        for (int v = 1; v <= n; ++v) {
            std::pair<std::map<std::vector<int>, int>::iterator, bool> ins =
                seen.insert(std::make_pair(nbr[v], v));
            if (!ins.second) dsu.unite(ins.first->second, v);
        }
    }
    if (kind == TwinKind::TRUE_TWINS || kind == TwinKind::BOTH) {
        std::map<std::vector<int>, int> seen;
        for (int v = 1; v <= n; ++v) {
            std::vector<int> key = nbr[v];
            key.insert(std::lower_bound(key.begin(), key.end(), v), v);
            std::pair<std::map<std::vector<int>, int>::iterator, bool> ins =
                seen.insert(std::make_pair(key, v));
            if (!ins.second) dsu.unite(ins.first->second, v);
        }
    }

    std::vector<int> class_of(n + 1, 0);
    num_classes = 0;
    for (int v = 1; v <= n; ++v) {
        int root = dsu.find(v);
        if (class_of[root] == 0) class_of[root] = ++num_classes;
        class_of[v] = class_of[root];
    }
    // class_of[root] was assigned when the root was first reached, which is at
    // the smallest member only if the root itself is smallest. Renumber by
    // smallest member so the ids do not depend on the union-by-rank shape.
    std::vector<int> smallest(num_classes + 1, 0);
    for (int v = n; v >= 1; --v) smallest[class_of[v]] = v;
    std::vector<std::pair<int, int>> by_min;
    by_min.reserve(num_classes);
    for (int c = 1; c <= num_classes; ++c) by_min.push_back(std::make_pair(smallest[c], c));
    std::sort(by_min.begin(), by_min.end());
    std::vector<int> renum(num_classes + 1, 0);
    for (size_t i = 0; i < by_min.size(); ++i) renum[by_min[i].second] = (int)i + 1;
    for (int v = 1; v <= n; ++v) class_of[v] = renum[class_of[v]];
    return class_of;
}

/** @brief Builds the quotient of g under a class assignment */
inline TwinQuotientResult build_quotient(const Graph& g, const std::vector<int>& class_of,
                                         int num_classes) {
    TwinQuotientResult res;
    res.block_of = class_of;
    res.members.assign(num_classes + 1, std::vector<int>());
    for (int v = 1; v <= g.n; ++v) res.members[class_of[v]].push_back(v);

    std::vector<std::pair<int, int>> edges;
    std::vector<int> seen(num_classes + 1, 0);
    for (int ci = 1; ci <= num_classes; ++ci) {
        if (res.members[ci].empty()) continue;
        int rep = res.members[ci][0];
        for (size_t i = 0; i < g.adj[rep].size(); ++i) {
            int cj = class_of[g.adj[rep][i]];
            if (cj <= ci || seen[cj] == ci) continue;
            seen[cj] = ci;
            edges.push_back(std::make_pair(ci, cj));
        }
    }
    res.quotient = Graph(num_classes, edges);
    return res;
}

} // namespace detail

/**
 * @brief Contracts twin classes
 * @param g Input graph
 * @param kind Twin relation to contract (default: BOTH, iterated to a fixpoint)
 * @return TwinQuotientResult
 *
 * With TRUE_TWINS or FALSE_TWINS a single round already is a fixpoint; with
 * BOTH the rounds are repeated until the graph stops shrinking, and the vertex
 * maps of the rounds are composed.
 */
inline TwinQuotientResult contract_twins(const Graph& g, TwinKind kind = TwinKind::BOTH) {
    int num_classes = 0;
    std::vector<int> class_of = detail::twin_classes(g, kind, num_classes);
    TwinQuotientResult res = detail::build_quotient(g, class_of, num_classes);
    if (kind != TwinKind::BOTH) return res;

    while (true) {
        int next_classes = 0;
        std::vector<int> next_of = detail::twin_classes(res.quotient, kind, next_classes);
        if (next_classes == res.quotient.n) break;
        TwinQuotientResult next = detail::build_quotient(res.quotient, next_of, next_classes);
        // Compose the maps: an original vertex lands in the class its previous
        // quotient vertex was merged into.
        std::vector<int> composed(g.n + 1, 0);
        for (int v = 1; v <= g.n; ++v) composed[v] = next_of[res.block_of[v]];
        next.block_of = composed;
        next.members.assign(next_classes + 1, std::vector<int>());
        for (int v = 1; v <= g.n; ++v) next.members[composed[v]].push_back(v);
        res = next;
    }
    return res;
}

/**
 * @brief Contracts the critical cliques (true twin classes) of a graph
 * @param g Input graph
 * @return TwinQuotientResult whose blocks are the critical cliques
 *
 * The critical clique graph is the quotient; it is the structure the leaf
 * power recognizers search over.
 */
inline TwinQuotientResult critical_clique_quotient(const Graph& g) {
    return contract_twins(g, TwinKind::TRUE_TWINS);
}

} // namespace graph_recognition

#endif
