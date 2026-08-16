#ifndef GRAPH_RECOGNITION_LEXBFS_H
#define GRAPH_RECOGNITION_LEXBFS_H

/**
 * @file lexbfs.h
 * @brief Lexicographic Breadth-First Search (LexBFS)
 *
 * Algorithm of Rose, Tarjan, Lueker (1976).
 * Produces a perfect elimination ordering (PEO) for chordal graphs.
 *
 * Like MCS, returns a correct PEO only for chordal graphs.
 * For non-chordal graphs, returns an ordering that fails PEO verification.
 *
 * Algorithms:
 *   - SIMPLE_LEXBFS: simple implementation via label list comparison O(n^2 + nm)
 *   - PARTITION_LEXBFS: fast implementation via partition refinement O(n+m) (default)
 */

#include "graph.h"
#include "mcs.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for LexBFS
 */
enum class LexBFSAlgorithm {
    SIMPLE_LEXBFS,    /**< Label list comparison O(n^2 + nm) */
    PARTITION_LEXBFS  /**< Partition refinement O(n+m) (default) */
};

namespace detail {

/**
 * @brief Simple LexBFS implementation
 *
 * Each vertex has a label (integer list), and the unlabeled vertex with the
 * lexicographically largest label is selected. Label comparison costs O(n), giving O(n^2 + nm) overall.
 *
 * @param g Input graph
 * @return MCSResult (order and number)
 */
inline MCSResult lexbfs_simple(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    // label[v]: label for vertex v (for lexicographic comparison)
    std::vector<std::vector<int> > label(n + 1);
    std::vector<unsigned char> used(n + 1, 0);

    for (int i = n; i >= 1; --i) {
        // Select the unlabeled vertex with the lexicographically largest label
        int best = -1;
        for (int v = 1; v <= n; ++v) {
            if (used[v]) continue;
            if (best == -1 || label[v] > label[best]) {
                best = v;
            }
        }

        used[best] = 1;
        res.order[i] = best;
        res.number[best] = i;

        // Append i to the labels of unlabeled adjacent vertices
        for (size_t j = 0; j < g.adj[best].size(); ++j) {
            int u = g.adj[best][j];
            if (!used[u]) {
                label[u].push_back(i);
            }
        }
    }

    return res;
}

/**
 * @brief LexBFS via partition refinement O(n+m)
 *
 * Based on the method of Habib, McConnell, Paul, Viennot (2000).
 * Manages an ordered list of vertex classes, and at each step separates
 * adjacent vertices to the front of their class.
 *
 * @param g Input graph
 * @return MCSResult (order and number)
 */
inline MCSResult lexbfs_partition(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    // Doubly-linked list of classes
    // sentinel = 0, initial class = 1
    // The IDs of emptied classes are recycled through free_class_ids, so the
    // number of live IDs -- and thus the size of these arrays, which grow on
    // demand -- stays O(n) instead of the O(n + m) a fresh-ID-per-split
    // scheme would need. Class IDs are only used as indices (never compared),
    // so recycling does not affect the traversal order.
    std::vector<int> class_next(n + 2, 0);
    std::vector<int> class_prev(n + 2, 0);
    std::vector<int> class_head(n + 2, 0);
    std::vector<int> free_class_ids;

    // Doubly-linked list of vertices within a class
    std::vector<int> vertex_next(n + 1, 0);
    std::vector<int> vertex_prev(n + 1, 0);
    std::vector<int> vertex_class(n + 1, 0);
    std::vector<unsigned char> used(n + 1, 0);

    int next_class_id = 2;

    // Initialize: place all vertices in class 1
    class_next[0] = 1;
    class_prev[1] = 0;
    class_next[1] = 0;
    class_prev[0] = 1;

    class_head[1] = 1;
    for (int v = 1; v <= n; ++v) {
        vertex_class[v] = 1;
        vertex_next[v] = (v < n) ? v + 1 : 0;
        vertex_prev[v] = (v > 1) ? v - 1 : 0;
    }

    // new_class[c]: ID of the new class split from class c in this step (0 = not split)
    std::vector<int> new_class(n + 2, 0);
    std::vector<int> touched_classes;

    for (int i = n; i >= 1; --i) {
        // Extract a vertex from the first non-empty class
        int first_class = class_next[0];
        while (first_class != 0 && class_head[first_class] == 0) {
            int nc = class_next[first_class];
            class_next[0] = nc;
            if (nc != 0) class_prev[nc] = 0;
            free_class_ids.push_back(first_class);
            first_class = nc;
        }

        int v = class_head[first_class];

        // Remove v from its class
        class_head[first_class] = vertex_next[v];
        if (vertex_next[v] != 0) vertex_prev[vertex_next[v]] = 0;

        // If the class becomes empty, remove it from the class list and
        // recycle its ID (no vertex references it and new_class[] is all
        // zero outside the refinement loop below).
        if (class_head[first_class] == 0) {
            int nc = class_next[first_class];
            int pc = class_prev[first_class];
            class_next[pc] = nc;
            if (nc != 0) class_prev[nc] = pc;
            free_class_ids.push_back(first_class);
        }

        used[v] = 1;
        res.order[i] = v;
        res.number[v] = i;

        // Partition refinement: separate v's unlabeled neighbors to the front of their class
        touched_classes.clear();

        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (used[u]) continue;

            int c = vertex_class[u];

            // If class c has not been split yet, create a new class before c
            if (new_class[c] == 0) {
                int nc;
                if (!free_class_ids.empty()) {
                    nc = free_class_ids.back();
                    free_class_ids.pop_back();
                } else {
                    nc = next_class_id++;
                    if (nc >= (int)class_next.size()) {
                        class_next.push_back(0);
                        class_prev.push_back(0);
                        class_head.push_back(0);
                        new_class.push_back(0);
                    }
                }

                // Insert nc before c
                int pc = class_prev[c];
                class_next[pc] = nc;
                class_prev[nc] = pc;
                class_next[nc] = c;
                class_prev[c] = nc;

                class_head[nc] = 0;
                new_class[c] = nc;
                touched_classes.push_back(c);
            }

            int nc = new_class[c];

            // Remove u from class c's vertex list
            if (vertex_prev[u] == 0) {
                class_head[c] = vertex_next[u];
                if (vertex_next[u] != 0) vertex_prev[vertex_next[u]] = 0;
            } else {
                vertex_next[vertex_prev[u]] = vertex_next[u];
                if (vertex_next[u] != 0) vertex_prev[vertex_next[u]] = vertex_prev[u];
            }

            // Insert u at the head of nc
            vertex_next[u] = class_head[nc];
            vertex_prev[u] = 0;
            if (class_head[nc] != 0) vertex_prev[class_head[nc]] = u;
            class_head[nc] = u;
            vertex_class[u] = nc;
        }

        // Remove empty original classes (recycling their IDs) and reset new_class
        for (size_t j = 0; j < touched_classes.size(); ++j) {
            int c = touched_classes[j];
            if (class_head[c] == 0) {
                int nc = class_next[c];
                int pc = class_prev[c];
                class_next[pc] = nc;
                if (nc != 0) class_prev[nc] = pc;
                free_class_ids.push_back(c);
            }
            new_class[c] = 0;
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief Computes a LexBFS ordering
 * @param g Input graph
 * @param algo Algorithm to use (default: PARTITION_LEXBFS)
 * @return MCSResult (order and number)
 *
 * For chordal graphs, the resulting order[1..n] is a perfect elimination ordering (PEO).
 * Returns results with the same interface as MCS.
 */
inline MCSResult lexbfs(const Graph& g,
    LexBFSAlgorithm algo = LexBFSAlgorithm::PARTITION_LEXBFS) {
    switch (algo) {
        case LexBFSAlgorithm::SIMPLE_LEXBFS:
            return detail::lexbfs_simple(g);
        case LexBFSAlgorithm::PARTITION_LEXBFS:
            return detail::lexbfs_partition(g);
        default:
            break;
    }
    return MCSResult();
}

} // namespace graph_recognition

#endif
