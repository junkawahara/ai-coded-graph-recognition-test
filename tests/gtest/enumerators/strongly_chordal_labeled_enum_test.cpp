#include "recognizers/strongly_chordal.h"
#include "enumerators/strongly_chordal_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::StronglyChordalLabeledEnumAlgorithm;
using graph_recognition::StronglyChordalLabeledEnumerationResult;
using graph_recognition::check_strongly_chordal;
using graph_recognition::enumerate_strongly_chordal_labeled_graphs_reverse_search;
using graph_recognition::enumerate_strongly_chordal_labeled_graphs_reverse_search_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "strongly_chordal_labeled_enum";

class StronglyChordalLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StronglyChordalLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    StronglyChordalLabeledEnumerationResult res = enumerate_strongly_chordal_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_strongly_chordal(g).is_strongly_chordal) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, StronglyChordalLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

std::set<std::vector<std::pair<int, int>>> graph_set(
    const StronglyChordalLabeledEnumerationResult& result) {
    std::set<std::vector<std::pair<int, int>>> graphs;
    for (std::size_t i = 0; i < result.graphs.size(); ++i) {
        graphs.insert(result.graphs[i].edges);
    }
    return graphs;
}

bool is_strong_elimination_order(
    const graph_recognition::detail::KiyomiStronglyChordalState& state,
    const std::vector<int>& order) {
    // Directly check Farber's four-index closed-neighborhood definition.
    if (order.size() != static_cast<std::size_t>(state.total_n)) return false;
    for (std::size_t i = 0; i < order.size(); ++i) {
        for (std::size_t j = i + 1; j < order.size(); ++j) {
            for (std::size_t k = 0; k < order.size(); ++k) {
                for (std::size_t l = k + 1; l < order.size(); ++l) {
                    const int vi = order[i];
                    const int vj = order[j];
                    const int vk = order[k];
                    const int vl = order[l];
                    const bool ik = vi == vk || state.adj[vi][vk];
                    const bool il = vi == vl || state.adj[vi][vl];
                    const bool jk = vj == vk || state.adj[vj][vk];
                    const bool jl = vj == vl || state.adj[vj][vl];
                    if (ik && il && jk && !jl) return false;
                }
            }
        }
    }
    return true;
}

TEST(StronglyChordalLabeledEnumKiyomiTest, EnumeratesSameSetsAsLegacyFilter) {
    for (int n = 0; n <= 6; ++n) {
        const StronglyChordalLabeledEnumerationResult dedicated =
            enumerate_strongly_chordal_labeled_graphs_reverse_search(
                n, StronglyChordalLabeledEnumAlgorithm::KIYOMI_EDGE_ADDITION);
        const StronglyChordalLabeledEnumerationResult legacy =
            enumerate_strongly_chordal_labeled_graphs_reverse_search(
                n, StronglyChordalLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER);

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

TEST(StronglyChordalLabeledEnumKiyomiTest, ExhaustiveSimpleGraphsAgreeThroughSixVertices) {
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
            if (check_strongly_chordal(Graph(n, edges)).is_strongly_chordal) {
                brute_force.insert(edges);
            }
        }

        const StronglyChordalLabeledEnumerationResult dedicated =
            enumerate_strongly_chordal_labeled_graphs_reverse_search(n);
        EXPECT_EQ(graph_set(dedicated), brute_force) << "n=" << n;
    }
}

TEST(StronglyChordalLabeledEnumKiyomiTest, StreamsSameGraphsAsMaterializingApi) {
    for (int n = 0; n <= 5; ++n) {
        const StronglyChordalLabeledEnumerationResult materialized =
            enumerate_strongly_chordal_labeled_graphs_reverse_search(n);
        std::vector<graph_recognition::EnumeratedGraph> streamed;
        enumerate_strongly_chordal_labeled_graphs_reverse_search_cb(
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

TEST(StronglyChordalLabeledEnumKiyomiTest, CanonicalParentDeletionPreservesClass) {
    for (int n = 1; n <= 6; ++n) {
        const StronglyChordalLabeledEnumerationResult result =
            enumerate_strongly_chordal_labeled_graphs_reverse_search(n);
        for (std::size_t i = 0; i < result.graphs.size(); ++i) {
            if (result.graphs[i].edges.empty()) continue;

            graph_recognition::detail::KiyomiStronglyChordalState state(n);
            for (std::size_t j = 0; j < result.graphs[i].edges.size(); ++j) {
                graph_recognition::detail::strongly_chordal_add_edge(
                    &state,
                    result.graphs[i].edges[j].first,
                    result.graphs[i].edges[j].second);
            }

            std::vector<int> order;
            ASSERT_TRUE(
                graph_recognition::detail::canonical_strong_elimination_order(
                    state, &order))
                << "n=" << n << " graph#" << i;
            ASSERT_TRUE(is_strong_elimination_order(state, order))
                << "n=" << n << " graph#" << i;

            const std::pair<int, int> edge =
                graph_recognition::detail::strongly_chordal_parent_edge(state);
            ASSERT_NE(edge, std::make_pair(0, 0))
                << "n=" << n << " graph#" << i;
            graph_recognition::detail::strongly_chordal_remove_edge(
                &state, edge.first, edge.second);
            const Graph parent(
                n, graph_recognition::detail::collect_strongly_chordal_edges(state));
            EXPECT_TRUE(check_strongly_chordal(parent).is_strongly_chordal)
                << "n=" << n << " graph#" << i;
        }
    }
}

TEST(StronglyChordalLabeledEnumKiyomiTest, RejectsSunAsAChild) {
    // A 3-sun: clique {1,2,3}; independent vertices 4,5,6 see consecutive
    // pairs {1,2}, {2,3}, and {3,1}, respectively.
    const std::vector<std::pair<int, int>> sun_edges = {
        {1, 2}, {1, 3}, {2, 3},
        {1, 4}, {2, 4}, {2, 5}, {3, 5}, {1, 6}, {3, 6}
    };
    graph_recognition::detail::KiyomiStronglyChordalState state(6);
    for (std::size_t i = 0; i < sun_edges.size(); ++i) {
        graph_recognition::detail::strongly_chordal_add_edge(
            &state, sun_edges[i].first, sun_edges[i].second);
    }
    EXPECT_EQ(graph_recognition::detail::strongly_chordal_parent_edge(state),
              std::make_pair(0, 0));
}

}  // namespace
