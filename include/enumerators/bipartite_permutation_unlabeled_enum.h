#ifndef GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_UNLABELED_ENUM_H

/**
 * @file bipartite_permutation_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic bipartite permutation graphs
 *
 * STRING_REPRESENTATION uses the line representations and endpoint-swap
 * reverse search of Saitoh, Otachi, Yamanaka and Uehara (Section 4).
 * For each bipartition size p >= q, it starts at K_{p,q}, traverses connected
 * representations, and emits only the least string under the applicable
 * flips. Component multisets give all disconnected graphs. No recognizer or
 * general graph canonicalization is needed in this mode.
 *
 * String comparisons and nominated-point discovery scan O(n) characters;
 * constructing an edge list takes O(n^2). The connected generator takes
 * O(n^2) amortized time per graph and O(n^2) search space, excluding outputs.
 * It does not implement the paper's constant-time updates, prepostorder or
 * jumps between bipartition sizes, and does not claim its O(1) delay bound.
 * All outputs and, for disconnected enumeration, connected representatives
 * are retained in memory. CANONICAL_AUGMENTATION retains the original
 * recognizer-pruned vertex additions and McKay canonical-parent test.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Saitoh, Otachi, Yamanaka, Uehara, "Random generation and enumeration
 *   of bipartite permutation graphs," J. Discrete Algorithms 10, 2012
 *   DOI: 10.1016/j.jda.2011.11.001
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "recognizers/bipartite_permutation.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic bipartite permutation enumeration
 */
enum class BipartitePermutationUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION = 0, /**< Previous recognizer-pruned canonical augmentation */
    STRING_REPRESENTATION = 1 /**< Endpoint-swap reverse search and canonical strings (default) */
};

/**
 * @brief An enumerated bipartite permutation graph
 */
struct BipartitePermutationUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic bipartite permutation enumeration
 */
