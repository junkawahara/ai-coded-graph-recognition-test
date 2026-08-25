#ifndef GRAPH_RECOGNITION_PLANAR_H
#define GRAPH_RECOGNITION_PLANAR_H

/**
 * @file planar.h
 * @brief Planar graph recognition
 *
 * Two algorithms are available:
 *   - LEFT_RIGHT (default): the linear-time left-right planarity criterion
 *     (de Fraysseix, Ossona de Mendez, Rosenstiehl; see planarity_lr.h).
 *   - MINOR_CHECK: Kuratowski's theorem via explicit K5/K3,3 minor search.
 *     Exact but exponential in the worst case, and only practical for tiny
 *     graphs; kept as a cross-check for the default algorithm.
 * Both share a fast filter using the edge count upper bound m <= 3n-6.
 */

#include "graph.h"
#include "forbidden_subgraph.h"
#include "minor.h"
#include "planarity_lr.h"

#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for planar graph recognition
 */
enum class PlanarAlgorithm {
    LEFT_RIGHT,  /**< Left-right planarity criterion, O(n + m) (default) */
    MINOR_CHECK  /**< K5/K3,3 minor check, exponential worst case */
};

/**
 * @brief Result of planar graph recognition
 */
struct PlanarResult {
    bool is_planar = false; /**< true if the graph is a planar graph */
    Obstruction obstruction; /**< NO certificate: a K5_MINOR or K33_MINOR. Left empty
                                  by both variants -- LEFT_RIGHT decides from conflict
                                  pairs with no Kuratowski extraction, and MINOR_CHECK
                                  rejects dense graphs on the edge count alone without
                                  ever looking for a model. Use
                                  build_planar_obstruction(). Valid only when
                                  is_planar == false */
};

namespace detail_planar {

/** @brief Planarity via explicit K5/K3,3 minor search (exponential) */
inline bool is_planar_minor(const Graph& g) {
    int n = g.n;
    if (n <= 4) return true;

    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    // Edge count upper bound for simple planar graphs
    if (n >= 3 && m > 3LL * n - 6) return false;

    // Check K5/K3,3 minor for each connected component
    std::vector<bool> visited(n + 1, false);
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;

        // Collect connected component via BFS
        std::vector<int> comp;
        std::queue<int> q;
        q.push(s);
        visited[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            comp.push_back(v);
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int w = g.adj[v][i];
                if (!visited[w]) {
                    visited[w] = true;
                    q.push(w);
                }
            }
        }

        int cn = static_cast<int>(comp.size());
        if (cn <= 4) continue;

        // Edge count check for component
        long long cm = 0;
        for (size_t i = 0; i < comp.size(); ++i)
            cm += (long long)g.adj[comp[i]].size();
        cm /= 2;
        if (cm > 3LL * cn - 6) return false;

        // Build subgraph of component and check for minor
        // Renumber vertices to 1..cn
        std::vector<int> id(n + 1, 0);
        for (int i = 0; i < cn; ++i) id[comp[i]] = i + 1;

        std::vector<std::pair<int, int> > edges;
        for (int i = 0; i < cn; ++i) {
            int u = comp[i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int v = g.adj[u][j];
                if (id[v] > id[u]) {
                    edges.push_back(std::make_pair(id[u], id[v]));
                }
            }
        }

        Graph sg(cn, edges);
        detail_minor::MinorState st = detail_minor::build_minor_state(sg);

        detail_minor::MinorChecker k5(detail_minor::MinorTarget::K5);
        if (k5.has_minor(st)) return false;

        detail_minor::MinorChecker k33(detail_minor::MinorTarget::K33);
        if (k33.has_minor(st)) return false;
    }

    return true;
}

} // namespace detail_planar

/**
 * @brief Determines whether the graph is a planar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: LEFT_RIGHT)
 * @return PlanarResult
 */
inline PlanarResult check_planar(const Graph& g,
    PlanarAlgorithm algo = PlanarAlgorithm::LEFT_RIGHT) {
    PlanarResult res;
    if (algo == PlanarAlgorithm::MINOR_CHECK) {
        res.is_planar = detail_planar::is_planar_minor(g);
    } else {
        res.is_planar = detail_planar_lr::is_planar_lr(g);
    }
    return res;
}

/**
 * @brief Builds a NO certificate for a non-planar graph
 * @param g Input graph
 * @return A K5_MINOR or K33_MINOR whose branch sets are vertex sets of g, or an
 *         empty obstruction if g is planar
 *
 * Wagner's theorem: a graph is planar exactly when it has neither minor. The
 * search runs per connected component and records, for each contracted vertex,
 * the input vertices merged into it; those are the branch sets. Contraction
 * only ever merges two groups joined by an edge, so every branch set is
 * connected, and two groups end up adjacent exactly when the input has an edge
 * between them.
 *
 * Separated from check_planar() because the default LEFT_RIGHT recognizer is
 * linear and this search is exponential in the worst case.
 */
inline Obstruction build_planar_obstruction(const Graph& g) {
    Obstruction o;
    int n = g.n;
    if (n <= 4) return o;

    std::vector<bool> visited(n + 1, false);
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;

        std::vector<int> comp;
        std::queue<int> q;
        q.push(s);
        visited[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            comp.push_back(v);
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int w = g.adj[v][i];
                if (visited[w]) continue;
                visited[w] = true;
                q.push(w);
            }
        }

        int cn = static_cast<int>(comp.size());
        if (cn <= 4) continue;

        std::vector<int> id(n + 1, 0);
        for (int i = 0; i < cn; ++i) id[comp[i]] = i + 1;
        std::vector<std::pair<int, int> > edges;
        for (int i = 0; i < cn; ++i) {
            int u = comp[i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int v = g.adj[u][j];
                if (id[v] > id[u]) edges.push_back(std::make_pair(id[u], id[v]));
            }
        }

        Graph sg(cn, edges);
        detail_minor::MinorState st = detail_minor::build_minor_state(sg);

        std::vector<std::vector<int> > sets;
        ObstructionKind kind = ObstructionKind::NONE;
        detail_minor::MinorChecker k5(detail_minor::MinorTarget::K5);
        if (k5.find_model(st, &sets)) {
            kind = ObstructionKind::K5_MINOR;
        } else {
            detail_minor::MinorChecker k33(detail_minor::MinorTarget::K33);
            if (k33.find_model(st, &sets)) kind = ObstructionKind::K33_MINOR;
        }
        if (kind == ObstructionKind::NONE) continue;

        o.kind = kind;
        for (size_t i = 0; i < sets.size(); ++i) {
            std::vector<int> branch;
            for (size_t j = 0; j < sets[i].size(); ++j) {
                branch.push_back(comp[sets[i][j] - 1]);
            }
            o.vertex_sets.push_back(branch);
        }
        return o;
    }
    return o;
}

} // namespace graph_recognition

#endif
