#include "split_decomposition.h"

#include "components.h"
#include "distance_hereditary.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

// Oracle: every bipartition, tested straight from the definition.
bool bf_has_split(const Graph& g) {
    int n = g.n;
    if (n < 4) return false;
    for (int mask = 1; mask < (1 << n) - 1; ++mask) {
        std::vector<int> side(n + 1, 0);
        int count0 = 0;
        for (int v = 1; v <= n; ++v) {
            side[v] = ((mask >> (v - 1)) & 1) ? 0 : 1;
            if (side[v] == 0) ++count0;
        }
        if (count0 < 2 || n - count0 < 2) continue;

        std::vector<int> a1, b1;
        for (int v = 1; v <= n; ++v) {
            bool sees = false;
            for (int u = 1; u <= n; ++u) {
                if (side[u] != side[v] && g.has_edge(u, v)) sees = true;
            }
            if (!sees) continue;
            if (side[v] == 0) a1.push_back(v);
            else b1.push_back(v);
        }
        bool ok = true;
        for (size_t i = 0; i < a1.size() && ok; ++i) {
            for (size_t j = 0; j < b1.size() && ok; ++j) {
                if (!g.has_edge(a1[i], b1[j])) ok = false;
            }
        }
        if (ok) return true;
    }
    return false;
}

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

Graph make_cycle(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(n, 1));
    return Graph(n, edges);
}

TEST(FindSplitTest, TooSmallGraphsHaveNoSplit) {
    for (int n = 0; n <= 3; ++n) {
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) edges.push_back(std::make_pair(u, v));
        Graph g(n, edges);
        EXPECT_FALSE(find_split(g).found) << "n=" << n;
    }
}

TEST(FindSplitTest, DisconnectedGraphReportsNoSplit) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    EXPECT_FALSE(find_split(g).found);
}

TEST(FindSplitTest, PathHasASplit) {
    Graph g = make_path(4);
    SplitResultPair r = find_split(g);
    ASSERT_TRUE(r.found);
    EXPECT_TRUE(detail_split::is_split(g, r.side));
}

TEST(FindSplitTest, CompleteGraphHasASplit) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= 5; ++u)
        for (int v = u + 1; v <= 5; ++v) edges.push_back(std::make_pair(u, v));
    Graph g(5, edges);
    SplitResultPair r = find_split(g);
    ASSERT_TRUE(r.found);
    EXPECT_TRUE(detail_split::is_split(g, r.side));
}

TEST(FindSplitTest, FiveCycleIsPrime) {
    // C5 is the smallest prime graph for split decomposition.
    Graph g = make_cycle(5);
    EXPECT_FALSE(find_split(g).found);
    EXPECT_FALSE(bf_has_split(g));
}

// Every connected graph on up to 6 vertices: the finder must report a split
// exactly when one exists, and whatever it reports must be one. The n = 7
// sweep lives in the property test, where the 2^21 graphs are affordable.
TEST(FindSplitTest, ExhaustiveAgreementWithBruteForce) {
    for (int n = 4; n <= 6; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);
            if (connected_components(g).count != 1) continue;

            SplitResultPair r = find_split(g);
            ASSERT_EQ(r.found, bf_has_split(g)) << "n=" << n << " mask=" << mask;
            if (r.found) {
                ASSERT_TRUE(detail_split::is_split(g, r.side)) << "n=" << n << " mask=" << mask;
            }
        }
    }
}

}  // namespace

// --- Split decomposition tree ---

