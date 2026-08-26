#include "recognizers/chordal.h"
#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/chordal_subgraph_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::ChordalSubgraphEnumerationResult;
using graph_recognition::EnumeratedGraph;
using graph_recognition::Graph;
using graph_recognition::check_chordal;
using graph_recognition::enumerate_chordal_labeled_graphs_reverse_search;
using graph_recognition::enumerate_chordal_subgraphs;
using graph_recognition::enumerate_chordal_subgraphs_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "chordal_subgraph_enum";

typedef std::vector<std::pair<int, int> > EdgeList;

EdgeList normalized(const EdgeList& edges) {
    EdgeList key = edges;
    for (std::size_t i = 0; i < key.size(); ++i) {
        if (key[i].first > key[i].second) {
            std::swap(key[i].first, key[i].second);
        }
    }
    std::sort(key.begin(), key.end());
    return key;
}

EdgeList host_edges(const Graph& g) {
    EdgeList edges;
    for (int u = 1; u <= g.n; ++u) {
        for (std::size_t i = 0; i < g.adj[u].size(); ++i) {
            const int v = g.adj[u][i];
            if (u < v) edges.push_back(std::make_pair(u, v));
        }
    }
    std::sort(edges.begin(), edges.end());
    return edges;
}

/** Every chordal edge subset of the host, found by testing all 2^m subsets. */
std::set<EdgeList> brute_force_chordal_subgraphs(const Graph& g) {
    const EdgeList edges = host_edges(g);
    const std::size_t m = edges.size();
    std::set<EdgeList> out;
    for (unsigned long mask = 0; mask < (1UL << m); ++mask) {
        EdgeList sub;
        for (std::size_t i = 0; i < m; ++i) {
            if (mask & (1UL << i)) sub.push_back(edges[i]);
        }
        if (check_chordal(Graph(g.n, sub)).is_chordal) out.insert(sub);
    }
    return out;
}

/** The enumerated subgraphs, checked for duplicates as they are collected. */
std::set<EdgeList> collect_unique(const ChordalSubgraphEnumerationResult& res,
                                  const std::string& label) {
    std::set<EdgeList> out;
    for (std::size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_TRUE(out.insert(normalized(res.graphs[i].edges)).second)
            << "duplicate subgraph in " << label;
    }
    return out;
}

class ChordalSubgraphEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalSubgraphEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    const Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    const int expected =
        std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    const ChordalSubgraphEnumerationResult res = enumerate_chordal_subgraphs(g);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<EdgeList> seen;
    for (std::size_t i = 0; i < res.graphs.size(); ++i) {
        const EnumeratedGraph& out = res.graphs[i];
        EXPECT_EQ(out.n, g.n) << "case=" << stem;

        const EdgeList key = normalized(out.edges);
        EXPECT_TRUE(seen.insert(key).second) << "duplicate subgraph in case=" << stem;
        for (std::size_t j = 0; j < key.size(); ++j) {
            EXPECT_TRUE(g.has_edge(key[j].first, key[j].second))
                << "edge outside the host in case=" << stem;
        }
        EXPECT_TRUE(check_chordal(Graph(out.n, out.edges)).is_chordal)
            << "non-chordal subgraph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalSubgraphEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The set of enumerated subgraphs must be exactly the chordal edge subsets of
// the host. Brute force is 2^m, so this covers only the small-m cases.
TEST(ChordalSubgraphEnumBruteForceTest, MatchesAllChordalEdgeSubsets) {
    const std::vector<std::string> stems = list_in_files(test_path(kDir));
    for (std::size_t i = 0; i < stems.size(); ++i) {
        const Graph g =
            load_graph(test_path(std::string(kDir) + "/" + stems[i] + ".in"));
        if (host_edges(g).size() > 15) continue;

        const ChordalSubgraphEnumerationResult res = enumerate_chordal_subgraphs(g);
        EXPECT_EQ(collect_unique(res, "case=" + stems[i]),
                  brute_force_chordal_subgraphs(g))
            << "case=" << stems[i];
    }
}

TEST(ChordalSubgraphEnumBruteForceTest, MatchesOnRandomHosts) {
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

        const ChordalSubgraphEnumerationResult res = enumerate_chordal_subgraphs(g);
        EXPECT_EQ(collect_unique(res, "trial=" + std::to_string(trial)),
                  brute_force_chordal_subgraphs(g))
            << "trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

// A complete host makes this the fixed-n labeled chordal enumeration, so it
// must agree with chordal_labeled_enum.h graph for graph.
TEST(ChordalSubgraphEnumTest, CompleteHostMatchesChordalLabeledEnum) {
    for (int n = 0; n <= 5; ++n) {
        EdgeList complete;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) complete.push_back(std::make_pair(u, v));
        }

        const ChordalSubgraphEnumerationResult sub =
            enumerate_chordal_subgraphs(Graph(n, complete));
        const graph_recognition::ChordalLabeledEnumerationResult full =
            enumerate_chordal_labeled_graphs_reverse_search(n);

        ASSERT_EQ(sub.graphs.size(), full.graphs.size()) << "n=" << n;
        std::set<EdgeList> sub_set;
        std::set<EdgeList> full_set;
        for (std::size_t i = 0; i < sub.graphs.size(); ++i) {
            sub_set.insert(normalized(sub.graphs[i].edges));
            full_set.insert(normalized(full.graphs[i].edges));
        }
        EXPECT_EQ(sub_set, full_set) << "n=" << n;
    }
}

