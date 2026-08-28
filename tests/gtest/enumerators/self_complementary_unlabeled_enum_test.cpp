#include "recognizers/self_complementary.h"
#include "enumerators/self_complementary_labeled_enum.h"
#include "enumerators/self_complementary_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::SelfComplementaryUnlabeledEnumerationResult;
using graph_recognition::check_self_complementary;
using graph_recognition::enumerate_self_complementary_labeled_graphs;
using graph_recognition::enumerate_self_complementary_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "self_complementary_unlabeled_enum";

class SelfComplementaryUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(SelfComplementaryUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    SelfComplementaryUnlabeledEnumerationResult res =
        enumerate_self_complementary_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_EQ(res.graphs[i].n, n) << "case=" << stem;

        std::vector<std::pair<int, int> > key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        // A self-complementary graph has exactly half the pairs as edges.
        EXPECT_EQ(static_cast<int>(key.size()), n * (n - 1) / 4)
            << "wrong edge count in case=" << stem;

        // This class enumerates one representative per isomorphism class, so
        // also reject isomorphic duplicates (canonical form by brute-force
        // permutation; guarded to n <= 8, since n = 9 would be 36 graphs
        // times 9! orderings).
        if (n <= 8) {
            EXPECT_TRUE(canon_seen.insert(canonical_edge_list(n, res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(n, res.graphs[i].edges);
        EXPECT_TRUE(check_self_complementary(g).is_self_complementary)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, SelfComplementaryUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Capped at n = 6 (canonicalization is 6! per
// labeled graph, and the labeled count jumps to 98280 at n = 8).
TEST(SelfComplementaryUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::SelfComplementaryLabeledEnumerationResult labeled =
            enumerate_self_complementary_labeled_graphs(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_edge_list(labeled.graphs[i].n, labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        SelfComplementaryUnlabeledEnumerationResult unlabeled =
            enumerate_self_complementary_unlabeled_graphs(n);
        for (size_t i = 0; i < unlabeled.graphs.size(); ++i) {
            unlabeled_classes.insert(
                canonical_edge_list(unlabeled.graphs[i].n, unlabeled.graphs[i].edges));
        }

        EXPECT_EQ(unlabeled.graphs.size(), unlabeled_classes.size())
            << "isomorphic duplicates for n=" << n;
        EXPECT_EQ(unlabeled_classes, labeled_classes)
            << "class set differs from the labeled enumerator for n=" << n;
    }
}

// Sizes that admit no self-complementary graph (n = 2, 3 mod 4) and the
// out-of-range guards return nothing.
TEST(SelfComplementaryUnlabeledEnumEdgeCaseTest, EmptyOutsideValidSizes) {
    const int impossible[] = {2, 3, 6, 7, 10, 11, 14, 15};
    for (size_t i = 0; i < sizeof(impossible) / sizeof(impossible[0]); ++i) {
        EXPECT_TRUE(
            enumerate_self_complementary_unlabeled_graphs(impossible[i]).graphs.empty())
            << "n=" << impossible[i];
    }
    EXPECT_TRUE(enumerate_self_complementary_unlabeled_graphs(-1).graphs.empty());
    EXPECT_TRUE(enumerate_self_complementary_unlabeled_graphs(64).graphs.empty());
}

}  // namespace
