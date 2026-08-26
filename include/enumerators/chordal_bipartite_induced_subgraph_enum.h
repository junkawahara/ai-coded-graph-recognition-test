#ifndef GRAPH_RECOGNITION_CHORDAL_BIPARTITE_INDUCED_SUBGRAPH_ENUM_H
#define GRAPH_RECOGNITION_CHORDAL_BIPARTITE_INDUCED_SUBGRAPH_ENUM_H

/**
 * @file chordal_bipartite_induced_subgraph_enum.h
 * @brief Enumeration of the chordal bipartite induced subgraphs of a given
 *        graph (Kurita--Wasa--Arimura--Uno ECB reverse search)
 *
 * Enumerates every vertex subset X of V(G) such that G[X] is chordal
 * bipartite, for a given host graph G.  Unlike the spanning-subgraph
 * enumerators (chordal_subgraph_enum.h), the output is a family of **vertex
 * sets**: the induced subgraph is determined by X, so no edge list is stored
 * and EnumeratedGraph is deliberately not reused.  The empty set is always a
 * solution and is emitted first.
 *
 * The search is a reverse search on the characterization of the paper: a
 * graph is chordal bipartite if and only if it has a chordal bipartite
 * elimination ordering, i.e. it can be emptied by repeatedly removing a
 * weak-simplicial vertex (one whose neighborhood is independent and totally
 * ordered by inclusion).  The parent of a nonempty solution X removes the
 * largest weak-simplicial vertex of G[X], so X u {v} is a child of X exactly
 * when v is weak-simplicial in G[X u {v}] and is the largest such vertex.
 *
 * No recognizer call is needed to test a child: if X is a solution and v is
 * weak-simplicial in G[X u {v}], then G[X u {v}] is chordal bipartite
 * automatically.  Independence of N(v) rules out triangles through v,
 * comparability of the neighbors chords every longer cycle through v, and a
 * bad cycle avoiding v would already contradict G[X] being a solution.
 *
 * This is the simple variant of the algorithm: the weak-simplicial sets are
 * recomputed from scratch instead of being maintained differentially, so the
 * paper's amortized O(k t Delta^2) bound (candidate set C(X), UpdateWS /
 * UpdateAWS, degeneracy ordering) does not apply here.  Only the delay is
 * affected; the enumerated family is the same.
 *
 * Reference: K. Kurita, K. Wasa, H. Arimura, and T. Uno, "An Efficient
 * Algorithm for Enumerating Chordal Bipartite Induced Subgraphs in Sparse
 * Graphs," COCOON 2019, LNCS 11653, 339--351.
 * DOI: 10.1007/978-3-030-26176-4_28 (arXiv:1903.02161)
 */

#include <algorithm>
#include <cstddef>
#include <vector>

#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for chordal bipartite induced subgraph enumeration
 */
enum class ChordalBipartiteInducedSubgraphEnumAlgorithm {
    KURITA_WASA_ARIMURA_UNO = 0 /**< ECB reverse search (simple variant) */
};

/**
 * @brief Result of chordal bipartite induced subgraph enumeration
 */
struct ChordalBipartiteInducedSubgraphEnumerationResult {
    /** @brief Enumerated vertex sets, each sorted ascending and carrying the
     *         host's labels.  The empty set is always the first entry. */
    std::vector<std::vector<int>> vertex_sets;
};

