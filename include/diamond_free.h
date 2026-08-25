#ifndef GRAPH_RECOGNITION_DIAMOND_FREE_H
#define GRAPH_RECOGNITION_DIAMOND_FREE_H

/**
 * @file diamond_free.h
 * @brief Diamond-free graph (K4-e free graph) recognition
 *
 * A diamond-free graph is a graph that does not contain a diamond (K4 minus
 * one edge, also denoted K4-e) as an induced subgraph.
 *
 * A diamond is a graph on 4 vertices {a, b, c, d} with edges ab, ac, ad,
 * bc, bd but without edge cd. In other words, a diamond appears when two
 * triangles sharing an edge do not form K4.
 *
 * Equivalent characterizations of diamond-free graphs:
 *   - Any two triangles sharing an edge form K4
 *   - For any edge (u,v), the set of common neighbors of u and v forms a clique
 *
 * Algorithms:
 *   - BRUTE: Enumerate all 4-vertex subsets and search for a diamond O(n^4)
 *   - EDGE_PAIR: Check whether common neighbors of each edge form a clique,
 *     O(m) per edge, so O(m^2) total (default)
 *
 * References:
 *   - Folklore; detection of small forbidden subgraphs
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "obstruction_extract.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for diamond-free graph recognition
 */
enum class DiamondFreeAlgorithm {
    BRUTE,     /**< Enumerate all 4-vertex subsets O(n^4) */
    EDGE_PAIR  /**< Check common neighbors per edge, O(m^2) total (default) */
};

/**
 * @brief Result of diamond-free graph recognition
 */
struct DiamondFreeResult {
    bool is_diamond_free = false; /**< true if the graph is diamond-free */
    Obstruction obstruction; /**< NO certificate: a DIAMOND. Valid only when
                                  is_diamond_free == false; filled by every variant */
};

namespace detail {

/**
 * @brief Detect an induced diamond by enumerating all 4-vertex subsets
 *
 * Checks whether the edge count is exactly 5 for every combination of 4 vertices.
 * Complexity: O(n^4)
 */
inline DiamondFreeResult check_diamond_free_brute(const Graph& g) {
    DiamondFreeResult res;
    res.is_diamond_free = true;

    for (int a = 1; a <= g.n; ++a) {
        for (int b = a + 1; b <= g.n; ++b) {
            for (int c = b + 1; c <= g.n; ++c) {
                for (int d = c + 1; d <= g.n; ++d) {
                    // Count edges among the 4 vertices
                    int cnt = 0;
                    if (g.has_edge(a, b)) cnt++;
                    if (g.has_edge(a, c)) cnt++;
                    if (g.has_edge(a, d)) cnt++;
                    if (g.has_edge(b, c)) cnt++;
                    if (g.has_edge(b, d)) cnt++;
                    if (g.has_edge(c, d)) cnt++;
                    if (cnt == 5) {
                        res.is_diamond_free = false;
                        // The subset test knows the count, not the roles; the
                        // shared finder names the degree-3 pair for O(m*Delta),
                        // well inside this variant's own O(n^4).
                        res.obstruction = make_obstruction(
                            ObstructionKind::DIAMOND, detail_obstruction::find_diamond(g));
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief Detect a diamond by checking common neighbors per edge, O(m^2) total
 *
 * For each edge (u,v), compute the set S of common neighbors of u and v.
 * If there is a non-adjacent pair in S, they form a diamond together with u and v.
 * If S is a clique, no diamond arises from this edge.
 *
 * For each edge, the number of common neighbors is O(min(deg(u), deg(v))),
 * and the clique check via edge counting is O(sum of degrees) = O(m).
 * Total over all edges: O(m^2).
 */
inline DiamondFreeResult check_diamond_free_edge_pair(const Graph& g) {
    DiamondFreeResult res;
    res.is_diamond_free = true;

    int n = g.n;
    std::vector<unsigned char> in_common(n + 1, 0);

    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (v <= u) continue; // Process each edge only once

            // Compute the set of common neighbors
            std::vector<int> common;
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (w != v && g.has_edge(w, v)) {
                    common.push_back(w);
                    in_common[w] = 1;
                }
            }

            if (common.size() < 2) {
                // If there are at most 1 common neighbors, no diamond is possible
                for (size_t i = 0; i < common.size(); ++i) {
                    in_common[common[i]] = 0;
                }
                continue;
            }

            // Check whether the common neighbors form a clique by counting edges
            long long edge_count = 0;
            for (size_t i = 0; i < common.size(); ++i) {
                int w = common[i];
                for (size_t j = 0; j < g.adj[w].size(); ++j) {
                    if (in_common[g.adj[w][j]] && g.adj[w][j] > w) {
                        edge_count++;
                    }
                }
            }

            long long s = (long long)common.size();
            long long need = s * (s - 1) / 2;

            // Cleanup
            for (size_t i = 0; i < common.size(); ++i) {
                in_common[common[i]] = 0;
            }

            if (edge_count < need) {
                res.is_diamond_free = false;
                // The count says the common neighbourhood is not a clique;
                // recover the non-adjacent pair it hides.
                for (size_t i = 0; i < common.size() && !res.obstruction.has_witness(); ++i) {
                    for (size_t j = i + 1; j < common.size(); ++j) {
                        if (g.has_edge(common[i], common[j])) continue;
                        std::vector<int> vs;
                        vs.push_back(u);
                        vs.push_back(v);
                        vs.push_back(common[i]);
                        vs.push_back(common[j]);
                        res.obstruction = make_obstruction(ObstructionKind::DIAMOND, vs);
                        break;
                    }
                }
                return res;
            }
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is diamond-free (K4-e free)
 * @param g Input graph
 * @param algo Algorithm to use (default: EDGE_PAIR)
 * @return DiamondFreeResult
 *
 * G is diamond-free iff it does not contain diamond (K4-e) as an induced subgraph.
 * Equivalent condition: any two triangles sharing an edge form K4.
 */
inline DiamondFreeResult check_diamond_free(const Graph& g,
    DiamondFreeAlgorithm algo = DiamondFreeAlgorithm::EDGE_PAIR) {
    switch (algo) {
        case DiamondFreeAlgorithm::BRUTE:
            return detail::check_diamond_free_brute(g);
        case DiamondFreeAlgorithm::EDGE_PAIR:
            return detail::check_diamond_free_edge_pair(g);
        default:
            break;
    }
    return DiamondFreeResult();
}

} // namespace graph_recognition

#endif