namespace {

// Independently recompose the tree: repeatedly join two bags across a tree
// edge (delete both markers, completely join their neighbourhoods) until one
// bag is left, then compare it with g.
bool recomposes_to(const Graph& g, const SplitDecompositionResult& sd) {
    struct Bag {
        std::vector<int> label;              // 1..k, 0 = marker
        std::vector<std::vector<char>> adj;  // (k+1) x (k+1)
        bool alive = true;
    };
    std::vector<Bag> bags(sd.bags.size());
    for (size_t i = 0; i < sd.bags.size(); ++i) {
        int k = sd.bags[i].skeleton.n;
        bags[i].label = sd.bags[i].label;
        bags[i].adj.assign(k + 1, std::vector<char>(k + 1, 0));
        for (int u = 1; u <= k; ++u) {
            for (size_t t = 0; t < sd.bags[i].skeleton.adj[u].size(); ++t) {
                bags[i].adj[u][sd.bags[i].skeleton.adj[u][t]] = 1;
            }
        }
    }

    std::vector<SplitTreeEdge> edges = sd.tree_edges;
    std::vector<bool> edge_alive(edges.size(), true);
    for (size_t step = 0; step < edges.size(); ++step) {
        // Find any live edge.
        int pick = -1;
        for (size_t e = 0; e < edges.size(); ++e) {
            if (edge_alive[e]) { pick = (int)e; break; }
        }
        if (pick < 0) break;
        int u = edges[pick].bag_u, v = edges[pick].bag_v;
        int mu = edges[pick].marker_u, mv = edges[pick].marker_v;
        if (u == v) return false;
        if (!bags[u].alive || !bags[v].alive) return false;
        if (bags[u].label[mu] != 0 || bags[v].label[mv] != 0) return false;

        int ku = (int)bags[u].label.size() - 1;
        int kv = (int)bags[v].label.size() - 1;
        std::vector<int> lu(ku + 1, 0), lv(kv + 1, 0);
        int count = 0;
        for (int i = 1; i <= ku; ++i) if (i != mu) lu[i] = ++count;
        for (int i = 1; i <= kv; ++i) if (i != mv) lv[i] = ++count;

        Bag out;
        out.label.assign(count + 1, 0);
        out.adj.assign(count + 1, std::vector<char>(count + 1, 0));
        for (int i = 1; i <= ku; ++i) {
            if (i == mu) continue;
            out.label[lu[i]] = bags[u].label[i];
            for (int j = 1; j <= ku; ++j) {
                if (j != mu && bags[u].adj[i][j]) out.adj[lu[i]][lu[j]] = 1;
            }
        }
        for (int i = 1; i <= kv; ++i) {
            if (i == mv) continue;
            out.label[lv[i]] = bags[v].label[i];
            for (int j = 1; j <= kv; ++j) {
                if (j != mv && bags[v].adj[i][j]) out.adj[lv[i]][lv[j]] = 1;
            }
        }
        for (int i = 1; i <= ku; ++i) {
            if (i == mu || !bags[u].adj[mu][i]) continue;
            for (int j = 1; j <= kv; ++j) {
                if (j == mv || !bags[v].adj[mv][j]) continue;
                out.adj[lu[i]][lv[j]] = 1;
                out.adj[lv[j]][lu[i]] = 1;
            }
        }

        // Rewire the remaining edges that pointed at u or v.
        for (size_t e = 0; e < edges.size(); ++e) {
            if (!edge_alive[e] || (int)e == pick) continue;
            if (edges[e].bag_u == u) edges[e].marker_u = lu[edges[e].marker_u];
            else if (edges[e].bag_u == v) { edges[e].bag_u = u; edges[e].marker_u = lv[edges[e].marker_u]; }
            if (edges[e].bag_v == u) edges[e].marker_v = lu[edges[e].marker_v];
            else if (edges[e].bag_v == v) { edges[e].bag_v = u; edges[e].marker_v = lv[edges[e].marker_v]; }
        }
        bags[u].label.swap(out.label);
        bags[u].adj.swap(out.adj);
        bags[v].alive = false;
        edge_alive[pick] = false;
    }

    int live = -1;
    for (size_t i = 0; i < bags.size(); ++i) {
        if (!bags[i].alive) continue;
        if (live >= 0) return false;  // the tree was disconnected
        live = (int)i;
    }
    if (live < 0) return g.n == 0;

    int k = (int)bags[live].label.size() - 1;
    if (k != g.n) return false;
    for (int i = 1; i <= k; ++i) {
        if (bags[live].label[i] < 1 || bags[live].label[i] > g.n) return false;
    }
    for (int i = 1; i <= k; ++i) {
        for (int j = i + 1; j <= k; ++j) {
            bool a = bags[live].adj[i][j] != 0;
            if (a != g.has_edge(bags[live].label[i], bags[live].label[j])) return false;
        }
    }
    return true;
}

// Structural invariants: labels partition the vertices, markers are matched
// one-to-one by tree edges, the tree is a tree, no bag has a split left, and
// no mergeable degenerate pair remains.
bool tree_is_canonical(const Graph& g, const SplitDecompositionResult& sd) {
    size_t b = sd.bags.size();
    if (b == 0) return g.n == 0;

    std::vector<int> seen(g.n + 1, 0);
    size_t marker_count = 0;
    for (size_t i = 0; i < b; ++i) {
        int k = sd.bags[i].skeleton.n;
        if ((int)sd.bags[i].label.size() != k + 1) return false;
        if (k < 1) return false;
        for (int x = 1; x <= k; ++x) {
            int lab = sd.bags[i].label[x];
            if (lab == 0) { ++marker_count; continue; }
            if (lab < 1 || lab > g.n || seen[lab]) return false;
            seen[lab] = 1;
        }
        // A bag must be degenerate or have no split. Degenerate bags do have
        // splits -- K5 and K_{1,4} both do -- which is exactly why the
        // decomposition stops at them instead of splitting and merging back.
        const Graph& sk = sd.bags[i].skeleton;
        if (sd.bags[i].kind == SplitNodeKind::PRIME) {
            if (find_split(sk).found) return false;
        } else if (sd.bags[i].kind == SplitNodeKind::CLIQUE) {
            for (int x = 1; x <= k; ++x) {
                for (int y = x + 1; y <= k; ++y) {
                    if (!sk.has_edge(x, y)) return false;
                }
            }
        } else {
            int c = sd.bags[i].center;
            if (c < 1 || c > k) return false;
            for (int x = 1; x <= k; ++x) {
                for (int y = x + 1; y <= k; ++y) {
                    bool expect = (x == c || y == c);
                    if (sk.has_edge(x, y) != expect) return false;
                }
            }
        }
    }
    for (int v = 1; v <= g.n; ++v) if (!seen[v]) return false;
    if (marker_count != 2 * sd.tree_edges.size()) return false;

    // The tree edges pair up distinct markers and form a tree.
    std::vector<std::vector<int>> tree(b);
    std::vector<std::vector<int>> used(b);
    for (size_t i = 0; i < b; ++i) used[i].assign(sd.bags[i].skeleton.n + 1, 0);
    for (size_t e = 0; e < sd.tree_edges.size(); ++e) {
        const SplitTreeEdge& te = sd.tree_edges[e];
        if (te.bag_u < 0 || te.bag_u >= (int)b || te.bag_v < 0 || te.bag_v >= (int)b) return false;
        if (te.bag_u == te.bag_v) return false;
        if (sd.bags[te.bag_u].label[te.marker_u] != 0) return false;
        if (sd.bags[te.bag_v].label[te.marker_v] != 0) return false;
        if (used[te.bag_u][te.marker_u] || used[te.bag_v][te.marker_v]) return false;
        used[te.bag_u][te.marker_u] = 1;
        used[te.bag_v][te.marker_v] = 1;
        tree[te.bag_u].push_back(te.bag_v);
        tree[te.bag_v].push_back(te.bag_u);
    }
    if (sd.tree_edges.size() != b - 1) return false;
    {
        std::vector<bool> vis(b, false);
        std::vector<int> stack(1, 0);
        vis[0] = true;
        size_t count = 1;
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (size_t j = 0; j < tree[x].size(); ++j) {
                int y = tree[x][j];
                if (vis[y]) continue;
                vis[y] = true;
                ++count;
                stack.push_back(y);
            }
        }
        if (count != b) return false;
    }

