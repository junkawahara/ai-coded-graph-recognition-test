#ifndef GRAPH_RECOGNITION_CHAIN_H
#define GRAPH_RECOGNITION_CHAIN_H

/**
 * @file chain.h
 * @brief Chain graph recognition
 *
 * Algorithms:
 *   - NEIGHBORHOOD_INCLUSION: pairwise neighborhood inclusion check
 *   - DEGREE_SORT: degree sort + suffix property verification (default)
 *
 * Both variants also report the nested orders themselves: the vertices of each
 * side listed so that their neighbourhoods grow by inclusion. That is the
 * structure the definition asks for, and sorting a side by degree produces it
 * whenever the graph really is a chain graph.
 */

#include "bipartite.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for chain graph recognition
 */
enum class ChainAlgorithm {
    NEIGHBORHOOD_INCLUSION, /**< pairwise neighborhood inclusion check */
    DEGREE_SORT             /**< degree sort + suffix property verification (default) */
};

/**
 * @brief Result of chain graph recognition
 */
struct ChainResult {
    bool is_chain = false; /**< true if the graph is a chain graph */
    std::vector<int> color; /**< color[v] in {0, 1}: the bipartition (size n+1) */
    /**
     * @brief Vertices of color 0, ordered so that N(v) grows by inclusion
     *
     * Valid only when is_chain == true.
     */
    std::vector<int> x_ordering;
    /**
     * @brief Vertices of color 1, ordered so that N(v) grows by inclusion
     *
     * Valid only when is_chain == true.
     */
    std::vector<int> y_ordering;
};

namespace detail {

/**
 * @brief Orders one side so that the neighbourhoods grow by inclusion
 * @param g Input graph
 * @param side Vertices to order
 * @param other_side The side the neighbourhoods are taken in
 * @param complemented Order by non-neighbourhood instead (used by cochain.h,
 *        so it never has to build the complement)
 * @param out Receives the ordering
 * @return true if the neighbourhoods really are nested
 *
 * Sorting by degree is what produces the order: if the neighbourhoods are
 * totally ordered by inclusion then the smaller one belongs to the vertex of
 * smaller degree. Only consecutive pairs need checking, since inclusion is
 * transitive. Complementing reverses the order, so the check is the same one
 * read backwards.
 */
inline bool nested_side_order(const Graph& g, const std::vector<int>& side,
                              const std::vector<int>& other_side, bool complemented,
                              std::vector<int>& out) {
    out.clear();
    std::vector<char> in_other(g.n + 1, 0);
    for (size_t i = 0; i < other_side.size(); ++i) in_other[other_side[i]] = 1;

    std::vector<int> deg(g.n + 1, 0);
    for (size_t i = 0; i < side.size(); ++i) {
        int v = side[i];
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            if (in_other[g.adj[v][j]]) deg[v]++;
        }
    }

    out = side;
    if (complemented) {
        // Non-neighbourhood ascending is neighbourhood descending.
        std::stable_sort(out.begin(), out.end(),
                         [&deg](int a, int b) { return deg[a] > deg[b]; });
    } else {
        std::stable_sort(out.begin(), out.end(),
                         [&deg](int a, int b) { return deg[a] < deg[b]; });
    }

