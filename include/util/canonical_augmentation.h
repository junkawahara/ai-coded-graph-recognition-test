#ifndef GRAPH_RECOGNITION_CANONICAL_AUGMENTATION_H
#define GRAPH_RECOGNITION_CANONICAL_AUGMENTATION_H

/**
 * @file canonical_augmentation.h
 * @brief Primitives for McKay-style canonical construction path enumeration
 *
 * Shared machinery for the unlabeled (non-isomorphic) enumerators that grow
 * graphs one vertex at a time and reject isomorphs by the canonical-parent
 * test (`geng`-style canonical augmentation): a canonicalization that also
 * reports the automorphism orbit of the canonically last vertex, plus the
 * bitmask-graph helpers those searches need.
 *
 * Graphs are represented as a `std::vector<unsigned long long>` of adjacency
 * bitmasks over vertices 0, ..., k-1 (bit v of row u set iff u ~ v), so
 * k <= 63; the enumerators guard n < 64 for that reason.
 *
 * Reference: McKay, "Isomorph-free exhaustive generation," J. Algorithms
 * 26(2):306--324, 1998
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

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
struct CanonicalAugmentationCanon {
    std::vector<unsigned long long> form;   /**< Canonical adjacency rows */
    unsigned long long last_orbit;          /**< Orbit of the canonically last vertex */
};

namespace detail {

/** @brief Branch-and-bound state for the canonical-ordering search */
struct CanonicalAugmentationCanonState {
    int k;                                          /**< Number of vertices */
    bool directed;                                  /**< Pack two bits per earlier position (digraph rows) */
    const std::vector<unsigned long long>* adj;     /**< Adjacency bitmasks (out-adjacency when directed) */
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
inline void canonical_augmentation_canon_dfs(
    CanonicalAugmentationCanonState& state, int pos, bool strictly_less) {
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
        if (state.directed) {
            // Two bits per earlier position: bit 2q = arc v -> chosen[q],
            // bit 2q + 1 = arc chosen[q] -> v, so the form distinguishes
            // non-adjacency from a reversed arc (which a single bit cannot).
            for (int q = 0; q < pos; ++q) {
                int w = state.chosen[q];
                if ((adj_v >> w) & 1ULL) row |= 1ULL << (2 * q);
                if (((*state.adj)[w] >> v) & 1ULL) row |= 1ULL << (2 * q + 1);
            }
        } else {
            for (int q = 0; q < pos; ++q) {
                if ((adj_v >> state.chosen[q]) & 1ULL) row |= 1ULL << q;
            }
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
        canonical_augmentation_canon_dfs(state, pos + 1, child_strictly_less);
        state.used[v] = 0;
    }
}

}  // namespace detail

/**
 * @brief Computes the canonical form and canonical-deletion orbit of a graph
 * @param k Number of vertices (1 <= k <= 63)
 * @param adj Adjacency bitmasks over vertices 0, ..., k-1
 * @return CanonicalAugmentationCanon
 *
 * @note Exact branch-and-bound over all vertex orderings, so the worst case
 *       is k! on vertex-transitive graphs (the empty graph, complete graph,
 *       cycles); this is what bounds the practical range of the unlabeled
 *       enumerators built on it.
 */
inline CanonicalAugmentationCanon canonicalize_bitmask_graph(
    int k, const std::vector<unsigned long long>& adj) {
    detail::CanonicalAugmentationCanonState state;
    state.k = k;
    state.directed = false;
    state.adj = &adj;
    state.chosen.assign(k, 0);
    state.used.assign(k, 0);
    state.cur.assign(k > 0 ? k - 1 : 0, 0);
    state.have_best = false;
    state.last_orbit = 0;
    detail::canonical_augmentation_canon_dfs(state, 0, false);
    CanonicalAugmentationCanon res;
    res.form = state.best;
    res.last_orbit = state.last_orbit;
    return res;
}

/**
 * @brief Computes the canonical form and canonical-deletion orbit of a digraph
 * @param k Number of vertices (1 <= k <= 33)
 * @param out Out-adjacency bitmasks over vertices 0, ..., k-1 (bit v of row
 *        u set iff arc u -> v)
 * @return CanonicalAugmentationCanon
 *
 * Directed sibling of `canonicalize_bitmask_graph`: `form[p]` packs **two**
 * bits per earlier position q (bit 2q = arc from the vertex at position
 * p + 1 to the one at position q, bit 2q + 1 = the reverse arc), so the
 * form records the state of every ordered pair — non-adjacency, either
 * single arc, or both arcs — and is a complete isomorphism invariant for
 * simple digraphs, with tied orderings differing by automorphisms. The
 * single-bit undirected form is sound on out-adjacency masks only for
 * tournaments (complete underlying graph), where an unset bit is exactly
 * the reversed arc; this function is the general-digraph replacement.
 * Two bits per earlier position bound the row width by 2(k - 1) <= 64,
 * hence k <= 33.
 *
 * @note Exact branch-and-bound over all vertex orderings, so the worst
 *       case is k! on vertex-transitive digraphs (the empty digraph, the
 *       complete digraph with all arc pairs, directed cycles).
 */
inline CanonicalAugmentationCanon canonicalize_bitmask_digraph(
    int k, const std::vector<unsigned long long>& out) {
    detail::CanonicalAugmentationCanonState state;
    state.k = k;
    state.directed = true;
    state.adj = &out;
    state.chosen.assign(k, 0);
    state.used.assign(k, 0);
    state.cur.assign(k > 0 ? k - 1 : 0, 0);
    state.have_best = false;
    state.last_orbit = 0;
    detail::canonical_augmentation_canon_dfs(state, 0, false);
    CanonicalAugmentationCanon res;
    res.form = state.best;
    res.last_orbit = state.last_orbit;
    return res;
}

/**
 * @brief Returns true iff the bitmask graph on vertices 0, ..., k-1 is connected
 * @param k Number of vertices (0 <= k <= 64)
 * @param adj Adjacency bitmasks
 */
inline bool bitmask_graph_connected(int k,
                                    const std::vector<unsigned long long>& adj) {
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

/**
 * @brief Converts a bitmask graph to a sorted 1-indexed edge list
 * @param k Number of vertices
 * @param adj Adjacency bitmasks
 * @return Edges (u, v) with u < v, sorted lexicographically
 */
inline std::vector<std::pair<int, int> > bitmask_graph_edges(
    int k, const std::vector<unsigned long long>& adj) {
    std::vector<std::pair<int, int> > edges;
    for (int u = 0; u < k; ++u) {
        for (int v = u + 1; v < k; ++v) {
            if ((adj[u] >> v) & 1ULL) {
                edges.push_back(std::make_pair(u + 1, v + 1));
            }
        }
    }
    return edges;
}

}  // namespace graph_recognition

#endif
