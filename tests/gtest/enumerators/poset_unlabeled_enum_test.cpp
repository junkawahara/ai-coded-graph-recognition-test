#include "recognizers/poset.h"
#include "enumerators/poset_labeled_enum.h"
#include "enumerators/poset_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::PosetUnlabeledEnumerationResult;
using graph_recognition::check_poset;
using graph_recognition::enumerate_poset_unlabeled_graphs;
using graph_recognition::enumerate_posets;
using graph_recognition::gtest_utils::canonical_arc_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "poset_unlabeled_enum";

class PosetUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PosetUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    PosetUnlabeledEnumerationResult res = enumerate_poset_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int> > key = res.graphs[i].arcs;
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate poset in case=" << stem;

        // This class enumerates one representative per isomorphism class, so
        // also reject isomorphic duplicates (canonical form by brute-force
        // permutation over the Hasse arcs; guarded to n <= 6, since n = 7
        // would be 2045 posets times 7! orderings).
        if (res.graphs[i].n <= 6) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_arc_list(res.graphs[i].n,
                                                       res.graphs[i].arcs))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        EXPECT_TRUE(check_poset(res.graphs[i].n, res.graphs[i].arcs).is_poset)
            << "invalid Hasse diagram in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PosetUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Two posets are isomorphic iff their Hasse
// diagrams are isomorphic as digraphs, so canonicalizing the Hasse arc
// lists compares the classes. Capped at n = 5 (4231 labeled posets
// canonicalized by brute force over 5! orderings; n = 6 would be 130023
// of them times 6! orderings).
TEST(PosetUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 5; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::PosetLabeledEnumerationResult labeled =
            enumerate_posets(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_arc_list(labeled.graphs[i].n, labeled.graphs[i].arcs));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        PosetUnlabeledEnumerationResult unlabeled =
            enumerate_poset_unlabeled_graphs(n);
        for (size_t i = 0; i < unlabeled.graphs.size(); ++i) {
            unlabeled_classes.insert(
                canonical_arc_list(unlabeled.graphs[i].n, unlabeled.graphs[i].arcs));
        }

        EXPECT_EQ(unlabeled.graphs.size(), unlabeled_classes.size())
            << "isomorphic duplicates for n=" << n;
        EXPECT_EQ(unlabeled_classes, labeled_classes)
            << "class set differs from the labeled enumerator for n=" << n;
    }
}

// n = 0 emits the single empty poset, matching the labeled enumerator and
// A000112(0) = 1.
TEST(PosetUnlabeledEnumEdgeCaseTest, EmptyPoset) {
    PosetUnlabeledEnumerationResult res = enumerate_poset_unlabeled_graphs(0);
    ASSERT_EQ(res.graphs.size(), 1u);
    EXPECT_EQ(res.graphs[0].n, 0);
    EXPECT_TRUE(res.graphs[0].arcs.empty());

    EXPECT_TRUE(enumerate_poset_unlabeled_graphs(-1).graphs.empty());
}

}  // namespace
