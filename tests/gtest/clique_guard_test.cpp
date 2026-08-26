#include "decompositions/clique.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

Graph make_c4() {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    edges.push_back(std::make_pair(4, 1));
    return Graph(4, edges);
}

// A violated is_chordal precondition must produce a safely indexable result
// (member sized n + 1), not an out-of-bounds read in the tree builders.
TEST(CliqueGuardTest, NonChordalInputYieldsIndexableEmptyResult) {
    Graph g = make_c4();
    ChordalResult ch = check_chordal(g);
    ASSERT_FALSE(ch.is_chordal);

    MaximalCliques mc = enumerate_maximal_cliques(g, ch);
    EXPECT_TRUE(mc.cliques.empty());
    ASSERT_EQ(mc.member.size(), (size_t)g.n + 1);
    for (int v = 1; v <= g.n; ++v) EXPECT_TRUE(mc.member[v].empty());
}

TEST(CliqueGuardTest, DetailBuildersGuardNonChordalInput) {
    Graph g = make_c4();
    ChordalResult ch = check_chordal(g);
    ASSERT_FALSE(ch.is_chordal);

    CliqueTreeResult kr = detail::build_clique_tree_kruskal(g, ch);
    EXPECT_TRUE(kr.mc.cliques.empty());
    EXPECT_TRUE(kr.tree.empty());
    ASSERT_EQ(kr.mc.member.size(), (size_t)g.n + 1);

    CliqueTreeResult inc = detail::build_clique_tree_incremental(g, ch);
    EXPECT_TRUE(inc.mc.cliques.empty());
    EXPECT_TRUE(inc.tree.empty());
    ASSERT_EQ(inc.mc.member.size(), (size_t)g.n + 1);

    CliqueTreeResult wrap = build_clique_tree(g, ch);
    EXPECT_TRUE(wrap.mc.cliques.empty());
    ASSERT_EQ(wrap.mc.member.size(), (size_t)g.n + 1);
}

TEST(CliqueGuardTest, ChordalPathStillBuildsTree) {
    // P4 is chordal with 3 maximal cliques (its edges); both builders must
    // produce a spanning tree over them.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    ChordalResult ch = check_chordal(g);
    ASSERT_TRUE(ch.is_chordal);

    CliqueTreeResult kr = build_clique_tree(g, ch, CliqueTreeAlgorithm::KRUSKAL);
    EXPECT_EQ(kr.mc.cliques.size(), 3u);
    size_t kr_edges = 0;
    for (size_t i = 0; i < kr.tree.size(); ++i) kr_edges += kr.tree[i].size();
    EXPECT_EQ(kr_edges, 4u);  // 2 tree edges, stored in both directions

    CliqueTreeResult inc = build_clique_tree(g, ch, CliqueTreeAlgorithm::INCREMENTAL);
    EXPECT_EQ(inc.mc.cliques.size(), 3u);
    size_t inc_edges = 0;
    for (size_t i = 0; i < inc.tree.size(); ++i) inc_edges += inc.tree[i].size();
    EXPECT_EQ(inc_edges, 4u);
}

}  // namespace
