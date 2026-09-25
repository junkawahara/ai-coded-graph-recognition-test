#include "enumerators/cubic_unlabeled_enum.h"
#include "enumerators/snark_labeled_enum.h"
#include "enumerators/snark_unlabeled_enum.h"
#include "util/snarkhunter_enum.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;
using Edges = std::vector<std::pair<int, int> >;
using Codes = std::set<std::vector<int> >;

SnarkhunterGraph adjacency(int n, const Edges& edges) {
    SnarkhunterGraph g(n, 0);
    for (const auto& e : edges) snarkhunter_add(g, e.first - 1, e.second - 1);
    return g;
}

template <typename Graphs>
Codes codes(const Graphs& graphs) {
    Codes result;
    for (const auto& g : graphs) result.insert(cubic_graph_canonical_code(g.n, g.edges));
    return result;
}

TEST(SnarkhunterTest, PrimeCountsIncludeAllThreeConstructions) {
    // Brinkmann--Goedgebeur--McKay (2011), Table 1.
    const auto levels = snarkhunter_primes(18);
    const std::size_t expected[] = {1, 0, 1, 1, 1, 3, 2, 5};
    for (int n = 4; n <= 18; n += 2) {
        ASSERT_EQ(levels[n].size(), expected[(n - 4) / 2]) << n;
        for (const auto& code : levels[n]) {
            const auto g = snarkhunter_from_code(code);
            EXPECT_EQ(g.size(), static_cast<std::size_t>(n));
            EXPECT_TRUE(snarkhunter_is_prime(g));
            EXPECT_TRUE(bitmask_graph_connected(n, g));
            for (auto row : g) {
                int degree = 0;
                for (; row; row &= row - 1) ++degree;
                EXPECT_EQ(degree, 3);
            }
        }
    }
}

TEST(SnarkhunterTest, CubicConstructionMatchesIndependentVertexAugmentation) {
    const std::size_t expected[] = {1, 2, 5, 19, 85};
    for (int n = 4; n <= 12; n += 2) {
        Codes actual;
        std::size_t count = 0;
        snarkhunter_enumerate(n, [&](const Edges& edges) {
            ++count;
            EXPECT_TRUE(actual.insert(cubic_graph_canonical_code(n, edges)).second);
            const Graph graph(n, edges);
            for (int v = 1; v <= n; ++v) EXPECT_EQ(graph.adj[v].size(), 3u);
        }, false);
        EXPECT_EQ(count, expected[(n - 4) / 2]) << n;
        EXPECT_EQ(actual, codes(enumerate_cubic_unlabeled_graphs(n, true).graphs)) << n;
    }
}

TEST(SnarkhunterTest, InsertionReductionAndCanonicalParentIgnoreLabels) {
    std::mt19937 rng(57013);
    snarkhunter_enumerate(8, [&](const Edges& edges) {
        const auto g = adjacency(8, edges);
        const auto parent_code = snarkhunter_code(g);
        const auto pairs = snarkhunter_edges(g);
        for (std::size_t a = 0; a < pairs.size(); ++a) {
            for (std::size_t b = a + 1; b < pairs.size(); ++b) {
                const auto child = snarkhunter_insert(g, pairs[a], pairs[b]);
                SnarkhunterGraph reduced;
                ASSERT_TRUE(snarkhunter_reduce(child, 8, 9, reduced));
                EXPECT_EQ(reduced, g);
                const auto code = snarkhunter_code(child);
                std::vector<int> labels;
                for (int i = 0; i < 10; ++i) labels.push_back(i);
                std::shuffle(labels.begin(), labels.end(), rng);
                SnarkhunterGraph permuted(10, 0);
                for (const auto& e : snarkhunter_edges(child))
                    snarkhunter_add(permuted, labels[e.first], labels[e.second]);
                EXPECT_EQ(snarkhunter_code(permuted), code);
                const auto canonical = snarkhunter_from_code(code);
                bool found = false;
                for (const auto& e : snarkhunter_edges(canonical)) {
                    if (!snarkhunter_reduce(canonical, e.first, e.second, reduced)) continue;
                    found = true;
                    EXPECT_EQ(snarkhunter_has_parent(code, parent_code),
                        snarkhunter_code(reduced) == parent_code);
                    break;
                }
                EXPECT_TRUE(found);
            }
        }
    }, false);
}

