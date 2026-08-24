#include "weakly_chordal.h"
#include "weakly_chordal_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::WeaklyChordalEnumAlgorithm;
using graph_recognition::WeaklyChordalEnumerationResult;
using graph_recognition::check_weakly_chordal;
using graph_recognition::enumerate_weakly_chordal_graphs_reverse_search;
using graph_recognition::enumerate_weakly_chordal_graphs_reverse_search_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "weakly_chordal_enum";

std::set<std::vector<std::pair<int, int>>> edge_set(
    const WeaklyChordalEnumerationResult& result) {
    std::set<std::vector<std::pair<int, int>>> graphs;
    for (size_t i = 0; i < result.graphs.size(); ++i) {
        graphs.insert(result.graphs[i].edges);
    }
    return graphs;
}

class WeaklyChordalEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(WeaklyChordalEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    WeaklyChordalEnumerationResult res = enumerate_weakly_chordal_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_weakly_chordal(g).is_weakly_chordal) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, WeaklyChordalEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(WeaklyChordalEnumAlgorithms, KiyomiSearchIsTheDefault) {
    WeaklyChordalEnumerationResult default_result =
        enumerate_weakly_chordal_graphs_reverse_search(4);
    WeaklyChordalEnumerationResult kiyomi_result =
        enumerate_weakly_chordal_graphs_reverse_search(
            4, WeaklyChordalEnumAlgorithm::KIYOMI_EDGE_ADDITION);

    ASSERT_EQ(default_result.graphs.size(), kiyomi_result.graphs.size());
    for (size_t i = 0; i < default_result.graphs.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].n, kiyomi_result.graphs[i].n);
        EXPECT_EQ(default_result.graphs[i].edges,
                  kiyomi_result.graphs[i].edges);
    }
}

TEST(WeaklyChordalEnumAlgorithms, KiyomiAndGenericSearchAgree) {
    for (int n = 0; n <= 5; ++n) {
        WeaklyChordalEnumerationResult kiyomi =
            enumerate_weakly_chordal_graphs_reverse_search(
                n, WeaklyChordalEnumAlgorithm::KIYOMI_EDGE_ADDITION);
        WeaklyChordalEnumerationResult generic =
            enumerate_weakly_chordal_graphs_reverse_search(
                n,
                WeaklyChordalEnumAlgorithm::GENERIC_VERTEX_AUGMENTATION);

        EXPECT_EQ(kiyomi.graphs.size(), edge_set(kiyomi).size()) << "n=" << n;
        EXPECT_EQ(generic.graphs.size(), edge_set(generic).size()) << "n=" << n;
        EXPECT_EQ(edge_set(kiyomi), edge_set(generic)) << "n=" << n;
    }
}

TEST(WeaklyChordalEnumAlgorithms, CanonicalParentUsesYoungestRemovableEdge) {
    graph_recognition::detail::KiyomiWeaklyChordalState triangle(3);
    triangle.present[0] = 1;  // {1, 2}
    triangle.present[1] = 1;  // {1, 3}
    triangle.present[2] = 1;  // {2, 3}, the youngest edge
    triangle.edge_count = 3;

    EXPECT_FALSE(graph_recognition::detail::kiyomi_weakly_chordal_is_parent_edge(
        triangle, 0));
    EXPECT_TRUE(graph_recognition::detail::kiyomi_weakly_chordal_is_parent_edge(
        triangle, 2));
}

TEST(WeaklyChordalEnumAlgorithms, StreamingAndMaterializedApisAgree) {
    const int n = 5;
    WeaklyChordalEnumerationResult materialized =
        enumerate_weakly_chordal_graphs_reverse_search(n);
    std::vector<EnumeratedGraph> streamed;
    enumerate_weakly_chordal_graphs_reverse_search_cb(
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
