#include "recognizers/cage.h"
#include "enumerators/cage_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::CageResult;
using graph_recognition::CageUnlabeledEnumerationResult;
using graph_recognition::Graph;
using graph_recognition::check_cage;
using graph_recognition::enumerate_cage_unlabeled_graphs;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "cage";

// Reads the class's parameterized input format: `k g` on the first
// line, then the standard `n m` + edge lines (load_graph cannot be
// reused).
Graph load_graph_with_kg(const std::string& path, int& k, int& g) {
    std::ifstream fin(path.c_str());
    int n = 0, m = 0;
    fin >> k >> g >> n >> m;
    std::vector<std::pair<int, int> > edges;
    for (int e = 0; e < m; ++e) {
        int u, v;
        fin >> u >> v;
        edges.push_back(std::make_pair(u, v));
    }
    return Graph(n, edges);
}

// Girth straight from a second definition, independent of the
// recognizer's per-vertex truncated BFS: the shortest cycle through an
// edge (u, v) is 1 + the shortest u-v path avoiding that edge. Returns
// 0 when acyclic.
int brute_girth(const Graph& g) {
    const int n = g.n;
    int girth = n + 1;
    for (int s = 1; s <= n; ++s) {
        for (size_t j = 0; j < g.adj[s].size(); ++j) {
            const int t = g.adj[s][j];
            if (t < s) continue;
            std::vector<int> dist(n + 1, -1);
            std::vector<int> queue;
            queue.push_back(s);
            dist[s] = 0;
            for (size_t head = 0; head < queue.size(); ++head) {
                const int v = queue[head];
                for (size_t i = 0; i < g.adj[v].size(); ++i) {
                    const int u = g.adj[v][i];
                    if (v == s && u == t) continue;
                    if (dist[u] == -1) {
                        dist[u] = dist[v] + 1;
                        queue.push_back(u);
                    }
                }
            }
            if (dist[t] != -1 && dist[t] + 1 < girth) girth = dist[t] + 1;
        }
    }
    return girth > n ? 0 : girth;
}

class CageTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CageTest, MatchesExpected) {
    const std::string stem = GetParam();
    int k = 0, g = 0;
    Graph graph = load_graph_with_kg(
        test_path(std::string(kDir) + "/" + stem + ".in"), k, g);
    std::string exp =
        read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    CageResult r = check_cage(graph, k, g);
    ASSERT_EQ(r.is_cage, exp == "YES") << "case=" << stem;
    EXPECT_EQ(r.girth, brute_girth(graph)) << "case=" << stem;
    if (r.is_cage) {
        // A cage is in particular a (k,g)-graph of girth exactly g.
        EXPECT_TRUE(r.is_kg_graph) << "case=" << stem;
        EXPECT_EQ(r.girth, g) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CageTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Differential test on every graph on up to 5 vertices: the reported
// girth must match the per-edge-BFS definition, and is_kg_graph must
// match the definition (k-regular, girth >= g with acyclic = infinite)
// for every k and g.
TEST(CageBruteForceTest, GirthAndMembershipMatchDefinition) {
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int> > pairs;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                pairs.push_back(std::make_pair(u, v));
            }
        }
        for (int mask = 0; mask < (1 << (int)pairs.size()); ++mask) {
            std::vector<std::pair<int, int> > edges;
            for (size_t e = 0; e < pairs.size(); ++e) {
                if (mask & (1 << (int)e)) edges.push_back(pairs[e]);
            }
            Graph g(n, edges);
            const int girth = brute_girth(g);
            for (int k = 0; k <= 4; ++k) {
                bool regular = true;
                for (int v = 1; v <= n; ++v) {
                    if ((int)g.adj[v].size() != k) regular = false;
                }
                for (int gp = 3; gp <= 6; ++gp) {
                    CageResult r = check_cage(g, k, gp);
                    EXPECT_EQ(r.girth, girth)
                        << "n=" << n << " mask=" << mask;
                    EXPECT_EQ(r.is_kg_graph,
                              regular && (girth == 0 || girth >= gp))
                        << "n=" << n << " mask=" << mask << " k=" << k
                        << " g=" << gp;
                }
            }
        }
    }
}

// Cage-ness against the enumerator: for each (k,g), the cages are
// exactly the members of the first nonempty enumeration level.
TEST(CageMinimalityTest, MatchesFirstNonemptyLevel) {
    const int params[][2] = {{2, 3}, {2, 4}, {2, 5}, {3, 3}, {3, 4}, {3, 5}};
    for (size_t p = 0; p < sizeof(params) / sizeof(params[0]); ++p) {
        const int k = params[p][0], g = params[p][1];
        int cage_order = -1;
        const int scan_to = (k == 3 && g == 5) ? 12 : 10;
        for (int n = 3; n <= scan_to; ++n) {
            CageUnlabeledEnumerationResult res =
                enumerate_cage_unlabeled_graphs(n, k, g);
            for (size_t i = 0; i < res.graphs.size(); ++i) {
                Graph graph(res.graphs[i].n, res.graphs[i].edges);
                const bool expect_cage =
                    cage_order == -1 && brute_girth(graph) == g;
                EXPECT_EQ(check_cage(graph, k, g).is_cage, expect_cage)
                    << "k=" << k << " g=" << g << " n=" << n << " i=" << i;
            }
            if (cage_order == -1 && !res.graphs.empty()) cage_order = n;
        }
        EXPECT_NE(cage_order, -1) << "k=" << k << " g=" << g;
    }
}

// Boundary cases: the empty graph, matchings and long cycles.
TEST(CageEdgeCaseTest, DegenerateParameters) {
    Graph empty(0, std::vector<std::pair<int, int> >());
    EXPECT_TRUE(check_cage(empty, 0, 5).is_kg_graph);
    EXPECT_FALSE(check_cage(empty, 0, 5).is_cage);
    EXPECT_FALSE(check_cage(empty, 1, 5).is_kg_graph);
    EXPECT_FALSE(check_cage(empty, -1, 5).is_kg_graph);

    std::vector<std::pair<int, int> > one_edge(1, std::make_pair(1, 2));
    Graph k2(2, one_edge);
    CageResult rk2 = check_cage(k2, 1, 5);
    EXPECT_TRUE(rk2.is_kg_graph);  /* acyclic: girth infinite */
    EXPECT_FALSE(rk2.is_cage);     /* girth is not exactly 5 */
    EXPECT_EQ(rk2.girth, 0);

    // C7 is the (2,7)-cage; with g = 6 it is a (2,6)-graph but its
    // girth is not exactly 6.
    std::vector<std::pair<int, int> > cyc;
    for (int v = 1; v <= 7; ++v) {
        cyc.push_back(std::make_pair(v, v % 7 + 1));
    }
    Graph c7(7, cyc);
    EXPECT_TRUE(check_cage(c7, 2, 7).is_cage);
    CageResult r6 = check_cage(c7, 2, 6);
    EXPECT_TRUE(r6.is_kg_graph);
    EXPECT_FALSE(r6.is_cage);
    EXPECT_EQ(r6.girth, 7);
}

}  // namespace