TEST(SnarkhunterTest, EveryLookaheadRejectionHasAnIndependentColoring) {
    std::size_t rejected = 0, kempe = 0;
    for (int n = 4; n <= 10; n += 2) {
        snarkhunter_enumerate(n, [&](const Edges& edges) {
            const auto g = adjacency(n, edges);
            const auto pairs = snarkhunter_edges(g);
            std::vector<int> coloring;
            const bool colorable = snarkhunter_coloring(n, pairs, coloring);
            EXPECT_EQ(colorable, is_3_edge_colorable(Graph(n, edges)));
            if (colorable) {
                std::vector<int> masks(n, 0);
                for (std::size_t i = 0; i < pairs.size(); ++i) {
                    ASSERT_GE(coloring[i], 0);
                    ASSERT_LT(coloring[i], 3);
                    masks[pairs[i].first] |= 1 << coloring[i];
                    masks[pairs[i].second] |= 1 << coloring[i];
                }
                for (int mask : masks) EXPECT_EQ(mask, 7);
            }
            const auto cycles = snarkhunter_lookahead(n, pairs);
            if (cycles.size() > 3) ++kempe;
            for (std::size_t a = 0; a < pairs.size(); ++a)
                for (std::size_t b = a + 1; b < pairs.size(); ++b) {
                    if (!snarkhunter_colorable_pair(cycles, a, b)) continue;
                    ++rejected;
                    const auto child = snarkhunter_insert(g, pairs[a], pairs[b]);
                    EXPECT_TRUE(is_3_edge_colorable(Graph(n + 2,
                        bitmask_graph_edges(n + 2, child))));
                }
        }, false);
    }
    EXPECT_GT(rejected, 0u);
    EXPECT_GT(kempe, 0u);
}

TEST(SnarkhunterTest, PruningPreservesTheUnrestrictedCubicFilter) {
    std::size_t with_tests = 0, without_tests = 0;
    for (int n : {10, 12}) {
        SCOPED_TRACE(n);
        Codes expected, actual;
        snarkhunter_enumerate(n, [&](const Edges& edges) {
            if (check_snark(Graph(n, edges)).is_snark)
                expected.insert(cubic_graph_canonical_code(n, edges));
        }, false);
        SnarkhunterStats without, with;
        snarkhunter_enumerate(n, [&](const Edges& edges) {
            actual.insert(cubic_graph_canonical_code(n, edges));
        }, true, false, &without);
        EXPECT_EQ(actual, expected);
        actual.clear();
        snarkhunter_enumerate(n, [&](const Edges& edges) {
            actual.insert(cubic_graph_canonical_code(n, edges));
        }, true, true, &with);
        EXPECT_EQ(actual, expected);
        EXPECT_GT(with.lookahead_rejections, 0u);
        EXPECT_LE(with.terminal_color_tests, without.terminal_color_tests);
        with_tests += with.terminal_color_tests;
        without_tests += without.terminal_color_tests;
    }
    // At n=10 every cubic girth-5 candidate is already the Petersen graph;
    // the coloring look-ahead starts saving terminal tests at n=12.
    EXPECT_LT(with_tests, without_tests);
}

