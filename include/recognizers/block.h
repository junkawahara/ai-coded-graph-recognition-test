#ifndef GRAPH_RECOGNITION_BLOCK_H
#define GRAPH_RECOGNITION_BLOCK_H

/**
 * @file block.h
 * @brief Block graph recognition
 *
 * Determines the graph is a block graph if every biconnected component is a clique.
 *
 * Algorithms:
 *   - DFS: biconnected component decomposition O(n+m) (default)
 *   - CHORDAL_DIAMOND_FREE: chordal + diamond-free test O(n+m+mDelta^2)
 *   - BLOCK_CUT_TREE: shared block decomposition + clique test per block
 */

#include "decompositions/block_cut_tree.h"
#include "recognizers/chordal.h"
#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "certificates/obstruction_extract.h"
#include <algorithm>
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for block graph recognition
 */
enum class BlockAlgorithm {
    DFS,                  /**< biconnected component decomposition by DFS (default) */
    CHORDAL_DIAMOND_FREE, /**< chordal + diamond-free test O(n+m+mDelta^2) */
    BLOCK_CUT_TREE        /**< shared block decomposition + clique test per block */
};

/**
 * @brief Result of block graph recognition
 */
struct BlockResult {
    bool is_block = false; /**< true if the graph is a block graph */
    Obstruction obstruction; /**< NO certificate: a HOLE or a DIAMOND. Filled only by
                                  CHORDAL_DIAMOND_FREE, whose own work produces it; the
                                  DFS and BLOCK_CUT_TREE variants run in O(n+m) and
                                  would have to search for the witness afterwards, so
                                  they leave kind == NONE. Use
                                  build_block_obstruction() to get one regardless of
                                  variant. Valid only when is_block == false */
};

namespace detail {

/** @brief DFS-based block graph checker (internal class) */
class BlockChecker {
public:
    explicit BlockChecker(const Graph& graph)
        : g(graph),
          adj(g.n + 1),
          tin(g.n + 1, 0),
          low(g.n + 1, 0),
          mark(g.n + 1, 0),
          timer(0),
          mark_token(0) {
        build_simple_graph();
    }

    bool run() {
        for (int v = 1; v <= g.n; ++v) {
            if (tin[v] != 0) continue;
            if (!dfs(v, -1)) return false;
            if (!edge_stack.empty()) return false;
        }
        return true;
    }

private:
    struct UEdge {
        int u, v;
    };

    const Graph& g;
    std::vector<UEdge> edges;
    std::vector<std::vector<std::pair<int, int>>> adj;
    std::vector<int> tin, low;
    std::vector<int> edge_stack;
    std::vector<int> mark;
    int timer;
    int mark_token;

    void build_simple_graph() {
        for (int u = 1; u <= g.n; ++u) {
            for (std::unordered_set<int>::const_iterator it = g.adj_set[u].begin();
                 it != g.adj_set[u].end(); ++it) {
                int v = *it;
                if (u >= v) continue;
                int eid = (int)edges.size();
                UEdge e;
                e.u = u;
                e.v = v;
                edges.push_back(e);
                adj[u].push_back(std::make_pair(v, eid));
                adj[v].push_back(std::make_pair(u, eid));
            }
        }
    }

    bool pop_component_and_check_clique(int stop_eid) {
        if (mark_token == INT_MAX) {
            std::fill(mark.begin(), mark.end(), 0);
            mark_token = 0;
        }
        mark_token++;

        std::vector<int> verts;
        int edge_count = 0;
        while (true) {
            if (edge_stack.empty()) return false;
            int eid = edge_stack.back();
            edge_stack.pop_back();
            edge_count++;

            int a = edges[eid].u;
            int b = edges[eid].v;
            if (mark[a] != mark_token) {
                mark[a] = mark_token;
                verts.push_back(a);
            }
            if (mark[b] != mark_token) {
                mark[b] = mark_token;
                verts.push_back(b);
            }
            if (eid == stop_eid) break;
        }

        long long k = (long long)verts.size();
        long long need = k * (k - 1) / 2;
        return (long long)edge_count == need;
    }

