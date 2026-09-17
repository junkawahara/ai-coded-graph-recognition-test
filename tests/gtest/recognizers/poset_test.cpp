#include "recognizers/poset.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <climits>
#include <stdexcept>
#include <utility>
#include <vector>

using graph_recognition::PosetResult;
using graph_recognition::check_poset;
using graph_recognition::gtest_utils::DirectedInput;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::load_arcs;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "poset";

class PosetTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PosetTest, MatchesExpected) {
    const std::string stem = GetParam();
    DirectedInput in = load_arcs(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    PosetResult r = check_poset(in.n, in.arcs);
    ASSERT_EQ(r.is_poset, exp == "YES") << "case=" << stem;
}

TEST(PosetEdgeCaseTest, UnrepresentableVertexCountThrows) {
    // check_poset sizes its own n + 1 arrays without going through Graph,
    // so n = INT_MAX was signed overflow here too. It must be refused with
    // a clear exception; negative n stays a plain NO.
    std::vector<std::pair<int, int>> arcs;
    EXPECT_THROW(check_poset(INT_MAX, arcs), std::invalid_argument);
    EXPECT_FALSE(check_poset(-1, arcs).is_poset);
    EXPECT_TRUE(check_poset(0, arcs).is_poset);
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PosetTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