TEST(SnarkhunterTest, NonemptyLabeledOutputMatchesLegacyExactly) {
    const auto result = enumerate_snark_labeled_graphs(10);
    const auto legacy = enumerate_snark_labeled_graphs(10, SnarkLabeledEnumAlgorithm::REVERSE_SEARCH);
    std::set<Edges> actual, expected;
    for (const auto& g : result.graphs) EXPECT_TRUE(actual.insert(g.edges).second);
    for (const auto& g : legacy.graphs) EXPECT_TRUE(expected.insert(g.edges).second);
    EXPECT_EQ(result.graphs.size(), 30240u); // 10! / |Aut(Petersen)| = 10! / 120.
    EXPECT_EQ(actual, expected);
    ASSERT_FALSE(result.graphs.empty());
    EXPECT_TRUE(check_snark(Graph(10, result.graphs.front().edges)).is_snark);
}

TEST(SnarkhunterTest, UnlabeledClassesMatchLegacyAndKnownSnarkCounts) {
    for (int n : {10, 12, 14}) {
        const auto result = enumerate_snark_unlabeled_graphs(n);
        const auto legacy = enumerate_snark_unlabeled_graphs(n,
            SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
        EXPECT_EQ(codes(result.graphs), codes(legacy.graphs)) << n;
    }
    EXPECT_TRUE(enumerate_snark_unlabeled_graphs(16).graphs.empty());
    const auto result = enumerate_snark_unlabeled_graphs(18);
    ASSERT_EQ(result.graphs.size(), 2u); // The two Blanusa snarks.
    EXPECT_EQ(codes(result.graphs).size(), 2u);
    for (const auto& g : result.graphs) EXPECT_TRUE(check_snark(Graph(g.n, g.edges)).is_snark);
}

TEST(SnarkhunterTest, DefaultsLimitsAndLegacyRepresentativeArePreserved) {
    EXPECT_EQ(enumerate_snark_unlabeled_graphs(10).graphs[0].edges,
        enumerate_snark_unlabeled_graphs(10, SnarkUnlabeledEnumAlgorithm::SNARKHUNTER).graphs[0].edges);
    for (auto algorithm : {SnarkLabeledEnumAlgorithm::SNARKHUNTER,
                          SnarkLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        for (int n : {0, 1, 4, 8, 9, 11})
            EXPECT_TRUE(enumerate_snark_labeled_graphs(n, algorithm).graphs.empty());
        EXPECT_THROW(enumerate_snark_labeled_graphs(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_snark_labeled_graphs(64, algorithm), std::invalid_argument);
    }
    for (auto algorithm : {SnarkUnlabeledEnumAlgorithm::SNARKHUNTER,
                          SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION}) {
        for (int n : {0, 1, 4, 8, 9, 11, 63})
            EXPECT_TRUE(enumerate_snark_unlabeled_graphs(n, algorithm).graphs.empty());
        EXPECT_THROW(enumerate_snark_unlabeled_graphs(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_snark_unlabeled_graphs(64, algorithm), std::invalid_argument);
    }
    EXPECT_TRUE(enumerate_snark_labeled_graphs(63).graphs.empty());
    EXPECT_THROW(enumerate_snark_labeled_graphs(31, SnarkLabeledEnumAlgorithm::REVERSE_SEARCH),
        std::invalid_argument);
    EXPECT_THROW(enumerate_snark_labeled_graphs(0, static_cast<SnarkLabeledEnumAlgorithm>(99)),
        std::invalid_argument);
    EXPECT_THROW(enumerate_snark_unlabeled_graphs(0, static_cast<SnarkUnlabeledEnumAlgorithm>(99)),
        std::invalid_argument);
    const Edges previous{{1,5},{1,6},{1,8},{2,5},{2,7},{2,9},{3,6},{3,7},
                         {3,10},{4,8},{4,9},{4,10},{5,10},{6,9},{7,8}};
    const auto legacy = enumerate_snark_unlabeled_graphs(10,
        SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
    ASSERT_EQ(legacy.graphs.size(), 1u);
    EXPECT_EQ(legacy.graphs[0].edges, previous);
}
}  // namespace
