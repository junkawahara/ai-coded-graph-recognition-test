#ifndef GRAPH_RECOGNITION_MD_TREE_H
#define GRAPH_RECOGNITION_MD_TREE_H

/**
 * @file md_tree.h
 * @brief Decomposition tree shared by the modular decomposition and the cotree
 *
 * A node of the tree stands for a module: its leaves are single vertices, and
 * an internal node is labelled by how its children relate to each other --
 * PARALLEL when no two children are joined, SERIES when every two are, and
 * PRIME otherwise. A cotree is exactly such a tree without PRIME nodes, so
 * cograph.h and modular_decomposition.h build the same structure and can be
 * compared directly.
 *
 * Builders only need to create nodes (a parent always before its children,
 * i.e. with a smaller index) and record leaf vertices; md_finalize() then
 * derives the vertex set of every node, orders the children canonically and
 * builds the quotient graphs.
 */

#include "graph.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Label of a decomposition tree node
 */
enum class MDNodeKind {
    LEAF,     /**< a single vertex */
    SERIES    /**< every two children are completely joined (a join) */,
    PARALLEL, /**< no two children are joined (a disjoint union) */
    PRIME     /**< neither; the quotient has only trivial modules */
};

/**
 * @brief A node of a decomposition tree
 */
struct MDNode {
    MDNodeKind kind = MDNodeKind::LEAF; /**< label of this node */
    int vertex = 0;                     /**< leaf: the vertex; internal nodes: 0 */
    int parent = -1;                    /**< index of the parent node, -1 at the root */
    std::vector<int> children;          /**< child node indices, ordered by smallest vertex */
    std::vector<int> vertices;          /**< the module this node represents, ascending */
    Graph quotient;                     /**< internal nodes: quotient on the children,
                                             where quotient vertex j+1 is children[j] */
};

/**
 * @brief A decomposition tree
 */
struct MDTree {
    std::vector<MDNode> nodes; /**< tree nodes; a parent always precedes its children */
    int root = -1;             /**< index of the root node, -1 for the empty graph */
    std::vector<int> leaf_of;  /**< leaf_of[v] = node index of the leaf for v (size n+1) */
};

/**
 * @brief Completes a freshly built tree
 * @param t Tree whose nodes carry kind, parent, children and leaf vertices
 * @param g The graph the tree decomposes
 *
 * Fills in the vertex set of every node, sorts the children of each node by
 * their smallest vertex, builds the quotient graph of every internal node and
 * indexes the leaves. Relies on every node having a smaller index than its
 * children, which is what a top-down builder produces naturally.
 */
inline void md_finalize(MDTree& t, const Graph& g) {
    size_t count = t.nodes.size();
    for (size_t i = 0; i < count; ++i) {
        t.nodes[i].vertices.clear();
        if (t.nodes[i].kind == MDNodeKind::LEAF) {
            t.nodes[i].vertices.push_back(t.nodes[i].vertex);
        }
    }
    // Children come after their parent, so one reverse sweep propagates the
    // vertex sets upwards.
    for (size_t i = count; i-- > 0;) {
        std::vector<int>& mine = t.nodes[i].vertices;
        std::sort(mine.begin(), mine.end());
        int parent = t.nodes[i].parent;
        if (parent >= 0) {
            std::vector<int>& up = t.nodes[parent].vertices;
            up.insert(up.end(), mine.begin(), mine.end());
        }
    }

    for (size_t i = 0; i < count; ++i) {
        MDNode& node = t.nodes[i];
        if (node.children.empty()) continue;
        std::vector<std::pair<int, int>> by_min;
        by_min.reserve(node.children.size());
        for (size_t j = 0; j < node.children.size(); ++j) {
            int c = node.children[j];
            int key = t.nodes[c].vertices.empty() ? 0 : t.nodes[c].vertices[0];
            by_min.push_back(std::make_pair(key, c));
        }
        std::sort(by_min.begin(), by_min.end());
        for (size_t j = 0; j < by_min.size(); ++j) node.children[j] = by_min[j].second;

        int k = (int)node.children.size();
        std::vector<std::pair<int, int>> edges;
        for (int a = 0; a < k; ++a) {
            const std::vector<int>& va = t.nodes[node.children[a]].vertices;
            if (va.empty()) continue;
            for (int b = a + 1; b < k; ++b) {
                const std::vector<int>& vb = t.nodes[node.children[b]].vertices;
                if (vb.empty()) continue;
                // Children are modules, so one representative pair decides.
                if (g.has_edge(va[0], vb[0])) edges.push_back(std::make_pair(a + 1, b + 1));
            }
        }
        node.quotient = Graph(k, edges);
    }

    t.leaf_of.assign(g.n + 1, -1);
    for (size_t i = 0; i < count; ++i) {
        if (t.nodes[i].kind != MDNodeKind::LEAF) continue;
        int v = t.nodes[i].vertex;
        if (v >= 1 && v <= g.n) t.leaf_of[v] = (int)i;
    }
}

/**
 * @brief Rebuilds the graph a decomposition tree describes
 * @param t Decomposition tree
 * @param n Number of vertices of the original graph
 * @return The graph obtained by substituting the children into each quotient
 *
 * Two vertices are adjacent exactly when, at their lowest common ancestor,
 * the two children holding them are adjacent in that node's quotient. Used to
 * check a tree against the graph it came from.
 */
inline Graph md_rebuild_graph(const MDTree& t, int n) {
    std::vector<std::pair<int, int>> edges;
    for (size_t i = 0; i < t.nodes.size(); ++i) {
        const MDNode& node = t.nodes[i];
        int k = (int)node.children.size();
        for (int a = 0; a < k; ++a) {
            for (int b = a + 1; b < k; ++b) {
                if (!node.quotient.has_edge(a + 1, b + 1)) continue;
                const std::vector<int>& va = t.nodes[node.children[a]].vertices;
                const std::vector<int>& vb = t.nodes[node.children[b]].vertices;
                for (size_t x = 0; x < va.size(); ++x) {
                    for (size_t y = 0; y < vb.size(); ++y) {
                        edges.push_back(std::make_pair(va[x], vb[y]));
                    }
                }
            }
        }
    }
    return Graph(n, edges);
}

/**
 * @brief Whether a decomposition tree is a cotree (has no PRIME node)
 */
inline bool md_is_cotree(const MDTree& t) {
    for (size_t i = 0; i < t.nodes.size(); ++i) {
        if (t.nodes[i].kind == MDNodeKind::PRIME) return false;
    }
    return true;
}

} // namespace graph_recognition

#endif
