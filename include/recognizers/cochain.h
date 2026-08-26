#ifndef GRAPH_RECOGNITION_COCHAIN_H
#define GRAPH_RECOGNITION_COCHAIN_H

/**
 * @file cochain.h
 * @brief Cochain graph recognition
 *
 * Algorithms:
 *   - COMPLEMENT: complement graph construction + chain test O(n^2)
 *   - DIRECT: direct test avoiding complement construction O(n^2) (default)
 *
 * Both variants also report the partition into the two cliques and the orders
 * in which the non-neighbourhoods grow by inclusion -- the chain structure of
 * the complement. The DIRECT variant obtains them without building the
 * complement, using the complemented mode of chain.h's nested_side_order.
 */

#include "recognizers/bipartite.h"
#include "recognizers/chain.h"
#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "util/graph_utils.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for cochain graph recognition
 */
enum class CochainAlgorithm {
    COMPLEMENT, /**< chain test on the complement graph O(n^2) */
    DIRECT      /**< direct test avoiding complement construction O(n^2) (default) */
};

/**
 * @brief Result of cochain graph recognition
 */
struct CochainResult {
    bool is_cochain = false; /**< true if the graph is a cochain graph */
    std::vector<int> color;  /**< color[v] in {0, 1}: the two cliques (size n+1) */
    /**
     * @brief Vertices of color 0, ordered so that the non-neighbourhood grows by inclusion
     *
     * Valid only when is_cochain == true.
     */
    std::vector<int> x_ordering;
    /**
     * @brief Vertices of color 1, ordered so that the non-neighbourhood grows by inclusion
     *
     * Valid only when is_cochain == true.
     */
    std::vector<int> y_ordering;
    /**
     * @brief NO certificate of the complement: an ODD_CYCLE or a TWO_K2, with
     *        in_complement set
     *
     * Cochain graphs are the complements of chain graphs, so the witness is
     * chain's, read in the complement. The DIRECT variant avoids building the
     * complement to decide, but builds it to produce this -- the same O(n^2)
     * the variant already costs. Valid only when is_cochain == false.
     */
    Obstruction obstruction;
};