    for (size_t i = 0; i + 1 < out.size(); ++i) {
        // The smaller neighbourhood must sit inside the larger one.
        int small = complemented ? out[i + 1] : out[i];
        int large = complemented ? out[i] : out[i + 1];
        for (size_t j = 0; j < g.adj[small].size(); ++j) {
            int w = g.adj[small][j];
            if (!in_other[w]) continue;
            if (!g.has_edge(large, w)) {
                out.clear();
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Determines whether one side's neighborhoods form a linear order by inclusion (internal function)
 */
inline bool is_nested_neighborhood_side(
    const Graph& g,
    const std::vector<int>& side,
    const std::vector<int>& other_side) {
    for (size_t i = 0; i < side.size(); ++i) {
        int u = side[i];
        for (size_t j = i + 1; j < side.size(); ++j) {
            int v = side[j];
            bool u_subset_v = true;
            bool v_subset_u = true;
            for (size_t k = 0; k < other_side.size(); ++k) {
                int w = other_side[k];
                bool uw = g.has_edge(u, w);
                bool vw = g.has_edge(v, w);
                if (uw && !vw) u_subset_v = false;
                if (vw && !uw) v_subset_u = false;
                if (!u_subset_v && !v_subset_u) return false;
            }
        }
    }
    return true;
}

/** @brief Chain graph recognition by pairwise inclusion check (original algorithm) */
inline ChainResult check_chain_inclusion(const Graph& g) {
    ChainResult res;
    res.is_chain = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    std::vector<int> left, right;
    left.reserve(g.n);
    right.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    if (!is_nested_neighborhood_side(g, left, right)) return res;

    if (!nested_side_order(g, left, right, false, res.x_ordering)) return res;
    if (!nested_side_order(g, right, left, false, res.y_ordering)) return res;
    res.color = bip.color;
    res.is_chain = true;
    return res;
}

/**
 * @brief Chain graph recognition via degree sort + suffix property verification
 *
 * Sorts the L side in ascending order by degree and verifies in O(n+m)
 * that the L-side neighbors of each R-side vertex form a suffix of the sorted L.
 */
inline ChainResult check_chain_degree_sort(const Graph& g) {
    ChainResult res;
    res.is_chain = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;
    std::vector<int> left, right;
    left.reserve(n);
    right.reserve(n);
    for (int v = 1; v <= n; ++v) {
        if (bip.color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    if (left.empty() || right.empty()) {
        res.color = bip.color;
        res.x_ordering = left;
        res.y_ordering = right;
        res.is_chain = true;
        return res;
    }

    // Compute the R-side degree of each L-side vertex
    int left_size = (int)left.size();
    std::vector<int> deg_r(n + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (bip.color[u] == 1) deg_r[v]++;
        }
    }

    // Sort L side in ascending degree order using counting sort
    int max_deg = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        if (deg_r[left[i]] > max_deg) max_deg = deg_r[left[i]];
    }
    std::vector<int> cnt(max_deg + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) cnt[deg_r[left[i]]]++;

    std::vector<int> sorted_left(left_size);
    // Determine positions via prefix sums (ascending)
    std::vector<int> start(max_deg + 1, 0);
    for (int k = 1; k <= max_deg; ++k) start[k] = start[k - 1] + cnt[k - 1];
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        sorted_left[start[deg_r[v]]++] = v;
    }

    // rank[v] = position within sorted_left
    std::vector<int> rank_l(n + 1, -1);
    for (int i = 0; i < left_size; ++i) {
        rank_l[sorted_left[i]] = i;
    }

    // For each R-side vertex: verify minimum rank and count of L-side neighbors
    for (size_t i = 0; i < right.size(); ++i) {
        int r = right[i];
        int min_rank = left_size;
        int count_l = 0;
        for (size_t j = 0; j < g.adj[r].size(); ++j) {
            int u = g.adj[r][j];
            if (bip.color[u] == 0) {
                count_l++;
                if (rank_l[u] < min_rank) min_rank = rank_l[u];
            }
        }
        if (count_l == 0) continue; // No neighbors -> OK
        // Suffix property: count_l == left_size - min_rank
        if (count_l != left_size - min_rank) return res;
    }

    if (!nested_side_order(g, left, right, false, res.x_ordering)) return res;
    if (!nested_side_order(g, right, left, false, res.y_ordering)) return res;
    res.color = bip.color;
    res.is_chain = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a chain graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DEGREE_SORT)
 * @return ChainResult
 */
inline ChainResult check_chain(const Graph& g,
    ChainAlgorithm algo = ChainAlgorithm::DEGREE_SORT) {
    switch (algo) {
        case ChainAlgorithm::NEIGHBORHOOD_INCLUSION:
            return detail::check_chain_inclusion(g);
        case ChainAlgorithm::DEGREE_SORT:
            return detail::check_chain_degree_sort(g);
        default:
            break;
    }
    return ChainResult();
}

} // namespace graph_recognition

#endif
