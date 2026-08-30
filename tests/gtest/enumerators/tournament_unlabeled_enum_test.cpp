#include "recognizers/tournament.h"
#include "enumerators/tournament_labeled_enum.h"
#include "enumerators/tournament_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::TournamentUnlabeledEnumerationResult;
using graph_recognition::check_tournament;
using graph_recognition::enumerate_tournament_unlabeled_graphs;
using graph_recognition::enumerate_tournaments;
using graph_recognition::gtest_utils::canonical_arc_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "tournament_unlabeled_enum";

class TournamentUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(TournamentUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    TournamentUnlabeledEnumerationResult res = enumerate_tournament_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int> > key = res.graphs[i].arcs;
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate digraph in case=" << stem;

        // This class enumerates one representative per isomorphism class, so
        // also reject isomorphic duplicates (canonical form by brute-force
        // permutation; guarded to n <= 7, since n = 8 would be 6880
        // tournaments times 8! orderings).
        if (res.graphs[i].n <= 7) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_arc_list(res.graphs[i].n,
                                                       res.graphs[i].arcs))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        EXPECT_TRUE(check_tournament(res.graphs[i].n, res.graphs[i].arcs).is_tournament)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, TournamentUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Capped at n = 6 (2^15 labeled tournaments,
// each canonicalized by brute force over 6! orderings).
TEST(TournamentUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::TournamentLabeledEnumerationResult labeled =
            enumerate_tournaments(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_arc_list(labeled.graphs[i].n, labeled.graphs[i].arcs));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        TournamentUnlabeledEnumerationResult unlabeled =
            enumerate_tournament_unlabeled_graphs(n);
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

// n = 0 emits the single empty tournament, matching the labeled enumerator
// and A000568(0) = 1.
TEST(TournamentUnlabeledEnumEdgeCaseTest, EmptyGraph) {
    TournamentUnlabeledEnumerationResult res = enumerate_tournament_unlabeled_graphs(0);
    ASSERT_EQ(res.graphs.size(), 1u);
    EXPECT_EQ(res.graphs[0].n, 0);
    EXPECT_TRUE(res.graphs[0].arcs.empty());

    EXPECT_TRUE(enumerate_tournament_unlabeled_graphs(-1).graphs.empty());
}

}  // namespace
