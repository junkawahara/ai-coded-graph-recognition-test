#include "biconnected.h"
#include "graph.h"
#include "planar.h"
#include "series_parallel.h"
#include "spqr_tree.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::SPQRNodeKind;
using graph_recognition::SPQRTreeResult;
using graph_recognition::check_biconnected;
using graph_recognition::check_planar;
using graph_recognition::check_series_parallel;
using graph_recognition::compute_spqr_tree;

// Brute force: {x, y} is a separation pair when removing both disconnects the
// rest. The virtual edge pairs of the tree must be exactly the separation
// pairs, together with the edges of the graph that a bond sits on.
std::set<std::pair<int, int>> bf_separation_pairs(const Graph& g) {
    std::set<std::pair<int, int>> pairs;
    int n = g.n;
    for (int x = 1; x <= n; ++x) {
        for (int y = x + 1; y <= n; ++y) {
            std::vector<bool> keep(n + 1, true);
            keep[x] = keep[y] = false;
            int start = 0;
            int remaining = 0;
            for (int v = 1; v <= n; ++v) {
                if (!keep[v]) continue;
                ++remaining;
                if (!start) start = v;
            }
            if (remaining < 2) continue;
            std::vector<bool> seen(n + 1, false);
            std::vector<int> stack(1, start);
            seen[start] = true;
            int reached = 1;
            while (!stack.empty()) {
                int a = stack.back();
                stack.pop_back();
                for (size_t i = 0; i < g.adj[a].size(); ++i) {
                    int b = g.adj[a][i];
                    if (!keep[b] || seen[b]) continue;
                    seen[b] = true;
                    ++reached;
                    stack.push_back(b);
                }
            }
            if (reached < remaining) pairs.insert(std::make_pair(x, y));
        }
    }
    return pairs;
}

// The n = 7 continuation of the exhaustive unit test, plus the separation
// pair cross-check, which is too slow to run for every smaller graph as well.
TEST(SPQRTreeProperty, ExhaustiveOnSevenVertices) {
    const int n = 7;
    std::vector<std::pair<int, int>> all;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
    int m = (int)all.size();
    for (int mask = 0; mask < (1 << m); ++mask) {
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < m; ++i)
            if (mask & (1 << i)) edges.push_back(all[i]);
        Graph g(n, edges);
        if (!check_biconnected(g).is_biconnected) continue;

        SPQRTreeResult r = compute_spqr_tree(g);
        ASSERT_TRUE(r.success) << "mask=" << mask;

        bool has_rigid = false;
        for (size_t i = 0; i < r.nodes.size(); ++i) {
            if (r.nodes[i].kind == SPQRNodeKind::R) has_rigid = true;
        }
        ASSERT_EQ(!has_rigid, check_series_parallel(g).is_series_parallel) << "mask=" << mask;

        // Every separation pair must appear together in some skeleton. This is
        // what catches over-splitting: a decomposition cut too finely would
        // leave some separating pair with no skeleton holding both. (The check
        // is about skeletons, not virtual edges -- the pair 1,3 of a polygon
        // 1-4-3-2 separates the graph without any virtual edge sitting on it,
        // because the polygon already stands for all of its own split pairs.)
        // Under-splitting is caught by the node labels instead: a component
        // left too coarse is neither a cycle, nor a bond, nor 3-connected.
        std::set<std::pair<int, int>> together;
        for (size_t i = 0; i < r.nodes.size(); ++i) {
            const std::vector<int>& verts = r.nodes[i].vertices;
            for (size_t a = 0; a < verts.size(); ++a) {
                for (size_t b = a + 1; b < verts.size(); ++b) {
                    together.insert(std::make_pair(verts[a], verts[b]));
                }
            }
        }
        std::set<std::pair<int, int>> sep = bf_separation_pairs(g);
        for (std::set<std::pair<int, int>>::const_iterator it = sep.begin(); it != sep.end();
             ++it) {
            ASSERT_TRUE(together.count(*it) > 0)
                << "mask=" << mask << " separation pair " << it->first << "," << it->second
                << " is in no skeleton";
        }
        // The converse does not hold for skeletons -- every pair of a rigid
        // skeleton appears together without separating anything -- but it does
        // hold for the virtual edges, which are exactly where the splits were
        // taken.
        for (size_t t = 0; t < r.tree_edges.size(); ++t) {
            std::pair<int, int> e = r.nodes[r.tree_edges[t].node_u].edges[r.tree_edges[t].edge_u];
            if (e.first > e.second) std::swap(e.first, e.second);
            ASSERT_TRUE(sep.count(e) > 0 || g.has_edge(e.first, e.second))
                << "mask=" << mask << " virtual pair " << e.first << "," << e.second
                << " is neither a separation pair nor an edge";
        }
    }
}

}  // namespace
