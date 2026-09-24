#ifndef GRAPH_RECOGNITION_BLOCK_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_BLOCK_UNLABELED_ENUM_H

/**
 * @file block_unlabeled_enum.h
 * @brief Non-isomorphic block graphs from weighted, colored block trees
 *
 * Implements the BlockGraphGen construction of Ekim, Shalom and Yirik,
 * "Generation of weighted trees, block trees and block graphs," 2024,
 * https://arxiv.org/abs/2401.09764, Sections 4--6.
 * A cut node has weight 1; a block node's weight counts its non-cut vertices.
 * The colors alternate, cut nodes have degree at least two, and leaf blocks
 * have positive weight. Sorted multisets of rooted branches remove sibling
 * symmetries. The three weighted-centroid cases generate each free tree once:
 * one centroid, two adjacent centroids of different colors, or a zero-weight
 * block between two equal-weight cut-rooted branches. Each block is expanded
 * to a clique; no graph recognition or general graph canonization is needed.
 * Disconnected graphs are multisets of connected representatives.
 *
 * This implementation caches rooted branches up to weight floor(n/2), plus
 * connected representatives when composing components. It materializes all
 * output edge lists. Clique reconstruction takes O(n + m) before sorting
 * edges, and O(n + m log(m+1)) including sorting. Cached forest traversal and
 * preprocessing are additional costs; the paper's linear output-delay bound
 * for tree representations is not claimed for this materializing API.
 */

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "util/vertex_count.h"

namespace graph_recognition {

/** @brief Algorithm for non-isomorphic block graph enumeration */
enum class BlockUnlabeledEnumAlgorithm {
    BLOCK_GRAPH_GEN /**< Weighted block trees with canonical centroid choices */
};

/** @brief One representative of a block graph isomorphism class */
struct BlockUnlabeledEnumeratedGraph {
    int n; /**< Number of vertices */
    std::vector<std::pair<int, int>> edges; /**< Sorted edges with 1 <= u < v <= n */
};

/** @brief Materialized non-isomorphic block graphs */
struct BlockUnlabeledEnumerationResult {
    std::vector<BlockUnlabeledEnumeratedGraph> graphs; /**< Enumerated representatives */
};

namespace detail {

// A branch root has an external parent of the opposite color. Consequently
// a cut root needs at least one block child, whereas a weight-zero block
// needs at least one cut child. Whole-tree roots impose the extra degree
// condition separately. Child IDs give a total order on rooted tree types.
struct BlockUnlabeledRootedTree {
    bool cut;
    int weight;
    int total_weight;
    std::vector<std::size_t> children;
};

class BlockUnlabeledGenerator {
    typedef std::vector<std::vector<std::size_t>> BranchLevels;
    std::vector<BlockUnlabeledRootedTree> trees_;
    BranchLevels blocks_, cuts_;

    // Enumerate multisets by increasing (total weight, representative index).
    // Every branch has positive total weight, even if its root has weight 0.
    template <typename Callback>
    void forests(int remaining, bool cut, int min_weight, std::size_t min_index,
                 int max_weight, std::vector<std::size_t>& chosen,
                 const Callback& callback) {
        if (remaining == 0) {
            callback(chosen);
            return;
        }
        const BranchLevels& levels = cut ? cuts_ : blocks_;
        const int limit = std::min(remaining, max_weight);
        for (int size = min_weight; size <= limit; ++size) {
            const std::vector<std::size_t>& choices = levels[size];
            const std::size_t first = size == min_weight ? min_index : 0;
            for (std::size_t i = first; i < choices.size(); ++i) {
                chosen.push_back(choices[i]);
                forests(remaining - size, cut, size, i, max_weight, chosen, callback);
                chosen.pop_back();
            }
        }
    }

    template <typename Callback>
    void forests(int weight, bool cut, int max_weight, const Callback& callback) {
        std::vector<std::size_t> chosen;
        forests(weight, cut, 1, 0, max_weight, chosen, callback);
    }

    void save_branch(bool cut, int weight, int total,
                     const std::vector<std::size_t>& children) {
        BlockUnlabeledRootedTree tree = {cut, weight, total, children};
        BranchLevels& levels = cut ? cuts_ : blocks_;
        levels[total].push_back(trees_.size());
        trees_.push_back(std::move(tree));
    }

    // Returns the graph label for a cut node (zero for a block node).
    int expand(const BlockUnlabeledRootedTree& tree, int parent_cut, int& next,
               std::vector<std::pair<int, int>>& edges) const {
        if (tree.cut) {
            const int label = next++;
            for (std::size_t i = 0; i < tree.children.size(); ++i)
                expand(trees_[tree.children[i]], label, next, edges);
            return label;
        }
        std::vector<int> clique;
        if (parent_cut != 0) clique.push_back(parent_cut);
        for (int i = 0; i < tree.weight; ++i) clique.push_back(next++);
        for (std::size_t i = 0; i < tree.children.size(); ++i)
            clique.push_back(expand(trees_[tree.children[i]], 0, next, edges));
        // Labels increase between children, but their descendants can create
        // gaps. Only these cut labels belong to the present clique.
        for (std::size_t i = 0; i < clique.size(); ++i)
            for (std::size_t j = i + 1; j < clique.size(); ++j)
                edges.push_back(std::make_pair(clique[i], clique[j]));
        return 0;
    }

