#include "decompositions/spqr_tree.h"

#include "decompositions/components.h"
#include "recognizers/planar.h"
#include "recognizers/series_parallel.h"
#include "recognizers/triconnected.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

Graph make_cycle(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(n, 1));
    return Graph(n, edges);
}

Graph make_complete(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) edges.push_back(std::make_pair(u, v));
    return Graph(n, edges);
}

// Contract every tree edge: delete the two virtual edges and identify the
// skeletons. What remains must be the input graph's edge set.
bool recomposes_to(const Graph& g, const SPQRTreeResult& r) {
    // Each real edge must appear exactly once across all skeletons, and each
    // virtual edge exactly once as an end of a tree edge.
    size_t m = 0;
    for (int v = 1; v <= g.n; ++v) m += g.adj[v].size();
    m /= 2;

    std::vector<int> real_seen(m, 0);
    size_t virtual_count = 0;
    for (size_t i = 0; i < r.nodes.size(); ++i) {
        const SPQRNode& node = r.nodes[i];
        if (node.edges.size() != node.edge_orig.size()) return false;
        for (size_t e = 0; e < node.edges.size(); ++e) {
            int o = node.edge_orig[e];
            if (o < 0) {
                ++virtual_count;
                continue;
            }
            if (o >= (int)m || real_seen[o]) return false;
            real_seen[o] = 1;
            if (!g.has_edge(node.edges[e].first, node.edges[e].second)) return false;
        }
    }
    for (size_t i = 0; i < m; ++i) {
        if (!real_seen[i]) return false;
    }
    if (virtual_count != 2 * r.tree_edges.size()) return false;

    // The tree edges pair distinct virtual edges with matching endpoints, and
    // the nodes form a tree.
    std::set<std::pair<int, int>> used;
    std::vector<std::vector<int>> tree(r.nodes.size());
    for (size_t t = 0; t < r.tree_edges.size(); ++t) {
        const SPQRTreeEdge& te = r.tree_edges[t];
        if (te.node_u < 0 || te.node_u >= (int)r.nodes.size()) return false;
        if (te.node_v < 0 || te.node_v >= (int)r.nodes.size()) return false;
        if (te.node_u == te.node_v) return false;
        if (r.nodes[te.node_u].edge_orig[te.edge_u] != -1) return false;
        if (r.nodes[te.node_v].edge_orig[te.edge_v] != -1) return false;
        if (!used.insert(std::make_pair(te.node_u, te.edge_u)).second) return false;
        if (!used.insert(std::make_pair(te.node_v, te.edge_v)).second) return false;
        // A virtual pair stands for the same vertex pair on both sides.
        std::pair<int, int> a = r.nodes[te.node_u].edges[te.edge_u];
        std::pair<int, int> b = r.nodes[te.node_v].edges[te.edge_v];
        if (a.first > a.second) std::swap(a.first, a.second);
        if (b.first > b.second) std::swap(b.first, b.second);
        if (a != b) return false;
        tree[te.node_u].push_back(te.node_v);
        tree[te.node_v].push_back(te.node_u);
    }
    if (used.size() != virtual_count) return false;
    if (r.tree_edges.size() != r.nodes.size() - 1) return false;
    {
        std::vector<bool> seen(r.nodes.size(), false);
        std::vector<int> stack(1, 0);
        seen[0] = true;
        size_t count = 1;
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (size_t i = 0; i < tree[x].size(); ++i) {
                int y = tree[x][i];
                if (seen[y]) continue;
                seen[y] = true;
                ++count;
                stack.push_back(y);
            }
        }
        if (count != r.nodes.size()) return false;
    }
    return true;
}

// The labels must match the skeletons, and no two adjacent nodes may share a
// label that the merge pass should have collapsed.
bool labels_are_canonical(const SPQRTreeResult& r) {
    for (size_t i = 0; i < r.nodes.size(); ++i) {
        const SPQRNode& node = r.nodes[i];
        std::vector<int> degree;
        std::vector<int> verts = node.vertices;
        degree.assign(verts.size(), 0);
        for (size_t e = 0; e < node.edges.size(); ++e) {
            int a = (int)(std::lower_bound(verts.begin(), verts.end(), node.edges[e].first) -
                          verts.begin());
            int b = (int)(std::lower_bound(verts.begin(), verts.end(), node.edges[e].second) -
                          verts.begin());
            ++degree[a];
            ++degree[b];
        }
        if (node.kind == SPQRNodeKind::P) {
            if (verts.size() != 2 || node.edges.size() < 3) return false;
        } else if (node.kind == SPQRNodeKind::S) {
            if (verts.size() < 3 || node.edges.size() != verts.size()) return false;
            for (size_t d = 0; d < degree.size(); ++d) {
                if (degree[d] != 2) return false;
            }
        } else {
            // R: simple and 3-connected.
            if (verts.size() < 4) return false;
            std::set<std::pair<int, int>> seen;
            std::vector<std::pair<int, int>> local;
            for (size_t e = 0; e < node.edges.size(); ++e) {
                std::pair<int, int> key = node.edges[e];
                if (key.first > key.second) std::swap(key.first, key.second);
                if (!seen.insert(key).second) return false;  // parallel edge in an R node
                int a = (int)(std::lower_bound(verts.begin(), verts.end(), key.first) -
                              verts.begin()) + 1;
                int b = (int)(std::lower_bound(verts.begin(), verts.end(), key.second) -
                              verts.begin()) + 1;
                local.push_back(std::make_pair(a, b));
            }
            Graph skel((int)verts.size(), local);
            if (!check_triconnected(skel).is_triconnected) return false;
        }
    }
    for (size_t t = 0; t < r.tree_edges.size(); ++t) {
        SPQRNodeKind a = r.nodes[r.tree_edges[t].node_u].kind;
        SPQRNodeKind b = r.nodes[r.tree_edges[t].node_v].kind;
        if (a == b && a != SPQRNodeKind::R) return false;  // S-S or P-P left unmerged
    }
    return true;
}