    // Canonicity: no clique-clique and no centre-to-extremity star-star edge.
    for (size_t e = 0; e < sd.tree_edges.size(); ++e) {
        const SplitTreeEdge& te = sd.tree_edges[e];
        const SplitBag& u = sd.bags[te.bag_u];
        const SplitBag& v = sd.bags[te.bag_v];
        if (u.kind == SplitNodeKind::CLIQUE && v.kind == SplitNodeKind::CLIQUE) return false;
        if (u.kind == SplitNodeKind::STAR && v.kind == SplitNodeKind::STAR) {
            bool u_center = te.marker_u == u.center;
            bool v_center = te.marker_v == v.center;
            if (u_center != v_center) return false;
        }
    }
    return true;
}

}  // namespace

TEST(SplitDecompositionTest, EmptyGraph) {
    Graph g;
    SplitDecompositionResult sd = split_decomposition(g);
    EXPECT_TRUE(sd.success);
    EXPECT_TRUE(sd.totally_decomposable);
    EXPECT_TRUE(sd.bags.empty());
}

TEST(SplitDecompositionTest, DisconnectedInputIsRejected) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    EXPECT_FALSE(split_decomposition(g).success);
    // The per-component wrapper still answers.
    EXPECT_TRUE(is_totally_decomposable(g));
}

