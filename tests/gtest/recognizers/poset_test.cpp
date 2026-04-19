#include "poset.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

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

INSTANTIATE_TEST_SUITE_P(
    AllCases, PosetTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
