#ifndef GRAPH_RECOGNITION_HALIN_ENUM_H
#define GRAPH_RECOGNITION_HALIN_ENUM_H

/**
 * @file halin_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic Halin graphs
 *
 * Enumerates all non-isomorphic Halin graphs on n vertices via constructive enumeration.
 *
 * A Halin graph is constructed as follows:
 *   1. Take a tree T with no degree-2 vertices (HI-tree)
 *   2. Embed T in the plane
 *   3. Connect the leaves of T with a cycle in embedding order
 *
 * Algorithm:
 *   Generate non-isomorphic trees using the tree enumerator, filter for HI-trees.
 *   Enumerate all planar embeddings (cyclic adjacency order at each vertex) of each HI-tree,
 *   determine leaf order via DFS, and construct Halin graphs.
 *   Remove isomorphic duplicates using the plane tree bracket code
 *   (minimum over all roots, all rotations, and both reflections).
 *
 * Non-isomorphic counts: OEIS A346779
 *   0, 0, 0, 1, 1, 2, 2, 4, 6, 13, 22, 50, 106, 252, ...
 *
 * References:
 *   Halin, "Studies on minimally n-connected graphs,"
 *   Combinatorial Mathematics and its Applications, 1971
 */

#include "tree_unlabeled_enum.h"

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief An enumerated Halin graph
 */
struct HalinUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of Halin graph enumeration
 */
struct HalinUnlabeledEnumerationResult {
    std::vector<HalinUnlabeledEnumeratedGraph> graphs;
};

namespace detail {

/**
 * @brief Determines whether the tree is an HI-tree (no degree-2 vertices)
 */
inline bool is_hi_tree(int n,
                       const std::vector<std::pair<int, int> >& edges) {
    if (n < 4) return false;
    std::vector<int> deg(n + 1, 0);
    for (std::size_t i = 0; i < edges.size(); ++i) {
        ++deg[edges[i].first];
        ++deg[edges[i].second];
    }
    int leaf_count = 0;
    for (int v = 1; v <= n; ++v) {
        if (deg[v] == 2) return false;
        if (deg[v] == 1) ++leaf_count;
    }
    return leaf_count >= 3;
}

/**
 * @brief Builds the adjacency list
 */
inline std::vector<std::vector<int> > build_adj(
    int n, const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > adj(n + 1);
    for (std::size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first].push_back(edges[i].second);
        adj[edges[i].second].push_back(edges[i].first);
    }
    for (int v = 1; v <= n; ++v) {
        std::sort(adj[v].begin(), adj[v].end());
    }
    return adj;
}

/**
 * @brief Recursively computes the bracket code of a plane tree
 *
 * @param v Current vertex
 * @param parent Parent vertex (0 if root)
 * @param reflected Reflection flag (true to reverse cyclic order)
 * @param tree_adj Tree adjacency list
 * @param embedding Cyclic adjacency order at each vertex
 * @return Bracket code string
 */
inline std::string compute_plane_tree_code(
    int v, int parent, bool reflected,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::vector<int> >& embedding) {

    // Leaf
    if ((int)tree_adj[v].size() == 1 && parent != 0) {
        return "L";
    }

    const std::vector<int>& cyc = embedding[v];
    int k = (int)cyc.size();

    // Determine child order
    std::vector<int> children;
    if (parent == 0) {
        // Root: use cyclic order as-is (or reflected)
        if (!reflected) {
            for (int i = 0; i < k; ++i) {
                children.push_back(cyc[i]);
            }
        } else {
            children.push_back(cyc[0]);
            for (int i = k - 1; i >= 1; --i) {
                children.push_back(cyc[i]);
            }
        }
    } else {
        // Non-root: traverse starting from the position after parent
        int parent_pos = -1;
        for (int i = 0; i < k; ++i) {
            if (cyc[i] == parent) { parent_pos = i; break; }
        }
        if (!reflected) {
            for (int i = 1; i < k; ++i) {
                children.push_back(cyc[(parent_pos + i) % k]);
            }
        } else {
            for (int i = 1; i < k; ++i) {
                children.push_back(cyc[((parent_pos - i) % k + k) % k]);
            }
        }
    }

    std::string code = "(";
    for (int i = 0; i < (int)children.size(); ++i) {
        code += compute_plane_tree_code(
            children[i], v, reflected, tree_adj, embedding);
    }
    code += ")";
    return code;
}

/**
 * @brief Computes the canonical code of a plane tree
 *
 * Returns the minimum bracket code over all roots, all rotations, and both reflections.
 * Plane trees that yield the same Halin graph have the same canonical code.
 */
inline std::string canonical_plane_tree_code(
    int n,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::vector<int> >& embedding) {

    std::string min_code;
    bool first = true;

    for (int root = 1; root <= n; ++root) {
        int deg = (int)tree_adj[root].size();
        if (deg == 0) continue;

        // Try each rotation of the root's cyclic order
        for (int start = 0; start < deg; ++start) {
            // Create rotated embedding (root only)
            std::vector<std::vector<int> > emb_rot = embedding;
            emb_rot[root].clear();
            for (int i = 0; i < deg; ++i) {
                emb_rot[root].push_back(
                    embedding[root][(start + i) % deg]);
            }

            // Forward direction
            std::string code = compute_plane_tree_code(
                root, 0, false, tree_adj, emb_rot);
            if (first || code < min_code) {
                min_code = code;
                first = false;
            }

            // Reflected direction
            code = compute_plane_tree_code(
                root, 0, true, tree_adj, emb_rot);
            if (code < min_code) {
                min_code = code;
            }
        }
    }

    return min_code;
}