TEST(SplitDecompositionTest, CompleteGraphIsOneCliqueBag) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= 5; ++u)
        for (int v = u + 1; v <= 5; ++v) edges.push_back(std::make_pair(u, v));
    Graph g(5, edges);
    SplitDecompositionResult sd = split_decomposition(g);
    ASSERT_TRUE(sd.success);
    ASSERT_EQ(sd.bags.size(), 1u);
    EXPECT_EQ(sd.bags[0].kind, SplitNodeKind::CLIQUE);
    EXPECT_TRUE(sd.totally_decomposable);
    EXPECT_TRUE(tree_is_canonical(g, sd));
    EXPECT_TRUE(recomposes_to(g, sd));
}

TEST(SplitDecompositionTest, StarIsOneStarBag) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 2; v <= 5; ++v) edges.push_back(std::make_pair(1, v));
    Graph g(5, edges);
    SplitDecompositionResult sd = split_decomposition(g);
    ASSERT_TRUE(sd.success);
    ASSERT_EQ(sd.bags.size(), 1u);
    EXPECT_EQ(sd.bags[0].kind, SplitNodeKind::STAR);
    EXPECT_TRUE(sd.totally_decomposable);
    EXPECT_TRUE(recomposes_to(g, sd));
}

TEST(SplitDecompositionTest, FiveCycleIsASinglePrimeBag) {
    Graph g = make_cycle(5);
    SplitDecompositionResult sd = split_decomposition(g);
    ASSERT_TRUE(sd.success);
    ASSERT_EQ(sd.bags.size(), 1u);
    EXPECT_EQ(sd.bags[0].kind, SplitNodeKind::PRIME);
    EXPECT_FALSE(sd.totally_decomposable);
    EXPECT_TRUE(recomposes_to(g, sd));
}

TEST(SplitDecompositionTest, PathDecomposesIntoStars) {
    Graph g = make_path(5);
    SplitDecompositionResult sd = split_decomposition(g);
    ASSERT_TRUE(sd.success);
    EXPECT_TRUE(sd.totally_decomposable);
    EXPECT_TRUE(tree_is_canonical(g, sd));
    EXPECT_TRUE(recomposes_to(g, sd));
}

// Every connected graph on up to 6 vertices: the tree must recompose to the
// graph, satisfy its structural invariants and be canonical, and it must have
// no prime bag exactly when the graph is distance-hereditary.
TEST(SplitDecompositionTest, ExhaustiveRecompositionAndDistanceHereditary) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);
            if (connected_components(g).count != 1) continue;

            SplitDecompositionResult sd = split_decomposition(g);
            ASSERT_TRUE(sd.success) << "n=" << n << " mask=" << mask;
            ASSERT_TRUE(tree_is_canonical(g, sd)) << "n=" << n << " mask=" << mask;
            ASSERT_TRUE(recomposes_to(g, sd)) << "n=" << n << " mask=" << mask;
            ASSERT_EQ(sd.totally_decomposable,
                      check_distance_hereditary(g).is_distance_hereditary)
                << "n=" << n << " mask=" << mask;
        }
    }
}