TEST(SPQRTreeTest, NonBiconnectedInputIsRejected) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    Graph path(3, edges);
    EXPECT_FALSE(compute_spqr_tree(path).success);
    EXPECT_FALSE(compute_spqr_tree(Graph()).success);
}

TEST(SPQRTreeTest, CycleIsASinglePolygon) {
    Graph g = make_cycle(6);
    SPQRTreeResult r = compute_spqr_tree(g);
    ASSERT_TRUE(r.success);
    ASSERT_EQ(r.nodes.size(), 1u);
    EXPECT_EQ(r.nodes[0].kind, SPQRNodeKind::S);
    EXPECT_TRUE(r.tree_edges.empty());
    EXPECT_TRUE(recomposes_to(g, r));
    EXPECT_TRUE(labels_are_canonical(r));
}

TEST(SPQRTreeTest, K4IsASingleRigidNode) {
    Graph g = make_complete(4);
    SPQRTreeResult r = compute_spqr_tree(g);
    ASSERT_TRUE(r.success);
    ASSERT_EQ(r.nodes.size(), 1u);
    EXPECT_EQ(r.nodes[0].kind, SPQRNodeKind::R);
    EXPECT_TRUE(recomposes_to(g, r));
    EXPECT_TRUE(labels_are_canonical(r));
}

TEST(SPQRTreeTest, TwoTrianglesSharingAnEdge) {
    // K4 minus an edge: two polygons hanging off one bond.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(1, 3));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(1, 4));
    edges.push_back(std::make_pair(2, 4));
    Graph g(4, edges);
    SPQRTreeResult r = compute_spqr_tree(g);
    ASSERT_TRUE(r.success);
    int s = 0, p = 0, rigid = 0;
    for (size_t i = 0; i < r.nodes.size(); ++i) {
        if (r.nodes[i].kind == SPQRNodeKind::S) ++s;
        else if (r.nodes[i].kind == SPQRNodeKind::P) ++p;
        else ++rigid;
    }
    EXPECT_EQ(s, 2);
    EXPECT_EQ(p, 1);
    EXPECT_EQ(rigid, 0);
    EXPECT_TRUE(recomposes_to(g, r));
    EXPECT_TRUE(labels_are_canonical(r));
}

// Every biconnected graph on up to 6 vertices: the tree must account for
// every edge, be a tree, have correctly labelled and canonical nodes, have no
// rigid node exactly when the graph is series-parallel, and be planar exactly
// when all its rigid skeletons are. The n = 7 sweep is in the property test.
TEST(SPQRTreeTest, ExhaustiveStructureAndSeriesParallel) {
    for (int n = 3; n <= 6; ++n) {
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
            ASSERT_TRUE(r.success) << "n=" << n << " mask=" << mask;
            ASSERT_TRUE(recomposes_to(g, r)) << "n=" << n << " mask=" << mask;
            ASSERT_TRUE(labels_are_canonical(r)) << "n=" << n << " mask=" << mask;

            bool has_rigid = false;
            bool rigid_planar = true;
            for (size_t i = 0; i < r.nodes.size(); ++i) {
                if (r.nodes[i].kind != SPQRNodeKind::R) continue;
                has_rigid = true;
                const std::vector<int>& verts = r.nodes[i].vertices;
                std::vector<std::pair<int, int>> local;
                for (size_t e = 0; e < r.nodes[i].edges.size(); ++e) {
                    int a = (int)(std::lower_bound(verts.begin(), verts.end(),
                                                   r.nodes[i].edges[e].first) - verts.begin()) + 1;
                    int b = (int)(std::lower_bound(verts.begin(), verts.end(),
                                                   r.nodes[i].edges[e].second) - verts.begin()) + 1;
                    local.push_back(std::make_pair(a, b));
                }
                Graph skel((int)verts.size(), local);
                if (!check_planar(skel).is_planar) rigid_planar = false;
            }
            ASSERT_EQ(!has_rigid, check_series_parallel(g).is_series_parallel)
                << "n=" << n << " mask=" << mask;
            // A graph is planar exactly when every rigid skeleton is: the
            // polygons and bonds can always be drawn.
            ASSERT_EQ(rigid_planar, check_planar(g).is_planar)
                << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