namespace detail {

/**
 * @brief Tests N(a) inter X' is a subset of N(b) inter X'
 * @param g Host graph
 * @param in_sub Membership bitmap of X' (indices 1..n)
 * @param a Vertex whose neighborhood must be contained
 * @param b Vertex whose neighborhood must contain it
 *
 * The caller has already checked that a and b are non-adjacent, so b is never
 * scanned as a member of N(a) and "b in N(b)" is never asked.
 */
inline bool chordal_bipartite_induced_subgraph_neighborhood_subset(
    const Graph& g, const std::vector<char>& in_sub, int a, int b) {
    for (std::size_t i = 0; i < g.adj[a].size(); ++i) {
        const int x = g.adj[a][i];
        if (in_sub[x] && !g.adj_set[b].count(x)) return false;
    }
    return true;
}

/**
 * @brief Tests whether u is weak-simplicial in G[X']
 * @param g Host graph
 * @param in_sub Membership bitmap of X' (indices 1..n); u must be a member
 * @param u Vertex to test
 *
 * u is weak-simplicial when N(u) inter X' is an independent set whose members
 * are pairwise comparable by inclusion of their neighborhoods inside X'.  A
 * vertex isolated in G[X'] is vacuously weak-simplicial.
 */
inline bool chordal_bipartite_induced_subgraph_is_weak_simplicial(
    const Graph& g, const std::vector<char>& in_sub, int u) {
    std::vector<int> nb;
    for (std::size_t i = 0; i < g.adj[u].size(); ++i) {
        const int x = g.adj[u][i];
        if (in_sub[x]) nb.push_back(x);
    }

    for (std::size_t i = 0; i < nb.size(); ++i) {
        for (std::size_t j = i + 1; j < nb.size(); ++j) {
            if (g.adj_set[nb[i]].count(nb[j])) return false;
            if (!chordal_bipartite_induced_subgraph_neighborhood_subset(
                    g, in_sub, nb[i], nb[j]) &&
                !chordal_bipartite_induced_subgraph_neighborhood_subset(
                    g, in_sub, nb[j], nb[i])) {
                return false;
            }
        }
    }
    return true;
}

/** @brief Collects the enumerated vertex sets into a vector */
struct ChordalBipartiteInducedSubgraphAppend {
    std::vector<std::vector<int>>* out;
    void operator()(const std::vector<int>& vertices) {
        out->push_back(vertices);
    }
};

/**
 * @brief Reverse-search DFS over the family tree of solutions
 * @param g Host graph
 * @param in_sub Membership bitmap of the current solution X
 * @param members The current solution X, kept sorted ascending
 * @param cb Callback invoked once per solution
 *
 * X is never copied: the recursion pushes v into the shared bitmap and the
 * shared sorted vector and undoes both on the way out.
 */
template <typename Callback>
inline void chordal_bipartite_induced_subgraph_dfs(const Graph& g,
                                                   std::vector<char>& in_sub,
                                                   std::vector<int>& members,
                                                   Callback& cb) {
    cb(members);

    for (int v = 1; v <= g.n; ++v) {
        if (in_sub[v]) continue;

        // Both weak-simpliciality tests are relative to G[X u {v}], so v joins
        // the bitmap before either of them runs.
        in_sub[v] = 1;

        bool is_child =
            chordal_bipartite_induced_subgraph_is_weak_simplicial(g, in_sub, v);
        if (is_child) {
            // v has to be the largest weak-simplicial vertex of G[X u {v}].
            // WS(X u {v}) is a subset of X u {v}, so only the members of X
            // above v can beat it; members is sorted, so walk it backwards.
            for (std::size_t i = members.size(); i-- > 0;) {
                if (members[i] < v) break;
                if (chordal_bipartite_induced_subgraph_is_weak_simplicial(
                        g, in_sub, members[i])) {
                    is_child = false;
                    break;
                }
            }
        }

        if (is_child) {
            const std::size_t pos = static_cast<std::size_t>(
                std::lower_bound(members.begin(), members.end(), v) -
                members.begin());
            members.insert(members.begin() + pos, v);
            chordal_bipartite_induced_subgraph_dfs(g, in_sub, members, cb);
            members.erase(members.begin() + pos);
        }

        in_sub[v] = 0;
    }
}

template <typename Callback>
inline void enumerate_chordal_bipartite_induced_subgraphs_ecb_cb(const Graph& g,
                                                                 Callback& cb) {
    std::vector<char> in_sub(g.n + 1, 0);
    std::vector<int> members;
    chordal_bipartite_induced_subgraph_dfs(g, in_sub, members, cb);
}

}  // namespace detail

/**
 * @brief Enumerates every chordal bipartite induced subgraph of a given graph
 * @param g Host graph (1-indexed, vertex set {1, ..., n})
 * @param algo Algorithm selector; KURITA_WASA_ARIMURA_UNO is the only algorithm
 * @return ChordalBipartiteInducedSubgraphEnumerationResult
 *
 * The solutions are the vertex subsets X of {1, ..., n} for which G[X] is
 * chordal bipartite, reported as sorted vertex lists; the empty set comes
 * first.  Because the class is hereditary, a chordal bipartite host makes
 * every one of its 2^n subsets a solution, so prefer
 * enumerate_chordal_bipartite_induced_subgraphs_cb() when only aggregation is
 * needed.
 */
inline ChordalBipartiteInducedSubgraphEnumerationResult
enumerate_chordal_bipartite_induced_subgraphs(
    const Graph& g,
    ChordalBipartiteInducedSubgraphEnumAlgorithm algo =
        ChordalBipartiteInducedSubgraphEnumAlgorithm::KURITA_WASA_ARIMURA_UNO) {
    (void)algo;
    ChordalBipartiteInducedSubgraphEnumerationResult result;
    detail::ChordalBipartiteInducedSubgraphAppend cb;
    cb.out = &result.vertex_sets;
    detail::enumerate_chordal_bipartite_induced_subgraphs_ecb_cb(g, cb);
    return result;
}

/**
 * @brief Streaming enumeration of the chordal bipartite induced subgraphs
 * @param g Host graph (1-indexed, vertex set {1, ..., n})
 * @param cb Callback invoked as cb(const std::vector<int>&) per solution
 * @param algo Algorithm selector; KURITA_WASA_ARIMURA_UNO is the only algorithm
 *
 * Memory-friendly alternative to
 * enumerate_chordal_bipartite_induced_subgraphs(): the vertex set is handed to
 * the callback as it is generated and never stored, so memory stays O(n + m)
 * instead of O(number of solutions * n).  The callback receives the search's
 * own buffer, which is modified as soon as it returns: copy it to keep it.
 */
template <typename Callback>
inline void enumerate_chordal_bipartite_induced_subgraphs_cb(
    const Graph& g, Callback&& cb,
    ChordalBipartiteInducedSubgraphEnumAlgorithm algo =
        ChordalBipartiteInducedSubgraphEnumAlgorithm::KURITA_WASA_ARIMURA_UNO) {
    (void)algo;
    detail::enumerate_chordal_bipartite_induced_subgraphs_ecb_cb(g, cb);
}

}  // namespace graph_recognition

#endif
