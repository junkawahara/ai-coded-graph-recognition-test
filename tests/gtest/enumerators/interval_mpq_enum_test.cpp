#include "enumerators/interval_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <map>
#include <random>
#include <set>
#include <tuple>
#include <vector>

namespace {

using namespace graph_recognition;
using Edges = std::vector<std::pair<int, int>>;
using detail::IntervalMPQTree;
using gtest_utils::canonical_edge_list;
using Algorithm = IntervalUnlabeledEnumAlgorithm;

std::vector<int> model_string(const Graph& graph) {
    const auto model = check_interval(graph);
    EXPECT_TRUE(model.is_interval);
    std::vector<std::tuple<int, int, int>> events;
    for (int v = 1; v <= graph.n; ++v) {
        events.emplace_back(model.intervals[v].first, 0, v - 1);
        events.emplace_back(model.intervals[v].second, 1, v - 1);
    }
    std::sort(events.begin(), events.end());
    std::vector<int> result;
    for (const auto& event : events) result.push_back(std::get<2>(event));
    return result;
}

// Independent endpoint-intersection oracle; preserves the input labels.
Edges endpoint_edges(const std::vector<int>& endpoints) {
    const int n = static_cast<int>(endpoints.size() / 2);
    std::vector<int> left(n, -1), right(n, -1);
    for (size_t i = 0; i < endpoints.size(); ++i) {
        const int v = endpoints[i];
        if (left[v] < 0) left[v] = static_cast<int>(i);
        else right[v] = static_cast<int>(i);
    }
    Edges edges;
    for (int u = 0; u < n; ++u) for (int v = u + 1; v < n; ++v)
        if (left[u] <= right[v] && left[v] <= right[u]) edges.emplace_back(u + 1, v + 1);
    return edges;
}

std::vector<unsigned long long> graph_form(int n, const Edges& edges) {
    if (!n) return {}; // The shared canonicalizer requires at least one vertex.
    std::vector<unsigned long long> adj(n, 0);
    for (const auto& e : edges) {
        adj[e.first - 1] |= 1ULL << (e.second - 1);
        adj[e.second - 1] |= 1ULL << (e.first - 1);
    }
    return canonicalize_bitmask_graph(n, adj).form;
}

TEST(IntervalMPQCanonicalTest, AllSmallEndpointOrdersMatchBruteForceIsomorphism) {
    // All (2n-1)!! unlabeled interval endpoint pairings, including different
    // clique orders and containment patterns for the same graph (945 at n=5).
    for (int n = 0; n <= 5; ++n) {
        std::map<Edges, std::vector<int>> forms;
        std::vector<int> word, count(n, 0);
        std::function<void(int)> visit = [&](int opened) {
            if (word.size() == static_cast<size_t>(2 * n)) {
                const auto edges = endpoint_edges(word);
                const auto brute = canonical_edge_list(n, edges);
                const IntervalMPQTree tree(word);
                EXPECT_EQ(canonical_edge_list(n, endpoint_edges(tree.string)), brute);
                const auto inserted = forms.emplace(brute, tree.string);
                EXPECT_EQ(inserted.first->second, tree.string) << "n=" << n;
                return;
            }
            for (int v = 0; v < n && v <= opened; ++v) {
                if (count[v] == 2) continue;
                ++count[v];
                word.push_back(v);
                visit(std::max(opened, v + 1));
                word.pop_back();
                --count[v];
            }
        };
        visit(0);
    }
}

TEST(IntervalMPQEdgeTest, EveryDeletionMatchesIndependentRecognitionThroughEightVertices) {
    for (int n = 1; n <= 8; ++n) {
        const auto legacy = enumerate_interval_unlabeled_graphs(n, false, Algorithm::CANONICAL_AUGMENTATION);
        for (const auto& graph : legacy.graphs) {
            const IntervalMPQTree tree(model_string(Graph(n, graph.edges)));
            const auto edges = endpoint_edges(tree.string);
            ASSERT_EQ(graph_form(n, edges), graph_form(n, graph.edges));
            for (size_t i = 0; i < edges.size(); ++i) {
                auto removed = edges;
                removed.erase(removed.begin() + i);
                std::vector<int> endpoints;
                const bool accepted = tree.delete_edge(edges[i].first - 1, edges[i].second - 1, endpoints);
                ASSERT_EQ(accepted, check_interval(Graph(n, removed)).is_interval)
                    << "n=" << n << " edge=" << edges[i].first << ',' << edges[i].second;
                if (accepted) {
                    ASSERT_EQ(endpoints.size(), static_cast<size_t>(2 * n));
                    EXPECT_EQ(endpoint_edges(endpoints), removed);
                    const IntervalMPQTree rebuilt(endpoints);
                    EXPECT_EQ(graph_form(n, endpoint_edges(rebuilt.string)), graph_form(n, removed));
                }
            }
            if (edges.size() == static_cast<size_t>(n * (n - 1) / 2)) continue;
            // Independently find the lexicographically first valid added edge.
            Edges expected;
            for (int u = 1; u <= n && expected.empty(); ++u) {
                for (int v = u + 1; v <= n; ++v) {
                    if (std::binary_search(edges.begin(), edges.end(), std::make_pair(u, v))) continue;
                    auto added = edges;
                    added.emplace_back(u, v);
                    std::sort(added.begin(), added.end());
                    if (check_interval(Graph(n, added)).is_interval) { expected = added; break; }
                }
            }
            EXPECT_EQ(endpoint_edges(tree.parent_string()), expected);
        }
    }
}

TEST(IntervalMPQCanonicalTest, RelabelingAndReflectionPreserveStrings) {
    std::mt19937 random(2041);
    // Nested universal vertices, twin classes, central sections with/without
    // neighbors, and equal connected components occur in this random sample.
    for (int n : {8, 12, 20, 63}) {
        std::vector<int> word;
        for (int v = 0; v < n; ++v) { word.push_back(v); word.push_back(v); }
        for (int trial = 0; trial < 30; ++trial) {
            std::shuffle(word.begin(), word.end(), random);
            const IntervalMPQTree expected(word);
            std::vector<int> labels;
            for (int v = 0; v < n; ++v) labels.push_back(v);
            std::shuffle(labels.begin(), labels.end(), random);
            auto relabeled = word;
            for (int& v : relabeled) v = labels[v];
            std::reverse(relabeled.begin(), relabeled.end());
            EXPECT_EQ(IntervalMPQTree(relabeled).string, expected.string);
            EXPECT_EQ(IntervalMPQTree(expected.string).string, expected.string);
            EXPECT_EQ(expected.string.size(), static_cast<size_t>(2 * n));
        }
    }
}

TEST(IntervalMPQEnumTest, BothModesMatchLegacyClassesThroughEightVertices) {
    for (int n = 0; n <= 8; ++n) for (bool connected : {false, true}) {
        const auto actual = enumerate_interval_unlabeled_graphs(n, connected);
        const auto legacy = enumerate_interval_unlabeled_graphs(n, connected, Algorithm::CANONICAL_AUGMENTATION);
        std::set<std::vector<unsigned long long>> expected, found;
        for (const auto& g : legacy.graphs) expected.insert(graph_form(n, g.edges));
        for (const auto& g : actual.graphs) {
            EXPECT_EQ(g.n, n);
            EXPECT_TRUE(std::is_sorted(g.edges.begin(), g.edges.end()));
            EXPECT_TRUE(found.insert(graph_form(n, g.edges)).second);
        }
        EXPECT_EQ(found, expected) << "n=" << n << " connected=" << connected;
    }
}

TEST(IntervalMPQEnumTest, LegacyOrderingAndInvalidArguments) {
    const auto legacy = enumerate_interval_unlabeled_graphs(3, false, Algorithm::CANONICAL_AUGMENTATION);
    const std::vector<Edges> expected = {{}, {{1, 3}}, {{1, 3}, {2, 3}}, {{1, 2}, {1, 3}, {2, 3}}};
    ASSERT_EQ(legacy.graphs.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) EXPECT_EQ(legacy.graphs[i].edges, expected[i]);
    for (const auto algo : {Algorithm::MPQ_TREE, Algorithm::CANONICAL_AUGMENTATION}) {
        EXPECT_THROW(enumerate_interval_unlabeled_graphs(-1, false, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_interval_unlabeled_graphs(64, false, algo), std::invalid_argument);
    }
    EXPECT_THROW(enumerate_interval_unlabeled_graphs(0, false, static_cast<Algorithm>(99)), std::invalid_argument);
    EXPECT_THROW(enumerate_interval_unlabeled_graphs(3, true, static_cast<Algorithm>(99)), std::invalid_argument);
}

} // namespace
