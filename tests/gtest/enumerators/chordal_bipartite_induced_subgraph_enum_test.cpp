#include "recognizers/chordal_bipartite.h"
#include "enumerators/chordal_bipartite_induced_subgraph_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::ChordalBipartiteInducedSubgraphEnumerationResult;
using graph_recognition::Graph;
using graph_recognition::check_chordal_bipartite;
using graph_recognition::enumerate_chordal_bipartite_induced_subgraphs;
using graph_recognition::enumerate_chordal_bipartite_induced_subgraphs_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "chordal_bipartite_induced_subgraph_enum";

typedef std::vector<std::pair<int, int> > EdgeList;
typedef std::vector<int> VertexSet;

/** The subgraph induced by vs, relabeled to 1..|vs| so the recognizer sees a
 *  graph without isolated leftovers of the host. */
Graph induced(const Graph& g, const VertexSet& vs) {
    std::vector<int> label(g.n + 1, 0);
    for (std::size_t i = 0; i < vs.size(); ++i) {
        label[vs[i]] = static_cast<int>(i) + 1;
    }
    EdgeList edges;
    for (std::size_t i = 0; i < vs.size(); ++i) {
        for (std::size_t j = i + 1; j < vs.size(); ++j) {
            if (g.has_edge(vs[i], vs[j])) {
                edges.push_back(std::make_pair(label[vs[i]], label[vs[j]]));
            }
        }
    }
    return Graph(static_cast<int>(vs.size()), edges);
}

bool is_solution(const Graph& g, const VertexSet& vs) {
    return check_chordal_bipartite(induced(g, vs)).is_chordal_bipartite;
}

/** Every chordal bipartite vertex subset of the host, found by testing all
 *  2^n subsets. */
std::set<VertexSet> brute_force_solutions(const Graph& g) {
    std::set<VertexSet> out;
    for (unsigned long mask = 0; mask < (1UL << g.n); ++mask) {
        VertexSet vs;
        for (int i = 0; i < g.n; ++i) {
            if (mask & (1UL << i)) vs.push_back(i + 1);
        }
        if (is_solution(g, vs)) out.insert(vs);
    }
    return out;
}

/** The enumerated vertex sets, checked for duplicates as they are collected. */
std::set<VertexSet> collect_unique(
    const ChordalBipartiteInducedSubgraphEnumerationResult& res,
    const std::string& label) {
    std::set<VertexSet> out;
    for (std::size_t i = 0; i < res.vertex_sets.size(); ++i) {
        EXPECT_TRUE(out.insert(res.vertex_sets[i]).second)
            << "duplicate vertex set in " << label;
    }
    return out;
}

class ChordalBipartiteInducedSubgraphEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalBipartiteInducedSubgraphEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    const Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    const int expected =
        std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    const ChordalBipartiteInducedSubgraphEnumerationResult res =
        enumerate_chordal_bipartite_induced_subgraphs(g);
    EXPECT_EQ(static_cast<int>(res.vertex_sets.size()), expected) << "case=" << stem;

    ASSERT_FALSE(res.vertex_sets.empty()) << "case=" << stem;
    EXPECT_TRUE(res.vertex_sets[0].empty())
        << "the empty set must be emitted first in case=" << stem;

    std::set<VertexSet> seen;
    for (std::size_t i = 0; i < res.vertex_sets.size(); ++i) {
        const VertexSet& vs = res.vertex_sets[i];
        EXPECT_TRUE(seen.insert(vs).second) << "duplicate vertex set in case=" << stem;

        EXPECT_TRUE(std::is_sorted(vs.begin(), vs.end()))
            << "unsorted vertex set in case=" << stem;
        for (std::size_t j = 0; j < vs.size(); ++j) {
            EXPECT_GE(vs[j], 1) << "case=" << stem;
            EXPECT_LE(vs[j], g.n) << "case=" << stem;
        }
        EXPECT_TRUE(is_solution(g, vs))
            << "non-chordal-bipartite induced subgraph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalBipartiteInducedSubgraphEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The set of enumerated vertex sets must be exactly the chordal bipartite
// vertex subsets of the host. Brute force is 2^n recognizer calls, so this
// covers only the small-n cases.
TEST(ChordalBipartiteInducedSubgraphEnumBruteForceTest,
     MatchesAllChordalBipartiteVertexSubsets) {
    const std::vector<std::string> stems = list_in_files(test_path(kDir));
    for (std::size_t i = 0; i < stems.size(); ++i) {
        const Graph g =
            load_graph(test_path(std::string(kDir) + "/" + stems[i] + ".in"));
        if (g.n > 12) continue;

        const ChordalBipartiteInducedSubgraphEnumerationResult res =
            enumerate_chordal_bipartite_induced_subgraphs(g);
        EXPECT_EQ(collect_unique(res, "case=" + stems[i]), brute_force_solutions(g))
            << "case=" << stems[i];
    }
}

TEST(ChordalBipartiteInducedSubgraphEnumBruteForceTest, MatchesOnRandomHosts) {
    unsigned int seed = 20260826u;
    for (int trial = 0; trial < 60; ++trial) {
        seed = seed * 1103515245u + 12345u;
        const int n = 2 + static_cast<int>((seed >> 16) % 6u);

        EdgeList edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                seed = seed * 1103515245u + 12345u;
                if ((seed >> 16) & 1u) edges.push_back(std::make_pair(u, v));
            }
        }
        const Graph g(n, edges);

        const ChordalBipartiteInducedSubgraphEnumerationResult res =
            enumerate_chordal_bipartite_induced_subgraphs(g);
        EXPECT_EQ(collect_unique(res, "trial=" + std::to_string(trial)),
                  brute_force_solutions(g))
            << "trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

TEST(ChordalBipartiteInducedSubgraphEnumCallbackTest,
     StreamsSameSetsAsMaterializingApi) {
    const std::vector<std::string> stems = list_in_files(test_path(kDir));
    for (std::size_t i = 0; i < stems.size(); ++i) {
        const Graph g =
            load_graph(test_path(std::string(kDir) + "/" + stems[i] + ".in"));

        const ChordalBipartiteInducedSubgraphEnumerationResult res =
            enumerate_chordal_bipartite_induced_subgraphs(g);
        std::vector<VertexSet> streamed;
        enumerate_chordal_bipartite_induced_subgraphs_cb(
            g, [&streamed](const VertexSet& vs) { streamed.push_back(vs); });

        ASSERT_EQ(streamed.size(), res.vertex_sets.size()) << "case=" << stems[i];
        for (std::size_t j = 0; j < streamed.size(); ++j) {
            EXPECT_EQ(streamed[j], res.vertex_sets[j])
                << "case=" << stems[i] << " #" << j;
        }
    }
}

// The empty set is chordal bipartite and must be emitted exactly once, even
// for degenerate vertex counts where the search tree has no other node.
TEST(ChordalBipartiteInducedSubgraphEnumTest, DegenerateHosts) {
    const ChordalBipartiteInducedSubgraphEnumerationResult none =
        enumerate_chordal_bipartite_induced_subgraphs(Graph(0, EdgeList()));
    ASSERT_EQ(none.vertex_sets.size(), 1u);
    EXPECT_TRUE(none.vertex_sets[0].empty());

    const ChordalBipartiteInducedSubgraphEnumerationResult one =
        enumerate_chordal_bipartite_induced_subgraphs(Graph(1, EdgeList()));
    ASSERT_EQ(one.vertex_sets.size(), 2u);
    EXPECT_TRUE(one.vertex_sets[0].empty());
    EXPECT_EQ(one.vertex_sets[1], VertexSet(1, 1));
}

// The class is hereditary, so a chordal bipartite host has all 2^n vertex
// subsets as solutions -- the worst case for the output size.
TEST(ChordalBipartiteInducedSubgraphEnumTest, ChordalBipartiteHostYieldsAllSubsets) {
    EdgeList c4;
    c4.push_back(std::make_pair(1, 2));
    c4.push_back(std::make_pair(2, 3));
    c4.push_back(std::make_pair(3, 4));
    c4.push_back(std::make_pair(4, 1));
    EXPECT_EQ(enumerate_chordal_bipartite_induced_subgraphs(Graph(4, c4))
                  .vertex_sets.size(),
              1u << 4);

    EdgeList path;
    for (int v = 1; v <= 9; ++v) path.push_back(std::make_pair(v, v + 1));
    EXPECT_EQ(enumerate_chordal_bipartite_induced_subgraphs(Graph(10, path))
                  .vertex_sets.size(),
              1u << 10);
}

}  // namespace
