#include "proper_chordal.h"
#include "proper_chordal_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::ProperChordalEnumAlgorithm;
using graph_recognition::ProperChordalEnumerationResult;
using graph_recognition::check_proper_chordal;
using graph_recognition::enumerate_proper_chordal_graphs_reverse_search;
using graph_recognition::enumerate_proper_chordal_graphs_reverse_search_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "proper_chordal_enum";

class ProperChordalEnumTest : public ::testing::TestWithParam<std::string> {};

std::set<std::vector<std::pair<int, int>>> graph_set(
    const ProperChordalEnumerationResult& result) {
    std::set<std::vector<std::pair<int, int>>> graphs;
    for (std::size_t i = 0; i < result.graphs.size(); ++i) {
        graphs.insert(result.graphs[i].edges);
    }
    return graphs;
}

TEST_P(ProperChordalEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    ProperChordalEnumerationResult res = enumerate_proper_chordal_graphs_reverse_search(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int>>> seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int>> key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_proper_chordal(g).is_proper_chordal) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ProperChordalEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(ProperChordalEnumEdgeSearchTest, EnumeratesSameSetsAsLegacyFilter) {
    for (int n = 0; n <= 5; ++n) {
        const ProperChordalEnumerationResult dedicated =
            enumerate_proper_chordal_graphs_reverse_search(
                n, ProperChordalEnumAlgorithm::INDIFFERENCE_EDGE_ADDITION);
        const ProperChordalEnumerationResult legacy =
            enumerate_proper_chordal_graphs_reverse_search(
                n, ProperChordalEnumAlgorithm::LEGACY_CHORDAL_FILTER);

        const std::set<std::vector<std::pair<int, int>>> dedicated_set =
            graph_set(dedicated);
        const std::set<std::vector<std::pair<int, int>>> legacy_set =
            graph_set(legacy);
        EXPECT_EQ(dedicated_set.size(), dedicated.graphs.size())
            << "dedicated duplicate, n=" << n;
        EXPECT_EQ(legacy_set.size(), legacy.graphs.size())
            << "legacy duplicate, n=" << n;
        EXPECT_EQ(dedicated_set, legacy_set) << "n=" << n;
    }
}

TEST(ProperChordalEnumEdgeSearchTest,
     ExhaustiveSimpleGraphsAgreeThroughSixVertices) {
    for (int n = 0; n <= 6; ++n) {
        std::vector<std::pair<int, int>> possible_edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                possible_edges.push_back(std::make_pair(u, v));
            }
        }

        std::set<std::vector<std::pair<int, int>>> brute_force;
        const std::size_t graph_count =
            static_cast<std::size_t>(1) << possible_edges.size();
        for (std::size_t mask = 0; mask < graph_count; ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (std::size_t bit = 0; bit < possible_edges.size(); ++bit) {
                if ((mask >> bit) & static_cast<std::size_t>(1)) {
                    edges.push_back(possible_edges[bit]);
                }
            }
            if (check_proper_chordal(Graph(n, edges)).is_proper_chordal) {
                brute_force.insert(edges);
            }
        }

        const ProperChordalEnumerationResult dedicated =
            enumerate_proper_chordal_graphs_reverse_search(n);
        EXPECT_EQ(graph_set(dedicated), brute_force) << "n=" << n;
    }
}

TEST(ProperChordalEnumEdgeSearchTest, StreamsSameGraphsAsMaterializingApi) {
    for (int n = 0; n <= 5; ++n) {
        const ProperChordalEnumerationResult materialized =
            enumerate_proper_chordal_graphs_reverse_search(n);
        std::vector<graph_recognition::EnumeratedGraph> streamed;
        enumerate_proper_chordal_graphs_reverse_search_cb(
            n,
            [&streamed](const graph_recognition::EnumeratedGraph& graph) {
                streamed.push_back(graph);
            });

        ASSERT_EQ(streamed.size(), materialized.graphs.size()) << "n=" << n;
        for (std::size_t i = 0; i < streamed.size(); ++i) {
            EXPECT_EQ(streamed[i].n, materialized.graphs[i].n)
                << "n=" << n << " #" << i;
            EXPECT_EQ(streamed[i].edges, materialized.graphs[i].edges)
                << "n=" << n << " #" << i;
        }
    }
}

TEST(ProperChordalEnumEdgeSearchTest, CanonicalParentDeletionPreservesClass) {
    for (int n = 1; n <= 6; ++n) {
        const ProperChordalEnumerationResult result =
            enumerate_proper_chordal_graphs_reverse_search(n);
        for (std::size_t i = 0; i < result.graphs.size(); ++i) {
            if (result.graphs[i].edges.empty()) continue;

            graph_recognition::detail::ProperChordalEdgeState state(n);
            for (std::size_t e = 0; e < result.graphs[i].edges.size(); ++e) {
                graph_recognition::detail::proper_chordal_add_edge(
                    &state,
                    result.graphs[i].edges[e].first,
                    result.graphs[i].edges[e].second);
            }
            std::vector<int> tree_parent;
            ASSERT_TRUE(
                graph_recognition::detail::find_proper_chordal_edge_state_layout(
                    state, &tree_parent))
                << "n=" << n << " graph#" << i;
            const std::pair<int, int> edge =
                graph_recognition::detail::proper_chordal_parent_edge(
                    state, tree_parent);
            ASSERT_NE(edge, std::make_pair(0, 0))
                << "n=" << n << " graph#" << i;
            graph_recognition::detail::proper_chordal_remove_edge(
                &state, edge.first, edge.second);
            const Graph parent(
                n,
                graph_recognition::detail::collect_proper_chordal_edges(state));
            EXPECT_TRUE(check_proper_chordal(parent).is_proper_chordal)
                << "n=" << n << " graph#" << i;
        }
    }
}

}  // namespace