    bool dfs(int start, int start_parent_eid) {
        struct Frame {
            int v, parent_eid;
            size_t i;
        };
        std::vector<Frame> stack;
        Frame f0;
        f0.v = start;
        f0.parent_eid = start_parent_eid;
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
                    edge_stack.push_back(eid);
                    low[v] = std::min(low[v], tin[to]);
                }
            } else {
                stack.pop_back();
                if (!stack.empty()) {
                    Frame& parent = stack.back();
                    int pv = parent.v;
                    int eid = adj[pv][parent.i - 1].second;
                    low[pv] = std::min(low[pv], low[v]);
                    if (low[v] >= tin[pv]) {
                        if (!pop_component_and_check_clique(eid)) return false;
                    }
                }
            }
        }

        return true;
    }
};

/**
 * @brief Determines whether a diamond (K4-) exists as an induced subgraph
 *
 * A diamond consists of 4 vertices {u, v, w1, w2} with 5 edges (all except w1-w2).
 * For each edge (u, v), if there is a non-adjacent pair among common neighbors, it is a diamond.
 *
 * @param g Input graph
 * @return true if a diamond exists
 */
inline bool has_diamond(const Graph& g) {
    return !detail_obstruction::find_diamond(g).empty();
}

/**
 * @brief Block graph recognition via chordal + diamond-free test
 *
 * Block graph iff chordal and does not contain a diamond (K4-).
 * Based on the characterization by Bandelt-Mulder (1986).
 *
 * @param g Input graph
 * @return BlockResult
 */
inline BlockResult check_block_chordal_diamond_free(const Graph& g) {
    BlockResult res;
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) {
        res.is_block = false;
        res.obstruction = chordal.obstruction;
        return res;
    }
    std::vector<int> diamond = detail_obstruction::find_diamond(g);
    res.is_block = diamond.empty();
    if (!res.is_block) {
        res.obstruction = make_obstruction(ObstructionKind::DIAMOND, diamond);
    }
    return res;
}

/**
 * @brief Block graph recognition on top of the shared block decomposition
 *
 * Same characterization as the private DFS above -- every block is a clique --
 * but the decomposition itself comes from block_cut_tree.h, so only the
 * clique test remains here.
 */
inline BlockResult check_block_block_cut_tree(const Graph& g) {
    BlockResult res;
    BlockCutTreeResult bct = compute_block_cut_tree(g);
    for (size_t i = 0; i < bct.blocks.size(); ++i) {
        long long k = (long long)bct.blocks[i].size();
        if ((long long)bct.block_edges[i].size() != k * (k - 1) / 2) return res;
    }
    res.is_block = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a block graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DFS)
 * @return BlockResult
 *
 * Block graph if all biconnected components are cliques.
 */
inline BlockResult check_block(const Graph& g,
    BlockAlgorithm algo = BlockAlgorithm::DFS) {
    switch (algo) {
        case BlockAlgorithm::DFS: {
            BlockResult res;
            detail::BlockChecker checker(g);
            res.is_block = checker.run();
            return res;
        }
        case BlockAlgorithm::CHORDAL_DIAMOND_FREE:
            return detail::check_block_chordal_diamond_free(g);
        case BlockAlgorithm::BLOCK_CUT_TREE:
            return detail::check_block_block_cut_tree(g);
        default:
            break;
    }
    return BlockResult();
}

/**
 * @brief Builds a NO certificate for a non-block graph
 * @param g Input graph
 * @return A HOLE or a DIAMOND, or an empty obstruction if g is a block graph
 *
 * Block graphs are the chordal diamond-free graphs (Bandelt--Mulder 1986), so
 * one of the two witnesses always exists. Separated from check_block() because
 * the search costs more than the O(n+m) recognition it would otherwise slow
 * down.
 */
inline Obstruction build_block_obstruction(const Graph& g) {
    return detail::check_block_chordal_diamond_free(g).obstruction;
}

} // namespace graph_recognition

#endif
