#ifndef GRAPH_RECOGNITION_BLOCK_CUT_TREE_H
#define GRAPH_RECOGNITION_BLOCK_CUT_TREE_H

/**
 * @file block_cut_tree.h
 * @brief Biconnected components, cut vertices, bridges and the block-cut tree
 *
 * The decomposition is computed by the usual edge-stack DFS (Hopcroft &
 * Tarjan 1973): when a child subtree cannot reach above its parent, the edges
 * pushed since that child form a biconnected component and are popped.
 *
 * Cut vertices and bridges are not detected separately. A vertex is a cut
 * vertex exactly when it lies in more than one block, and a bridge is exactly
 * a block consisting of a single edge; deriving both from the blocks avoids
 * the root-of-the-DFS-tree special case that vertex-based articulation tests
 * need.
 *
 * An isolated vertex is reported as a block of its own (a K1 block) so that
 * every vertex belongs to at least one block and the block-cut forest covers
 * the whole graph. A disconnected graph yields a forest, one tree per
 * connected component; the structure is called a tree here by convention.
 */

#include "util/graph.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Biconnected components and the block-cut tree of a graph
 *
 * Block-cut tree node ids: 0 .. blocks.size()-1 are the block nodes, and
 * blocks.size() .. blocks.size()+c-1 are the cut vertex nodes.
 */
struct BlockCutTreeResult {
    std::vector<std::vector<int>> blocks; /**< blocks[i] = vertex set of block i, ascending */
    std::vector<std::vector<std::pair<int, int>>> block_edges; /**< block_edges[i] = edges of block i (u < v) */
    std::vector<unsigned char> is_cut;    /**< is_cut[v] = 1 iff v is a cut vertex (size n+1) */
    std::vector<std::pair<int, int>> bridges; /**< bridge edges (u < v) */
    std::vector<std::vector<int>> block_of;   /**< block_of[v] = indices of the blocks containing v (size n+1) */
    std::vector<std::vector<int>> tree;       /**< adjacency of the block-cut forest */
    std::vector<int> cut_vertex_of_node;      /**< node id -> vertex, 0 for block nodes */
    std::vector<int> node_of_cut_vertex;      /**< vertex -> node id, -1 if not a cut vertex (size n+1) */
};

namespace detail {

/** @brief Undirected edge with endpoint pair, indexed by edge id */
struct BctEdge {
    int u, v;
};

} // namespace detail

/**
 * @brief Computes the biconnected components and the block-cut tree
 * @param g Input graph
 * @return BlockCutTreeResult
 */
inline BlockCutTreeResult compute_block_cut_tree(const Graph& g) {
    int n = g.n;
    BlockCutTreeResult res;
    res.is_cut.assign(n + 1, 0);
    res.block_of.assign(n + 1, std::vector<int>());
    res.node_of_cut_vertex.assign(n + 1, -1);

    std::vector<detail::BctEdge> edges;
    std::vector<std::vector<std::pair<int, int>>> adj(n + 1);
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u >= v) continue;
            int eid = (int)edges.size();
            detail::BctEdge e;
            e.u = u;
            e.v = v;
            edges.push_back(e);
            adj[u].push_back(std::make_pair(v, eid));
            adj[v].push_back(std::make_pair(u, eid));
        }
    }

    std::vector<int> tin(n + 1, 0), low(n + 1, 0);
    std::vector<int> edge_stack;
    int timer = 0;

    struct Frame {
        int v;
        int parent_eid;
        size_t i;
    };
    std::vector<Frame> stack;

    for (int start = 1; start <= n; ++start) {
        if (tin[start] != 0) continue;
        if (adj[start].empty()) {
            // Isolated vertex: a K1 block with no edges.
            res.blocks.push_back(std::vector<int>(1, start));
            res.block_edges.push_back(std::vector<std::pair<int, int>>());
            tin[start] = low[start] = ++timer;
            continue;
        }

        Frame f0;
        f0.v = start;
        f0.parent_eid = -1;
        f0.i = 0;
        stack.push_back(f0);
        tin[start] = low[start] = ++timer;

        while (!stack.empty()) {
            Frame& f = stack.back();
            int v = f.v;
            if (f.i < adj[v].size()) {
                int to = adj[v][f.i].first;
                int eid = adj[v][f.i].second;
                f.i++;
                if (eid == f.parent_eid) continue;
                if (tin[to] == 0) {
                    edge_stack.push_back(eid);
                    tin[to] = low[to] = ++timer;
                    Frame fn;
                    fn.v = to;
                    fn.parent_eid = eid;
                    fn.i = 0;
                    stack.push_back(fn);
                } else if (tin[to] < tin[v]) {
                    // Back edge to an ancestor; pushed only from the
                    // descendant side so each edge enters the stack once.
                    edge_stack.push_back(eid);
                    low[v] = std::min(low[v], tin[to]);
                }
            } else {
                stack.pop_back();
                if (stack.empty()) break;
                Frame& parent = stack.back();
                int pv = parent.v;
                int eid = adj[pv][parent.i - 1].second;
                low[pv] = std::min(low[pv], low[v]);
                if (low[v] >= tin[pv]) {
                    // The edges pushed since (pv, v) form one block.
                    std::vector<std::pair<int, int>> comp_edges;
                    while (!edge_stack.empty()) {
                        int e = edge_stack.back();
                        edge_stack.pop_back();
                        comp_edges.push_back(std::make_pair(edges[e].u, edges[e].v));
                        if (e == eid) break;
                    }
                    std::vector<int> verts;
                    verts.reserve(comp_edges.size() * 2);
                    for (size_t i = 0; i < comp_edges.size(); ++i) {
                        verts.push_back(comp_edges[i].first);
                        verts.push_back(comp_edges[i].second);
                    }
                    std::sort(verts.begin(), verts.end());
                    verts.erase(std::unique(verts.begin(), verts.end()), verts.end());
                    std::sort(comp_edges.begin(), comp_edges.end());
                    res.blocks.push_back(verts);
                    res.block_edges.push_back(comp_edges);
                }
            }
        }
    }

    for (size_t i = 0; i < res.blocks.size(); ++i) {
        for (size_t j = 0; j < res.blocks[i].size(); ++j) {
            res.block_of[res.blocks[i][j]].push_back((int)i);
        }
        if (res.block_edges[i].size() == 1) res.bridges.push_back(res.block_edges[i][0]);
    }
    std::sort(res.bridges.begin(), res.bridges.end());
    for (int v = 1; v <= n; ++v) {
        if (res.block_of[v].size() > 1) res.is_cut[v] = 1;
    }

    int num_blocks = (int)res.blocks.size();
    res.cut_vertex_of_node.assign(num_blocks, 0);
    for (int v = 1; v <= n; ++v) {
        if (!res.is_cut[v]) continue;
        res.node_of_cut_vertex[v] = (int)res.cut_vertex_of_node.size();
        res.cut_vertex_of_node.push_back(v);
    }
    res.tree.assign(res.cut_vertex_of_node.size(), std::vector<int>());
    for (int i = 0; i < num_blocks; ++i) {
        for (size_t j = 0; j < res.blocks[i].size(); ++j) {
            int v = res.blocks[i][j];
            if (!res.is_cut[v]) continue;
            int node = res.node_of_cut_vertex[v];
            res.tree[i].push_back(node);
            res.tree[node].push_back(i);
        }
    }
    return res;
}

} // namespace graph_recognition

#endif
