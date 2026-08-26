#ifndef GRAPH_RECOGNITION_TREE_DECOMPOSITION_H
#define GRAPH_RECOGNITION_TREE_DECOMPOSITION_H

/**
 * @file tree_decomposition.h
 * @brief Tree decompositions of chordal graphs
 *
 * A tree decomposition assigns each node of a tree a bag of vertices so that
 * every vertex lies in some bag, every edge lies inside some bag, and the bags
 * holding any one vertex form a connected subtree. Its width is the largest
 * bag size minus one, and the treewidth of a graph is the smallest width over
 * all of its tree decompositions.
 *
 * For a chordal graph the clique tree already is an optimal tree
 * decomposition: its bags are the maximal cliques, the running intersection
 * property is exactly the subtree condition, and no decomposition can have a
 * bag smaller than the largest clique. So this header is a thin reading of
 * clique.h rather than a search -- which is also why it only handles chordal
 * graphs: computing treewidth in general is NP-hard.
 *
 * References:
 *   - Gavril (1974), Buneman (1974): chordal graphs are the intersection
 *     graphs of subtrees of a tree.
 *   - Robertson & Seymour (1986): tree decompositions and treewidth.
 */

#include "recognizers/chordal.h"
#include "decompositions/clique.h"
#include "util/graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief A tree decomposition
 */
struct TreeDecompositionResult {
    bool success = false;                 /**< true if a decomposition was built */
    std::vector<std::vector<int>> bags;   /**< bags[i] = vertex set of node i */
    std::vector<std::vector<int>> tree;   /**< adjacency of the decomposition tree */
    int width = -1;                       /**< largest bag size minus one; valid only when success */
};

/**
 * @brief Reads a tree decomposition off a clique tree
 * @param g The chordal graph
 * @param ct Clique tree of g, from build_clique_tree()
 * @return TreeDecompositionResult
 *
 * The bags are the maximal cliques. For a chordal graph this decomposition has
 * width exactly one less than the largest clique, which is optimal, so the
 * width reported is the treewidth.
 *
 * A clique tree built from a violated precondition has no cliques; the empty
 * graph legitimately has none either, so the two are told apart by g.n.
 */
inline TreeDecompositionResult tree_decomposition_from_clique_tree(const Graph& g,
                                                                  const CliqueTreeResult& ct) {
    TreeDecompositionResult res;
    if (ct.mc.cliques.empty()) {
        if (g.n != 0) return res;
        res.width = -1;
        res.success = true;
        return res;
    }

    res.bags = ct.mc.cliques;
    res.tree = ct.tree;
    res.tree.resize(res.bags.size());

    // A disconnected graph gives a clique forest, one tree per component.
    // Linking the pieces keeps every property: bags from different components
    // share no vertex, so no vertex's subtree is touched.
    {
        size_t k = res.tree.size();
        std::vector<int> seen(k, 0);
        int previous_root = -1;
        for (size_t s = 0; s < k; ++s) {
            if (seen[s]) continue;
            if (previous_root >= 0) {
                res.tree[previous_root].push_back((int)s);
                res.tree[s].push_back(previous_root);
            }
            previous_root = (int)s;
            std::vector<int> stack(1, (int)s);
            seen[s] = 1;
            while (!stack.empty()) {
                int x = stack.back();
                stack.pop_back();
                for (size_t j = 0; j < res.tree[x].size(); ++j) {
                    int y = res.tree[x][j];
                    if (seen[y]) continue;
                    seen[y] = 1;
                    stack.push_back(y);
                }
            }
        }
    }

    int width = 0;
    for (size_t i = 0; i < res.bags.size(); ++i) {
        int size = (int)res.bags[i].size();
        if (size > width) width = size;
    }
    res.width = width - 1;
    res.success = true;
    return res;
}

/**
 * @brief Builds a tree decomposition of a chordal graph
 * @param g Input graph
 * @return TreeDecompositionResult; success is false when g is not chordal
 *
 * The width reported is the treewidth of g.
 */
inline TreeDecompositionResult tree_decomposition_chordal(const Graph& g) {
    TreeDecompositionResult res;
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;
    return tree_decomposition_from_clique_tree(g, build_clique_tree(g, chordal));
}

} // namespace graph_recognition

#endif
