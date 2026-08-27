#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_UNLABELED_ENUM_H

/**
 * @file triangle_free_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic triangle-free graphs
 *
 * Enumerates one representative per isomorphism class of triangle-free
 * graphs on n vertices by McKay's canonical construction path method, the
 * algorithm behind geng -t. Graphs are grown one vertex at a time; adding a
 * vertex keeps the graph triangle-free if and only if its neighborhood is an
 * independent set, so the triangle-forbidding pruning is an independence
 * test on the candidate neighborhood, with no recognizer call.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 *
 * Number of non-isomorphic triangle-free graphs on n vertices =
 * OEIS A006785(n): 1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172, ...
 * The connected ones are counted by A024607: 1, 1, 1, 3, 6, 19, 59, 267, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path; geng -t);
 *   Colbourn, Read, "Orderly algorithms for generating restricted classes
 *   of graphs," J. Graph Theory 3(2), 1979; OEIS A006785, A024607
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic triangle-free enumeration
 */
enum class TriangleFreeUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (geng -t style) */
};

/**
 * @brief An enumerated triangle-free graph
 */
struct TriangleFreeUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic triangle-free enumeration
 */
struct TriangleFreeUnlabeledEnumerationResult {
    std::vector<TriangleFreeUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Canonicalization output: canonical form plus the last-vertex orbit
 *
 * `form[p]` is the adjacency of the vertex placed at position p + 1 to the
 * positions before it, packed into an integer (bit q = adjacent to position
 * q), for p = 0, ..., k - 2; the lexicographically smallest such vector
 * over all orderings is the canonical form. `last_orbit` has bit v set iff
 * some ordering achieving the canonical form places original vertex v last.
 * All orderings achieving the canonical form differ by an automorphism, so
 * `last_orbit` is exactly one automorphism orbit — the canonical-deletion
 * orbit of the canonical construction path method.
 */
struct TriangleFreeUnlabeledCanonResult {
    std::vector<unsigned long long> form;   /**< Canonical adjacency rows */
    unsigned long long last_orbit;          /**< Orbit of the canonically last vertex */
};

/** @brief Branch-and-bound state for the canonical-ordering search */
struct TriangleFreeUnlabeledCanonState {
    int k;                                          /**< Number of vertices */
    const std::vector<unsigned long long>* adj;     /**< Adjacency bitmasks */
    std::vector<int> chosen;                        /**< chosen[p] = vertex at position p */
    std::vector<char> used;                         /**< Vertex already placed */
    std::vector<unsigned long long> cur;            /**< Rows of the current ordering */
    bool have_best;                                 /**< A complete ordering was reached */
    std::vector<unsigned long long> best;           /**< Best (smallest) rows so far */
    unsigned long long last_orbit;                  /**< Vertices placed last by a best ordering */
};

/**
 * @brief DFS over vertex orderings, pruned against the best rows so far
 * @param state Search state
 * @param pos Next position to fill (0-based)
 * @param strictly_less Current prefix is already strictly below `best`
 *
 * Fills positions left to right. While the current prefix ties with `best`,
 * a candidate row above `best[pos - 1]` is pruned and a row below it marks
 * the branch strictly smaller; once strictly smaller, the branch runs to
 * its leaves unpruned, where a full comparison decides between replacing
 * `best` and discarding. A leaf that ties contributes its last vertex to
 * `last_orbit`.
 */
inline void triangle_free_unlabeled_canon_dfs(
    TriangleFreeUnlabeledCanonState& state, int pos, bool strictly_less) {
    if (pos == state.k) {
        // `best` may have shrunk since this branch was flagged strictly
        // smaller, so the leaf always re-compares in full; `strictly_less`
        // is only a pruning license, never a proof of a new minimum.
        (void)strictly_less;
        int last = state.chosen[state.k - 1];
        if (!state.have_best || state.cur < state.best) {
            state.best = state.cur;
            state.have_best = true;
            state.last_orbit = 1ULL << last;
        } else if (state.cur == state.best) {
            state.last_orbit |= 1ULL << last;
        }
        return;
    }
    for (int v = 0; v < state.k; ++v) {
        if (state.used[v]) continue;
        unsigned long long row = 0;
        const unsigned long long adj_v = (*state.adj)[v];
        for (int q = 0; q < pos; ++q) {
            if ((adj_v >> state.chosen[q]) & 1ULL) row |= 1ULL << q;
        }
        bool child_strictly_less = strictly_less;
        if (pos >= 1 && state.have_best && !strictly_less) {
            unsigned long long best_row = state.best[pos - 1];
            if (row > best_row) continue;
            if (row < best_row) child_strictly_less = true;
        }
        state.used[v] = 1;
        state.chosen[pos] = v;
        if (pos >= 1) state.cur[pos - 1] = row;
        triangle_free_unlabeled_canon_dfs(state, pos + 1, child_strictly_less);
        state.used[v] = 0;
    }
}

/**
 * @brief Computes the canonical form and canonical-deletion orbit of a graph
 * @param k Number of vertices (1 <= k <= 63)
 * @param adj Adjacency bitmasks over vertices 0, ..., k-1
 * @return TriangleFreeUnlabeledCanonResult
 */
inline TriangleFreeUnlabeledCanonResult triangle_free_unlabeled_canonicalize(
    int k, const std::vector<unsigned long long>& adj) {
    TriangleFreeUnlabeledCanonState state;
    state.k = k;
    state.adj = &adj;
    state.chosen.assign(k, 0);
    state.used.assign(k, 0);
    state.cur.assign(k > 0 ? k - 1 : 0, 0);
    state.have_best = false;
    state.last_orbit = 0;
    triangle_free_unlabeled_canon_dfs(state, 0, false);
    TriangleFreeUnlabeledCanonResult res;
    res.form = state.best;
    res.last_orbit = state.last_orbit;
    return res;
}

/** @brief Returns true iff the graph on vertices 0, ..., k-1 is connected */
inline bool triangle_free_unlabeled_is_connected(
    int k, const std::vector<unsigned long long>& adj) {
    if (k <= 1) return true;
    unsigned long long reached = 1ULL, frontier = 1ULL;
    while (frontier != 0) {
        unsigned long long next = 0;
        for (int v = 0; v < k; ++v) {
            if ((frontier >> v) & 1ULL) next |= adj[v];
        }
        frontier = next & ~reached;
        reached |= frontier;
    }
    return reached == (k == 64 ? ~0ULL : (1ULL << k) - 1ULL);
}

/** @brief Converts the bitmask adjacency to a sorted 1-indexed edge list */
inline TriangleFreeUnlabeledEnumeratedGraph triangle_free_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    TriangleFreeUnlabeledEnumeratedGraph g;
    g.n = k;
    for (int u = 0; u < k; ++u) {
        for (int v = u + 1; v < k; ++v) {
            if ((adj[u] >> v) & 1ULL) {
                g.edges.push_back(std::make_pair(u + 1, v + 1));
            }
        }
    }
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over the independent
 * subsets of the current vertex set (independence of S is exactly what keeps
 * the graph triangle-free). A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void triangle_free_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<TriangleFreeUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || triangle_free_unlabeled_is_connected(k, adj)) {
            results.push_back(triangle_free_unlabeled_build_graph(k, adj));
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        bool independent = true;
        for (int u = 0; u < k && independent; ++u) {
            if (((s >> u) & 1ULL) && (adj[u] & s) != 0) independent = false;
        }
        if (!independent) continue;

        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        TriangleFreeUnlabeledCanonResult canon =
            triangle_free_unlabeled_canonicalize(k + 1, adj);
        if (((canon.last_orbit >> k) & 1ULL) &&
            child_forms.insert(canon.form).second) {
            triangle_free_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
                                             results);
        }

        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic triangle-free graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return TriangleFreeUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A006785(n) graphs
 * (1, 2, 3, 7, 14, 38, 107, 410, ... for n = 1, 2, ...), or A024607(n)
 * (1, 1, 1, 3, 6, 19, 59, 267, ...) with @p connected_only. n = 0 yields
 * the single empty graph in both modes; negative n and n >= 64 yield
 * nothing.
 *
 * @note The canonicalization is exact branch-and-bound over vertex
 *       orderings (worst case k! on vertex-transitive graphs such as the
 *       empty graph), so the enumeration is practical to about n = 10
 *       (n = 9 takes about 2 seconds, n = 10 about half a minute;
 *       A006785(10) = 12172).
 */
inline TriangleFreeUnlabeledEnumerationResult
enumerate_triangle_free_unlabeled_graphs(
    int n, bool connected_only = false,
    TriangleFreeUnlabeledEnumAlgorithm algo =
        TriangleFreeUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    TriangleFreeUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        TriangleFreeUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::triangle_free_unlabeled_enum_dfs(1, adj, n, connected_only,
                                             result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
