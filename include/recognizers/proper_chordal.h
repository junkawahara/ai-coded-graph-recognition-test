#ifndef GRAPH_RECOGNITION_PROPER_CHORDAL_H
#define GRAPH_RECOGNITION_PROPER_CHORDAL_H

/**
 * @file proper_chordal.h
 * @brief Proper chordal graph recognition
 *
 * Proper chordal graphs are chordal graphs that admit an indifference tree-layout.
 * proper interval is a subset of proper chordal which is a subset of chordal (incomparable with interval).
 *
 * Algorithm (Paul & Protopapas, STACS 2024):
 *   1. Compute block tree with each vertex x as root (Algorithm 1)
 *   2. Verify nested-convex condition for each block (Algorithm 2)
 *   Proper chordal if successful for any root.
 *
 * Complexity: the algorithm of the paper is O(n^4), but the nested-convex
 * verification implemented here enumerates all vertex orderings of a block
 * (std::next_permutation), so the worst case is factorial in the block size,
 * not O(n^4).
 */

#include "recognizers/chordal.h"
#include "util/graph.h"
#include "decompositions/tree_layout.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Result of proper chordal recognition
 */
struct ProperChordalResult {
    bool is_proper_chordal = false;
    /**
     * @brief layout_parent[v] = parent of v in an indifference tree-layout, 0 at a root
     *
     * Sized n+1; valid only when is_proper_chordal == true. The layout is what
     * the recognition actually constructs; see tree_layout.h.
     */
    std::vector<int> layout_parent;
};


/**
 * @brief Determines whether the graph is proper chordal
 * @param g Input graph
 * @return ProperChordalResult
 *
 * proper chordal = chordal and admitting an indifference tree-layout.
 * proper interval is a subset of proper chordal which is a subset of chordal.
 * Hereditary class (closed under induced subgraphs).
 *
 * The nested-convex condition is verified by brute force over the vertex
 * orderings of each block, so the running time is exponential in the size of
 * the largest block even though the algorithm of Paul & Protopapas is O(n^4).
 */
inline ProperChordalResult check_proper_chordal(const Graph& g) {
    ProperChordalResult res;

    int n = g.n;
    if (n <= 2) {
        TreeLayoutResult trivial = find_indifference_tree_layout(g);
        if (!trivial.success) return res;
        res.layout_parent.swap(trivial.parent);
        res.is_proper_chordal = true;
        return res;
    }

    // Chordal check
    ChordalResult cr = check_chordal(g);
    if (!cr.is_chordal) return res;

    TreeLayoutResult layout = find_indifference_tree_layout(g);
    if (!layout.success) return res;
    res.layout_parent.swap(layout.parent);
    res.is_proper_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
