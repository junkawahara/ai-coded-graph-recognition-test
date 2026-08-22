#include "interval.h"
#include "interval_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::EnumeratedGraph;
using graph_recognition::Graph;
using graph_recognition::IntervalEnumAlgorithm;
using graph_recognition::IntervalEnumerationResult;
using graph_recognition::check_interval;
using graph_recognition::enumerate_interval_graphs_reverse_search;
using graph_recognition::enumerate_interval_graphs_reverse_search_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "interval_enum";

class IntervalEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(IntervalEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    IntervalEnumerationResult res = enumerate_interval_graphs_reverse_search(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int>>> seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        const EnumeratedGraph& eg = res.graphs[i];
        std::vector<std::pair<int, int>> key = eg.edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        Graph g(eg.n, eg.edges);
        EXPECT_TRUE(check_interval(g).is_interval) << "non-interval enumerated in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, IntervalEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(IntervalEnumAlgorithms, DedicatedSearchIsTheDefault) {
    IntervalEnumerationResult result = enumerate_interval_graphs_reverse_search(4);
    ASSERT_FALSE(result.graphs.empty());
    EXPECT_EQ(result.graphs.front().edges.size(), 6u);
}

TEST(IntervalEnumAlgorithms, DedicatedAndLegacySearchAgree) {
    for (int n = 0; n <= 6; ++n) {
        IntervalEnumerationResult dedicated =
            enumerate_interval_graphs_reverse_search(
                n, IntervalEnumAlgorithm::KIYOMI_KIJIMA_UNO);
        IntervalEnumerationResult legacy =
            enumerate_interval_graphs_reverse_search(
                n, IntervalEnumAlgorithm::LEGACY_CHORDAL_FILTER);

        std::set<std::vector<std::pair<int, int>>> dedicated_set;
        std::set<std::vector<std::pair<int, int>>> legacy_set;
        for (size_t i = 0; i < dedicated.graphs.size(); ++i) {
            dedicated_set.insert(dedicated.graphs[i].edges);
        }
        for (size_t i = 0; i < legacy.graphs.size(); ++i) {
            legacy_set.insert(legacy.graphs[i].edges);
        }
        EXPECT_EQ(dedicated_set, legacy_set) << "n=" << n;
    }
}

TEST(IntervalEnumAlgorithms, EveryNonRootGraphHasAnIntervalParent) {
    for (int n = 1; n <= 6; ++n) {
        IntervalEnumerationResult result =
            enumerate_interval_graphs_reverse_search(n);
        const size_t complete_edges = static_cast<size_t>(n * (n - 1) / 2);
        for (size_t i = 0; i < result.graphs.size(); ++i) {
            const EnumeratedGraph& graph = result.graphs[i];
            if (graph.edges.size() == complete_edges) continue;

            graph_recognition::detail::KiyomiKijimaUnoIntervalState state(n);
            for (int u = 1; u <= n; ++u) {
                for (int v = u + 1; v <= n; ++v) {
                    if (std::find(graph.edges.begin(), graph.edges.end(),
                                  std::make_pair(u, v)) == graph.edges.end()) {
                        graph_recognition::detail::interval_remove_edge(
                            &state, u, v);
                    }
                }
            }

            Graph g(n, graph.edges);
            graph_recognition::IntervalResult model = check_interval(g);
            std::pair<int, int> edge =
                graph_recognition::detail::interval_parent_edge_from_model(
                    state, model);
            ASSERT_NE(edge, std::make_pair(0, 0)) << "n=" << n;
            ASSERT_FALSE(g.has_edge(edge.first, edge.second)) << "n=" << n;

            std::vector<std::pair<int, int>> parent_edges = graph.edges;
            parent_edges.push_back(edge);
            EXPECT_TRUE(check_interval(Graph(n, parent_edges)).is_interval)
                << "n=" << n;
        }
    }
}

TEST(IntervalEnumAlgorithms, StreamingAndMaterializedApisAgree) {
    const int n = 5;
    IntervalEnumerationResult materialized =
        enumerate_interval_graphs_reverse_search(n);
    std::vector<EnumeratedGraph> streamed;
    enumerate_interval_graphs_reverse_search_cb(
        n, [&streamed](const EnumeratedGraph& graph) {
            streamed.push_back(graph);
        });
    ASSERT_EQ(streamed.size(), materialized.graphs.size());
    for (size_t i = 0; i < streamed.size(); ++i) {
        EXPECT_EQ(streamed[i].n, materialized.graphs[i].n);
        EXPECT_EQ(streamed[i].edges, materialized.graphs[i].edges);
    }
}

}  // namespace