TEST(ChordalSubgraphEnumCallbackTest, StreamsSameSubgraphsAsMaterializingApi) {
    const std::vector<std::string> stems = list_in_files(test_path(kDir));
    for (std::size_t i = 0; i < stems.size(); ++i) {
        const Graph g =
            load_graph(test_path(std::string(kDir) + "/" + stems[i] + ".in"));

        const ChordalSubgraphEnumerationResult res = enumerate_chordal_subgraphs(g);
        std::vector<EnumeratedGraph> streamed;
        enumerate_chordal_subgraphs_cb(
            g, [&streamed](const EnumeratedGraph& out) { streamed.push_back(out); });

        ASSERT_EQ(streamed.size(), res.graphs.size()) << "case=" << stems[i];
        for (std::size_t j = 0; j < streamed.size(); ++j) {
            EXPECT_EQ(streamed[j].n, res.graphs[j].n) << "case=" << stems[i] << " #" << j;
            EXPECT_EQ(streamed[j].edges, res.graphs[j].edges)
                << "case=" << stems[i] << " #" << j;
        }
    }
}

// The empty edge set is chordal and must be emitted exactly once, including
// for hosts with no edges and for degenerate vertex counts.
TEST(ChordalSubgraphEnumTest, EdgelessAndDegenerateHostsYieldOnlyTheEmptySubgraph) {
    const int vertex_counts[] = {0, 1, 2, 5};
    for (std::size_t i = 0; i < sizeof(vertex_counts) / sizeof(vertex_counts[0]); ++i) {
        const int n = vertex_counts[i];
        const ChordalSubgraphEnumerationResult res =
            enumerate_chordal_subgraphs(Graph(n, EdgeList()));
        ASSERT_EQ(res.graphs.size(), 1u) << "n=" << n;
        EXPECT_EQ(res.graphs[0].n, n) << "n=" << n;
        EXPECT_TRUE(res.graphs[0].edges.empty()) << "n=" << n;
    }
}

// Every subgraph of a forest is chordal, so a forest host yields all 2^m edge
// subsets -- the worst case for the output size.
TEST(ChordalSubgraphEnumTest, ForestHostYieldsEveryEdgeSubset) {
    EdgeList path;
    for (int v = 1; v <= 9; ++v) path.push_back(std::make_pair(v, v + 1));
    const ChordalSubgraphEnumerationResult res =
        enumerate_chordal_subgraphs(Graph(10, path));
    EXPECT_EQ(res.graphs.size(), 1u << path.size());
}

}  // namespace