struct BipartitePermutationUnlabeledEnumerationResult {
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Reverse a line string and exchange the two bracket types. */
inline std::string bipartite_permutation_unlabeled_reverse_flip(const std::string& s) {
    std::string result(s.rbegin(), s.rend());
    for (std::size_t i = 0; i < result.size(); ++i)
        result[i] = result[i] == '[' ? ']' : '[';
    return result;
}

/**
 * @brief Select one orientation of a connected line representation.
 *
 * top encodes X by '[' and Y by ']'; bottom uses the opposite convention.
 * Equal-colored endpoints are matched in order. Reversal of both lines,
 * exchange of the lines, and their composition are the only ambiguities
 * (Saitoh et al., Lemma 3). Fixing |X| = p >= q = |Y| leaves two orientations
 * when p > q and four when p == q. Equality must be accepted for symmetries.
 */
inline bool bipartite_permutation_unlabeled_string_is_canonical(
    const std::string& top, const std::string& bottom, int p, int q) {
    const std::string s = top + bottom;
    const std::string reverse_top = bipartite_permutation_unlabeled_reverse_flip(top);
    const std::string reverse_bottom = bipartite_permutation_unlabeled_reverse_flip(bottom);
    if (s > reverse_bottom + reverse_top) return false;
    return p != q || (s <= bottom + top && s <= reverse_top + reverse_bottom);
}

/**
 * @brief Check strict positive height of the associated 2-Motzkin path.
 *
 * At every proper nonempty prefix, (# '[' in top) + (# '[' in bottom)
 * must exceed the prefix length. This ensures both the chosen orientation
 * of every segment and connectivity. The complete height is zero because
 * the strings contain p and q opening brackets and both have length p+q.
 */
inline bool bipartite_permutation_unlabeled_string_connected(
    const std::string& top, const std::string& bottom) {
    int height = 0;
    for (std::size_t i = 0; i + 1 < top.size(); ++i) {
        height += (top[i] == '[' ? 1 : 0) + (bottom[i] == '[' ? 1 : 0) - 1;
        if (height <= 0) return false;
    }
    return true;
}

/** @brief Match endpoints within each color and emit the permutation inversions. */
inline BipartitePermutationUnlabeledEnumeratedGraph
bipartite_permutation_unlabeled_build_string_graph(
    const std::string& top, const std::string& bottom) {
    BipartitePermutationUnlabeledEnumeratedGraph graph;
    graph.n = static_cast<int>(top.size());
    std::vector<int> x, y;
    for (int i = 0; i < graph.n; ++i)
        (top[i] == '[' ? x : y).push_back(i);
    std::vector<int> position(graph.n);
    std::size_t next_x = 0, next_y = 0;
    for (int i = 0; i < graph.n; ++i) {
        const int v = bottom[i] == '[' ? y[next_y++] : x[next_x++];
        position[v] = i;
    }
    for (int u = 0; u < graph.n; ++u)
        for (int v = u + 1; v < graph.n; ++v)
            if (position[u] > position[v]) graph.edges.push_back(std::make_pair(u + 1, v + 1));
    return graph;
}

/**
 * @brief Reverse search on connected representations with fixed color sizes.
 *
 * The parent swaps the first ']'-run's following '[' left on the top line.
 * If top is '['^p ']'^q, it swaps the last '['-run's preceding ']' right
 * on the bottom line. Each parent adds an edge, hence stays connected, and
 * decreases the string lexicographically until K_{p,q}. Its inverse has at
 * most three nominated swaps (Section 4, Lemma 16).
 *
 * Canonicity filters emission only: a noncanonical orientation can have a
 * canonical descendant, especially when p == q. Traversing it is necessary
 * for completeness and costs at most four representations per output class.
 * The strings are modified in place; every child deletes one edge, so the
 * recursion depth is at most pq-(p+q-1), within the public vertex limits.
 */
inline void bipartite_permutation_unlabeled_string_dfs(
    std::string& top, std::string& bottom, int p, int q,
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph>& out) {
    if (bipartite_permutation_unlabeled_string_is_canonical(top, bottom, p, q))
        out.push_back(bipartite_permutation_unlabeled_build_string_graph(top, bottom));

    // Each pair is (line: 0=top, 1=bottom, left endpoint of the swap).
    std::pair<int, std::size_t> candidates[3];
    std::size_t count = 0;
    const std::size_t first_close = top.find(']');
    candidates[count++] = std::make_pair(0, first_close - 1);
    const std::size_t next_open = top.find('[', first_close);
    if (next_open != std::string::npos) {
        if (next_open + 1 < top.size() && top[next_open + 1] == ']')
            candidates[count++] = std::make_pair(0, next_open);
    } else {
        const std::size_t last_open = bottom.rfind('[');
        candidates[count++] = std::make_pair(1, last_open);
        const std::size_t previous_close = bottom.rfind(']', last_open);
        if (previous_close != std::string::npos && previous_close > 0 &&
            bottom[previous_close - 1] == '[')
            candidates[count++] = std::make_pair(1, previous_close - 1);
    }

    for (std::size_t i = 0; i < count; ++i) {
        std::string& line = candidates[i].first == 0 ? top : bottom;
        const std::size_t pos = candidates[i].second;
        std::swap(line[pos], line[pos + 1]);
        if (bipartite_permutation_unlabeled_string_connected(top, bottom))
            bipartite_permutation_unlabeled_string_dfs(top, bottom, p, q, out);
        std::swap(line[pos], line[pos + 1]);
    }
}

/** @brief Generate connected representatives, with K1 handled separately. */
inline std::vector<BipartitePermutationUnlabeledEnumeratedGraph>
bipartite_permutation_unlabeled_connected(int n) {
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph> out;
    if (n == 1) {
        out.push_back(BipartitePermutationUnlabeledEnumeratedGraph{1, {}});
        return out;
    }
    for (int p = (n + 1) / 2; p < n; ++p) {
        const int q = n - p;
        std::string top = std::string(p, '[') + std::string(q, ']');
        std::string bottom = std::string(q, '[') + std::string(p, ']');
        bipartite_permutation_unlabeled_string_dfs(top, bottom, p, q, out);
    }
    return out;
}

/**
 * @brief Compose component multisets over nonincreasing integer partitions.
 *
 * Equal-size components use nondecreasing indices, including repetitions;
 * thus permutations of isomorphic components are emitted only once.
 */
inline void bipartite_permutation_unlabeled_combine(
    int n, int remaining, int max_size, std::size_t min_index,
    std::vector<std::pair<int, int>>& edges,
    const std::vector<std::vector<BipartitePermutationUnlabeledEnumeratedGraph>>& connected,
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph>& out) {
    if (remaining == 0) {
        out.push_back(BipartitePermutationUnlabeledEnumeratedGraph{n, edges});
        return;
    }
    for (int size = std::min(remaining, max_size); size >= 1; --size) {
        const std::size_t start = size == max_size ? min_index : 0;
        for (std::size_t i = start; i < connected[size].size(); ++i) {
            const std::size_t old_size = edges.size();
            const int offset = n - remaining;
            for (const auto& edge : connected[size][i].edges)
                edges.push_back(std::make_pair(edge.first + offset, edge.second + offset));
            bipartite_permutation_unlabeled_combine(
                n, remaining - size, size, i, edges, connected, out);
            edges.resize(old_size);
        }
    }
}

/**
 * @brief Shared structural backend; callers validate n before entry.
 *
 * Kept separate from the public API so labeled enumeration retains its
 * existing 64-vertex limit, while unlabeled enumeration retains 63.
 */
inline BipartitePermutationUnlabeledEnumerationResult
bipartite_permutation_unlabeled_string_graphs(int n, bool connected_only) {
    BipartitePermutationUnlabeledEnumerationResult result;
    if (n == 0) {
        result.graphs.push_back(BipartitePermutationUnlabeledEnumeratedGraph{0, {}});
    } else if (connected_only) {
        result.graphs = bipartite_permutation_unlabeled_connected(n);
    } else {
        std::vector<std::vector<BipartitePermutationUnlabeledEnumeratedGraph>> connected(n + 1);
        for (int size = 1; size <= n; ++size)
            connected[size] = bipartite_permutation_unlabeled_connected(size);
        std::vector<std::pair<int, int>> edges;
        bipartite_permutation_unlabeled_combine(n, n, n, 0, edges, connected, result.graphs);
    }
    return result;
}

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline BipartitePermutationUnlabeledEnumeratedGraph
bipartite_permutation_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    BipartitePermutationUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
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
 * Extends by a new vertex whose neighborhood S runs over all subsets of the
 * current vertex set, keeping those for which the extended graph is still
 * a bipartite permutation graph. A child survives when the new vertex lies
 * in its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument. Connectivity cannot be
 * pruned during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void bipartite_permutation_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(
                bipartite_permutation_unlabeled_build_graph(k, adj));
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        Graph child(k + 1, bitmask_graph_edges(k + 1, adj));
        if (check_bipartite_permutation(child).is_bipartite_permutation) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                bipartite_permutation_unlabeled_enum_dfs(
                    k + 1, adj, n, connected_only, results);
            }
        }

        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic bipartite permutation graphs on n vertices
 * @param n Number of vertices (0 through 63)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: STRING_REPRESENTATION)
 * @return BipartitePermutationUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class of bipartite permutation
 * graphs, or the connected ones among them with @p connected_only.
 * n = 0 yields the single empty graph in both modes; negative n and
 * n >= 64 throw std::invalid_argument.
 *
 * @note Both modes materialize all results. The structural mode uses line
 *       strings and component multisets. The legacy mode retains recognition,
 *       general graph canonicalization, and its original output order.
 *       Representative labelings and output order can differ between modes.
 * @throws std::invalid_argument if n < 0 or n > 63
 *         (the existing API limit), or algo is unknown
 */
inline BipartitePermutationUnlabeledEnumerationResult
enumerate_bipartite_permutation_unlabeled_graphs(
    int n, bool connected_only = false,
    BipartitePermutationUnlabeledEnumAlgorithm algo =
        BipartitePermutationUnlabeledEnumAlgorithm::STRING_REPRESENTATION) {
    BipartitePermutationUnlabeledEnumerationResult result;
    require_vertex_count("enumerate_bipartite_permutation_unlabeled_graphs", n, 63);
    if (algo == BipartitePermutationUnlabeledEnumAlgorithm::STRING_REPRESENTATION)
        return detail::bipartite_permutation_unlabeled_string_graphs(n, connected_only);
    if (algo != BipartitePermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION)
        throw std::invalid_argument("enumerate_bipartite_permutation_unlabeled_graphs: unknown algorithm");
    if (n == 0) {
        BipartitePermutationUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::bipartite_permutation_unlabeled_enum_dfs(1, adj, n, connected_only,
                                                     result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
