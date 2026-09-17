#include "recognizers/tournament.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <climits>
#include <utility>
#include <vector>

using graph_recognition::TournamentResult;
using graph_recognition::check_tournament;
using graph_recognition::gtest_utils::DirectedInput;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::load_arcs;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "tournament";

class TournamentTest : public ::testing::TestWithParam<std::string> {};

// A negative n is refused before any arithmetic on it: n - 1 used to be
// computed first, which is signed overflow for INT_MIN (review_all_1.md #16).
TEST(TournamentEdgeCaseTest, NegativeVertexCountIsNotATournament) {
    std::vector<std::pair<int, int>> arcs;
    EXPECT_FALSE(check_tournament(INT_MIN, arcs).is_tournament);
    EXPECT_FALSE(check_tournament(-1, arcs).is_tournament);
    EXPECT_TRUE(check_tournament(0, arcs).is_tournament);
    EXPECT_TRUE(check_tournament(1, arcs).is_tournament);
}

TEST_P(TournamentTest, MatchesExpected) {
    const std::string stem = GetParam();
    DirectedInput in = load_arcs(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    TournamentResult r = check_tournament(in.n, in.arcs);
    ASSERT_EQ(r.is_tournament, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, TournamentTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
