#ifndef GRAPH_RECOGNITION_CACTUS_H
#define GRAPH_RECOGNITION_CACTUS_H

/**
 * @file cactus.h
 * @brief Cactus graph recognition
 *
 * A cactus graph if every biconnected component is a single edge or a simple cycle.
 *
 * Algorithms:
 *   - DFS: biconnected component decomposition by DFS (default)
 *   - BLOCK_CUT_TREE: shared block decomposition + edge/cycle test per block
 */

#include "decompositions/block_cut_tree.h"
#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "certificates/obstruction_extract.h"
#include <algorithm>
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for cactus graph recognition
 */
enum class CactusAlgorithm {
    DFS,           /**< biconnected component decomposition by DFS (default) */
    BLOCK_CUT_TREE /**< shared block decomposition + edge/cycle test per block */
};

/**
 * @brief Result of cactus graph recognition
 */
struct CactusResult {
    bool is_cactus = false; /**< true if the graph is a cactus graph */
    Obstruction obstruction; /**< NO certificate: TWO_CYCLES_SHARING_EDGE. Left empty
                                  by both variants, which run in O(n+m) and would
                                  have to search for the second cycle afterwards; use
                                  build_cactus_obstruction(). Valid only when
                                  is_cactus == false */
};

namespace detail_cactus {

/** @brief DFS checker for cactus recognition (internal class) */
class CactusChecker {
public:
    explicit CactusChecker(const Graph& graph)
        : g(graph),
          adj(g.n + 1),
          tin(g.n + 1, 0),
          low(g.n + 1, 0),
          mark(g.n + 1, 0),
          comp_deg(g.n + 1, 0),
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
    std::vector<int> comp_deg;
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

    bool pop_component_and_check_cactus(int stop_eid) {
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
                comp_deg[a] = 0;
                verts.push_back(a);
            }
            if (mark[b] != mark_token) {
                mark[b] = mark_token;
                comp_deg[b] = 0;
                verts.push_back(b);
            }

            comp_deg[a]++;
            comp_deg[b]++;

            if (eid == stop_eid) break;
        }

        if (edge_count == 1) return true;

        if ((int)verts.size() < 3) return false;
        if (edge_count != (int)verts.size()) return false;

        for (size_t i = 0; i < verts.size(); ++i) {
            if (comp_deg[verts[i]] != 2) return false;
        }

        return true;
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
                        if (!pop_component_and_check_cactus(eid)) return false;
                    }
                }
            }
        }

        return true;
    }
};

/**
 * @brief Cactus recognition on top of the shared block decomposition
 *
 * A block on k vertices with e edges is a single edge when e == 1, and a
 * simple cycle when e == k with k >= 3: a biconnected graph on k >= 3
 * vertices has minimum degree 2, so e == k forces every degree to be exactly
 * 2, and a connected 2-regular graph is one cycle. Isolated vertices appear
 * as K1 blocks and are accepted, matching the DFS variant.
 */
inline CactusResult check_cactus_block_cut_tree(const Graph& g) {
    CactusResult res;
    BlockCutTreeResult bct = compute_block_cut_tree(g);
    for (size_t i = 0; i < bct.blocks.size(); ++i) {
        size_t k = bct.blocks[i].size();
        size_t e = bct.block_edges[i].size();
        bool ok = (e == 0 && k == 1) || (e == 1 && k == 2) || (e == k && k >= 3);
        if (!ok) return res;
    }
    res.is_cactus = true;
    return res;
}

} // namespace detail_cactus

/**
 * @brief Determines whether a graph is a cactus graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DFS)
 * @return CactusResult
 */
inline CactusResult check_cactus(const Graph& g,
    CactusAlgorithm algo = CactusAlgorithm::DFS) {
    switch (algo) {
        case CactusAlgorithm::DFS: {
            CactusResult res;
            detail_cactus::CactusChecker checker(g);
            res.is_cactus = checker.run();
            return res;
        }
        case CactusAlgorithm::BLOCK_CUT_TREE:
            return detail_cactus::check_cactus_block_cut_tree(g);
        default:
            break;
    }
    return CactusResult();
}

namespace detail_cactus {

/**
 * @brief Shortest u-v path avoiding up to two forbidden edges
 * @return The path from u to v inclusive, or empty if none exists
 */
inline std::vector<int> path_avoiding_edges(const Graph& g, int u, int v,
                                            int ban1_a, int ban1_b,
                                            int ban2_a, int ban2_b) {
    std::vector<int> path;
    std::vector<int> parent(g.n + 1, 0);
    std::vector<unsigned char> seen(g.n + 1, 0);
    std::vector<int> queue;
    seen[u] = 1;
    queue.push_back(u);
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int cur = queue[qi];
        for (size_t i = 0; i < g.adj[cur].size(); ++i) {
            int w = g.adj[cur][i];
            if ((cur == ban1_a && w == ban1_b) || (cur == ban1_b && w == ban1_a)) continue;
            if ((cur == ban2_a && w == ban2_b) || (cur == ban2_b && w == ban2_a)) continue;
            if (seen[w]) continue;
            seen[w] = 1;
            parent[w] = cur;
            if (w == v) return detail_obstruction::path_from_bfs_parents(parent, v);
            queue.push_back(w);
        }
    }
    return path;
}

} // namespace detail_cactus

/**
 * @brief Builds a NO certificate for a non-cactus graph
 * @param g Input graph
 * @return TWO_CYCLES_SHARING_EDGE, or an empty obstruction if g is a cactus
 *
 * A cactus is a graph in which every edge lies on at most one cycle, so the
 * witness is an edge on two of them. For each edge uv it takes a cycle through
 * uv, then drops one of that cycle's other edges and searches again: a second
 * path found this way closes into a cycle that is missing the dropped edge and
 * so differs from the first. Every second cycle is caught, because two
 * distinct simple cycles never contain one another's edge sets.
 *
 * Separated from check_cactus() because both variants recognize in O(n+m) and
 * this search does not.
 */
inline Obstruction build_cactus_obstruction(const Graph& g) {
    Obstruction o;
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (v <= u) continue;

            std::vector<int> first =
                detail_cactus::path_avoiding_edges(g, u, v, u, v, 0, 0);
            if (first.size() < 3) continue;  // uv is a bridge

            for (size_t j = 0; j + 1 < first.size(); ++j) {
                std::vector<int> second = detail_cactus::path_avoiding_edges(
                    g, u, v, u, v, first[j], first[j + 1]);
                if (second.size() < 3) continue;

                o.kind = ObstructionKind::TWO_CYCLES_SHARING_EDGE;
                o.vertices.push_back(u);
                o.vertices.push_back(v);
                o.vertex_sets.push_back(first);
                o.vertex_sets.push_back(second);
                return o;
            }
        }
    }
    return o;
}

} // namespace graph_recognition

#endif
