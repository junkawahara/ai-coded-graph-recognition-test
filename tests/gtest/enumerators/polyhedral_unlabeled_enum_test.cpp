#include "recognizers/polyhedral.h"
#include "enumerators/polyhedral_labeled_enum.h"
#include "enumerators/polyhedral_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::PolyhedralUnlabeledEnumerationResult;
using graph_recognition::check_polyhedral;
using graph_recognition::enumerate_polyhedral_labeled_graphs;
using graph_recognition::enumerate_polyhedral_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "polyhedral_unlabeled_enum";

class PolyhedralUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(PolyhedralUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    PolyhedralUnlabeledEnumerationResult res =
        enumerate_polyhedral_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int> > key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        // This class enumerates one representative per isomorphism class, so
        // also reject isomorphic duplicates (canonical form by brute-force
        // permutation; guarded to n <= 7, since n = 8 would be 257 graphs
        // times 8! orderings).
        if (res.graphs[i].n <= 7) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_polyhedral(g).is_polyhedral)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PolyhedralUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Capped at n = 6 (canonicalization is 6! per
// labeled graph, and the labeled counts grow like 2^(n log n)).
TEST(PolyhedralUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::PolyhedralLabeledEnumerationResult labeled =
            enumerate_polyhedral_labeled_graphs(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_edge_list(labeled.graphs[i].n, labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        PolyhedralUnlabeledEnumerationResult unlabeled =
            enumerate_polyhedral_unlabeled_graphs(n);
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

}  // namespace