namespace detail {

/** @brief Complement construction + chain test (original algorithm) */
inline CochainResult check_cochain_complement(const Graph& g) {
    CochainResult res;
    res.is_cochain = false;

    Graph gc = build_complement(g);
    ChainResult cres = check_chain(gc);
    if (!cres.is_chain) {
        res.obstruction = cres.obstruction;
        res.obstruction.in_complement = true;
        if (!res.obstruction.has_witness()) res.obstruction = Obstruction();
        return res;
    }

    // The chain structure of the complement is exactly the cochain structure.
    res.color.swap(cres.color);
    res.x_ordering.swap(cres.x_ordering);
    res.y_ordering.swap(cres.y_ordering);
    res.is_cochain = true;
    return res;
}

/**
 * @brief Direct test avoiding complement construction O(n^2)
 *
 * cochain iff complement graph is bipartite + chain.
 * Complement bipartiteness = co-bipartite = partition vertices into 2 cliques.
 * Complement chain property = one side's "non-adjacency" is nested = suffix property.
 *
 * 1. BFS on complement graph for 2-coloring (O(n+m) with linked-list technique)
 * 2. Verify each color class is a clique (edge count check)
 * 3. Sort one side by ascending "non-adjacency count to the other side" -> suffix property verification
 */
inline CochainResult check_cochain_direct(const Graph& g) {
    CochainResult res;
    res.is_cochain = false;

    int n = g.n;
    if (n <= 1) {
        res.color.assign(n + 1, 0);
        for (int v = 1; v <= n; ++v) res.x_ordering.push_back(v);
        res.is_cochain = true;
        return res;
    }

    // 2-coloring via BFS on complement graph (fast non-neighbor enumeration via linked-list)
    std::vector<int> color(n + 1, -1);
    // remaining: list of vertices not yet colored (doubly-linked)
    std::vector<int> rem_prev(n + 2, 0), rem_next(n + 2, 0);
    // sentinel: 0
    rem_next[0] = 1;
    rem_prev[0] = 0;
    for (int v = 1; v <= n; ++v) {
        rem_next[v] = v + 1;
        rem_prev[v] = v - 1;
    }
    rem_next[n] = 0; // end sentinel
    rem_prev[0] = n; // not used but consistent

    // stamp array: for adjacency checks
    std::vector<unsigned char> stamped(n + 1, 0);

    // BFS queue
    std::vector<int> bfs_queue;
    bfs_queue.reserve(n);

    // Process each connected component
    while (rem_next[0] != 0) {
        int start = rem_next[0];
        // Remove start from remaining
        rem_next[0] = rem_next[start];
        if (rem_next[start] != 0) rem_prev[rem_next[start]] = 0;

        color[start] = 0;
        bfs_queue.clear();
        bfs_queue.push_back(start);

        size_t qi = 0;
        while (qi < bfs_queue.size()) {
            int v = bfs_queue[qi++];

            // Stamp v's neighbors in G
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                stamped[g.adj[v][j]] = 1;
            }

            int new_color = 1 - color[v];

            // Scan remaining list; unstamped vertices = complement graph neighbors
            int u = rem_next[0];
            while (u != 0) {
                int nxt = rem_next[u];
                if (!stamped[u]) {
                    // u is adjacent to v in complement graph
                    if (color[u] == -1) {
                        color[u] = new_color;
                        bfs_queue.push_back(u);
                        // Remove from remaining
                        rem_next[rem_prev[u]] = rem_next[u];
                        if (rem_next[u] != 0) rem_prev[rem_next[u]] = rem_prev[u];
                    } else if (color[u] != new_color) {
                        // 2-coloring impossible -> complement graph is not bipartite
                        return res;
                    }
                }
                u = nxt;
            }

            // Detect conflicts with already-colored and removed vertices
            // (complement graph neighbors already removed from remaining list)
            for (size_t pi = 0; pi < qi; ++pi) {
                int prev = bfs_queue[pi];
                if (!stamped[prev] && prev != v) {
                    // prev is a complement neighbor of v and already colored
                    if (color[prev] != new_color) {
                        // Clear stamps before returning
                        for (size_t j2 = 0; j2 < g.adj[v].size(); ++j2)
                            stamped[g.adj[v][j2]] = 0;
                        return res;
                    }
                }
            }

            // Clear stamps
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                stamped[g.adj[v][j]] = 0;
            }
        }
    }

    // Split into color classes
    std::vector<int> left, right;
    left.reserve(n);
    right.reserve(n);
    for (int v = 1; v <= n; ++v) {
        if (color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    // Verify each color class is a clique (= edge count check on G)
    // Left clique: all pairs within left are edges of G
    long long left_edges = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        for (size_t j = 0; j < g.adj[left[i]].size(); ++j) {
            int u = g.adj[left[i]][j];
            if (color[u] == 0) left_edges++;
        }
    }
    long long left_need = (long long)left.size() * ((long long)left.size() - 1);
    if (left_edges != left_need) return res; // Each edge counted twice

    long long right_edges = 0;
    for (size_t i = 0; i < right.size(); ++i) {
        for (size_t j = 0; j < g.adj[right[i]].size(); ++j) {
            int u = g.adj[right[i]][j];
            if (color[u] == 1) right_edges++;
        }
    }
    long long right_need = (long long)right.size() * ((long long)right.size() - 1);
    if (right_edges != right_need) return res;

    if (left.empty() || right.empty()) {
        res.color = color;
        res.x_ordering = left;
        res.y_ordering = right;
        res.is_cochain = true;
        return res;
    }

    // Chain property verification (on complement graph): suffix property via "non-adjacency count" of one side
    // Non-adjacency count of each L-side vertex to R side = |R| - (R-side adjacency count)
    int left_size = (int)left.size();
    int right_size = (int)right.size();

    // Assign ranks to L side (ascending non-adjacency count)
    std::vector<int> non_adj_r(n + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        int adj_r = 0;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            if (color[g.adj[v][j]] == 1) adj_r++;
        }
        non_adj_r[v] = right_size - adj_r;
    }

    // Sort L side in ascending non-adjacency count using counting sort
    int max_nonadj = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        if (non_adj_r[left[i]] > max_nonadj) max_nonadj = non_adj_r[left[i]];
    }
    std::vector<int> cnt(max_nonadj + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) cnt[non_adj_r[left[i]]]++;
    std::vector<int> sorted_left(left_size);
    std::vector<int> start_pos(max_nonadj + 1, 0);
    for (int k = 1; k <= max_nonadj; ++k) start_pos[k] = start_pos[k - 1] + cnt[k - 1];
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        sorted_left[start_pos[non_adj_r[v]]++] = v;
    }

    // rank[v] = position within sorted_left
    std::vector<int> rank_l(n + 1, -1);
    for (int i = 0; i < left_size; ++i) {
        rank_l[sorted_left[i]] = i;
    }

    // Verify minimum rank and count of L-side non-neighbors (= complement neighbors) for each R-side vertex
    for (size_t i = 0; i < right.size(); ++i) {
        int r = right[i];
        // Find L-side non-neighbors of R-side vertex r
        // Stamp r's L-side neighbors in G
        for (size_t j = 0; j < g.adj[r].size(); ++j) {
            int u = g.adj[r][j];
            if (color[u] == 0) stamped[u] = 1;
        }
        int min_rank = left_size;
        int count_nonadj = 0;
        for (int li = 0; li < left_size; ++li) {
            int u = left[li];
            if (!stamped[u]) {
                count_nonadj++;
                if (rank_l[u] < min_rank) min_rank = rank_l[u];
            }
        }
        // Clear stamps
        for (size_t j = 0; j < g.adj[r].size(); ++j) {
            int u = g.adj[r][j];
            if (color[u] == 0) stamped[u] = 0;
        }

        if (count_nonadj == 0) continue;
        // Suffix property: count_nonadj == left_size - min_rank
        if (count_nonadj != left_size - min_rank) return res;
    }

    if (!nested_side_order(g, left, right, true, res.x_ordering)) return res;
    if (!nested_side_order(g, right, left, true, res.y_ordering)) return res;
    res.color = color;
    res.is_cochain = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a cochain graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DIRECT)
 * @return CochainResult
 *
 * G is a cochain graph iff complement(G) is a chain graph.
 */
inline CochainResult check_cochain(const Graph& g,
    CochainAlgorithm algo = CochainAlgorithm::DIRECT) {
    switch (algo) {
        case CochainAlgorithm::COMPLEMENT:
            return detail::check_cochain_complement(g);
        case CochainAlgorithm::DIRECT: {
            CochainResult res = detail::check_cochain_direct(g);
            if (!res.is_cochain) {
                // DIRECT decides without materializing the complement, but the
                // witness is a structure of the complement, so it is read off
                // there. Same O(n^2) the variant already spends.
                res.obstruction = detail::check_cochain_complement(g).obstruction;
            }
            return res;
        }
        default:
            break;
    }
    return CochainResult();
}

/**
 * @brief Builds a NO certificate for a non-cochain graph
 * @param g Input graph
 * @return An ODD_CYCLE or TWO_K2 of the complement, with in_complement set, or
 *         an empty obstruction if g is a cochain graph
 */
inline Obstruction build_cochain_obstruction(const Graph& g) {
    return detail::check_cochain_complement(g).obstruction;
}

} // namespace graph_recognition

#endif
