#include "recognizers/digraph.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::DigraphResult;
using graph_recognition::check_digraph;
using graph_recognition::gtest_utils::DirectedInput;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::load_arcs;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "digraph";

class DigraphTest : public ::testing::TestWithParam<std::string> {};

TEST_P(DigraphTest, MatchesExpected) {
    const std::string stem = GetParam();
    DirectedInput in = load_arcs(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    DigraphResult r = check_digraph(in.n, in.arcs);
    ASSERT_EQ(r.is_digraph, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DigraphTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