/**
 * @brief Collects leaves in embedding order via DFS on a rooted tree
 */
inline std::vector<int> collect_leaf_order(
    int root,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::vector<int> >& embedding) {

    std::vector<int> leaves;
    // DFS stack: (vertex, parent)
    std::vector<std::pair<int, int> > stack;
    stack.push_back(std::make_pair(root, 0));

    while (!stack.empty()) {
        int v = stack.back().first;
        int parent = stack.back().second;
        stack.pop_back();

        if ((int)tree_adj[v].size() == 1 && parent != 0) {
            leaves.push_back(v);
            continue;
        }

        const std::vector<int>& cyc = embedding[v];
        int k = (int)cyc.size();

        int parent_pos = -1;
        if (parent != 0) {
            for (int i = 0; i < k; ++i) {
                if (cyc[i] == parent) { parent_pos = i; break; }
            }
        }

        std::vector<int> children;
        if (parent_pos == -1) {
            for (int i = 0; i < k; ++i) {
                children.push_back(cyc[i]);
            }
        } else {
            for (int i = 1; i < k; ++i) {
                children.push_back(cyc[(parent_pos + i) % k]);
            }
        }

        for (int i = (int)children.size() - 1; i >= 0; --i) {
            stack.push_back(std::make_pair(children[i], v));
        }
    }

    return leaves;
}

/**
 * @brief Recursively enumerates planar embeddings and constructs Halin graphs
 */
inline void enumerate_embeddings_dfs(
    int vertex, int n,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::pair<int, int> >& tree_edges,
    std::vector<std::vector<int> >& embedding,
    int root,
    std::set<std::string>& seen,
    std::vector<HalinUnlabeledEnumeratedGraph>& results) {

    if (vertex > n) {
        // All vertex embeddings determined -> check duplicates via canonical code
        std::string code = canonical_plane_tree_code(n, tree_adj, embedding);

        if (seen.find(code) != seen.end()) return;
        seen.insert(code);

        // Construct the Halin graph
        std::vector<int> leaves = collect_leaf_order(root, tree_adj, embedding);
        if ((int)leaves.size() < 3) return;

        HalinUnlabeledEnumeratedGraph graph;
        graph.n = n;

        // Tree edges
        for (std::size_t i = 0; i < tree_edges.size(); ++i) {
            graph.edges.push_back(tree_edges[i]);
        }

        // Leaf cycle edges
        int L = (int)leaves.size();
        for (int i = 0; i < L; ++i) {
            int u = leaves[i];
            int v = leaves[(i + 1) % L];
            if (u > v) { int tmp = u; u = v; v = tmp; }
            graph.edges.push_back(std::make_pair(u, v));
        }
        std::sort(graph.edges.begin(), graph.edges.end());

        results.push_back(graph);
        return;
    }

    int d = (int)tree_adj[vertex].size();
    if (d <= 1) {
        embedding[vertex] = tree_adj[vertex];
        enumerate_embeddings_dfs(vertex + 1, n, tree_adj, tree_edges,
                                 embedding, root, seen, results);
        return;
    }

    // Degree d >= 2: fix the first neighbor, enumerate permutations of the rest
    std::vector<int> perm = tree_adj[vertex];
    std::sort(perm.begin(), perm.end());
    std::vector<int> rest(perm.begin() + 1, perm.end());

    do {
        embedding[vertex].clear();
        embedding[vertex].push_back(perm[0]);
        for (std::size_t i = 0; i < rest.size(); ++i) {
            embedding[vertex].push_back(rest[i]);
        }
        enumerate_embeddings_dfs(vertex + 1, n, tree_adj, tree_edges,
                                 embedding, root, seen, results);
    } while (std::next_permutation(rest.begin(), rest.end()));
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic Halin graphs on n vertices
 * @param n Number of vertices
 * @return HalinUnlabeledEnumerationResult
 *
 * Generates non-isomorphic trees using tree enumeration, and filters for HI-trees.
 * Enumerates all planar embeddings of each HI-tree to construct Halin graphs,
 * and removes duplicates using plane tree canonical codes.
 */
inline HalinUnlabeledEnumerationResult enumerate_halin_unlabeled_graphs(int n) {
    HalinUnlabeledEnumerationResult result;
    if (n < 4) return result;

    TreeUnlabeledEnumerationResult trees = enumerate_tree_unlabeled_graphs(n);
    std::set<std::string> seen;

    for (std::size_t t = 0; t < trees.graphs.size(); ++t) {
        const TreeUnlabeledEnumeratedGraph& tree = trees.graphs[t];

        if (!detail::is_hi_tree(n, tree.edges)) continue;

        std::vector<std::vector<int> > tree_adj =
            detail::build_adj(n, tree.edges);

        int root = -1;
        for (int v = 1; v <= n; ++v) {
            if ((int)tree_adj[v].size() >= 3) {
                root = v;
                break;
            }
        }
        if (root == -1) continue;

        std::vector<std::vector<int> > embedding(n + 1);
        detail::enumerate_embeddings_dfs(
            1, n, tree_adj, tree.edges, embedding,
            root, seen, result.graphs);
    }

    return result;
}

}  // namespace graph_recognition

#endif