    void emit(const BlockUnlabeledRootedTree& root,
              std::vector<BlockUnlabeledEnumeratedGraph>& out) const {
        BlockUnlabeledEnumeratedGraph graph;
        graph.n = root.total_weight;
        int next = 1;
        expand(root, 0, next, graph.edges);
        std::sort(graph.edges.begin(), graph.edges.end());
        out.push_back(std::move(graph));
    }

public:
    explicit BlockUnlabeledGenerator(int max_branch_weight)
        : blocks_(max_branch_weight + 1), cuts_(max_branch_weight + 1) {
        for (int total = 1; total <= max_branch_weight; ++total) {
            // Cut roots consume one unit, so they only use smaller blocks.
            if (total >= 2) {
                forests(total - 1, false, total - 1,
                    [&](const std::vector<std::size_t>& children) {
                        save_branch(true, 1, total, children);
                    });
            }
            // A zero-weight block can have a cut child of the same total
            // weight, so build this level AFTER its cut-rooted branches.
            for (int weight = 0; weight <= total; ++weight) {
                forests(total - weight, true, total,
                    [&](const std::vector<std::size_t>& children) {
                        save_branch(false, weight, total, children);
                    });
            }
        }
    }

    std::vector<BlockUnlabeledEnumeratedGraph> connected(int n) {
        std::vector<BlockUnlabeledEnumeratedGraph> out;
        const int strict_half = (n - 1) / 2;

        // One centroid: all branches have weight strictly below n/2.
        for (int weight = 0; weight <= n; ++weight) {
            forests(n - weight, true, strict_half,
                [&](const std::vector<std::size_t>& children) {
                    BlockUnlabeledRootedTree root = {false, weight, n, children};
                    emit(root, out);
                });
        }
        forests(n - 1, false, strict_half,
            [&](const std::vector<std::size_t>& children) {
                if (children.size() < 2) return; // a whole-tree cut has degree >= 2
                BlockUnlabeledRootedTree root = {true, 1, n, children};
                emit(root, out);
            });

        if (n % 2 != 0) return out;
        const int half = n / 2;

        // Two centroids: a block half joined to a cut half. Their colors
        // distinguish the halves, so no exchange symmetry remains. Exclude
        // a block half with a full-weight child (that is the next case).
        for (std::size_t b = 0; b < blocks_[half].size(); ++b) {
            BlockUnlabeledRootedTree root = trees_[blocks_[half][b]];
            if (!root.children.empty() &&
                trees_[root.children.back()].total_weight == half) continue;
            root.total_weight = n;
            for (std::size_t c = 0; c < cuts_[half].size(); ++c) {
                root.children.push_back(cuts_[half][c]);
                emit(root, out);
                root.children.pop_back();
            }
        }

        // Three centroids: the central block is a weight-zero K2; the two
        // cut-rooted halves form an unordered pair, with repetition allowed.
        for (std::size_t a = 0; a < cuts_[half].size(); ++a) {
            for (std::size_t b = a; b < cuts_[half].size(); ++b) {
                BlockUnlabeledRootedTree root =
                    {false, 0, n, {cuts_[half][a], cuts_[half][b]}};
                emit(root, out);
            }
        }
        return out;
    }
};

// Descending component sizes and nondecreasing indices for equal sizes
// produce exactly one graph for each multiset of connected components.
inline void block_unlabeled_combine(
    int n, int remaining, int max_size, std::size_t min_index,
    const std::vector<std::vector<BlockUnlabeledEnumeratedGraph>>& connected,
    std::vector<std::pair<int, int>>& edges,
    std::vector<BlockUnlabeledEnumeratedGraph>& out) {
    if (remaining == 0) {
        BlockUnlabeledEnumeratedGraph graph = {n, edges};
        out.push_back(std::move(graph));
        return;
    }
    const int offset = n - remaining;
    for (int size = std::min(remaining, max_size); size >= 1; --size) {
        const std::size_t first = size == max_size ? min_index : 0;
        for (std::size_t i = first; i < connected[size].size(); ++i) {
            const std::size_t old_size = edges.size();
            const std::vector<std::pair<int, int>>& component = connected[size][i].edges;
            for (std::size_t e = 0; e < component.size(); ++e)
                edges.push_back(std::make_pair(component[e].first + offset,
                                               component[e].second + offset));
            block_unlabeled_combine(n, remaining - size, size, i, connected, edges, out);
            edges.resize(old_size);
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates one representative of every n-vertex block graph
 * @param n Number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm selection (BLOCK_GRAPH_GEN)
 * @return All representatives on labels {1, ..., n}, with sorted edge lists
 *
 * n = 0 yields one empty graph, or no graphs when connected_only is true.
 * This vector-based construction has no adjacency-bitmask size restriction.
 * @throws std::invalid_argument if n < 0, n > max_vertex_count, or algo is unknown
 */
inline BlockUnlabeledEnumerationResult enumerate_block_unlabeled_graphs(
    int n, bool connected_only = false,
    BlockUnlabeledEnumAlgorithm algo = BlockUnlabeledEnumAlgorithm::BLOCK_GRAPH_GEN) {
    require_vertex_count("enumerate_block_unlabeled_graphs", n);
    if (algo != BlockUnlabeledEnumAlgorithm::BLOCK_GRAPH_GEN)
        throw std::invalid_argument("enumerate_block_unlabeled_graphs: unknown algorithm");
    BlockUnlabeledEnumerationResult result;
    if (n == 0) {
        if (!connected_only) result.graphs.push_back(BlockUnlabeledEnumeratedGraph{0, {}});
        return result;
    }
    detail::BlockUnlabeledGenerator generator(n / 2);
    if (connected_only) {
        result.graphs = generator.connected(n);
    } else {
        std::vector<std::vector<BlockUnlabeledEnumeratedGraph>> connected(n + 1);
        for (int size = 1; size <= n; ++size) connected[size] = generator.connected(size);
        std::vector<std::pair<int, int>> edges;
        detail::block_unlabeled_combine(n, n, n, 0, connected, edges, result.graphs);
    }
    return result;
}

}  // namespace graph_recognition

#endif
