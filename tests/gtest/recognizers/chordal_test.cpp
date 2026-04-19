#include "chordal.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::ChordalAlgorithm;
using graph_recognition::ChordalResult;
using graph_recognition::Graph;
using graph_recognition::check_chordal;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "chordal";

class ChordalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ChordalResult r = check_chordal(g);
    ASSERT_EQ(r.is_chordal, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

class ChordalVariantTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalVariantTest, AllAlgorithmsAgree) {
    Graph g = load_graph(test_path(std::string(kDir) + "/" + GetParam() + ".in"));
    bool a = check_chordal(g, ChordalAlgorithm::MCS_PEO).is_chordal;
    bool b = check_chordal(g, ChordalAlgorithm::BUCKET_MCS_PEO).is_chordal;
    bool c = check_chordal(g, ChordalAlgorithm::LEXBFS_PEO).is_chordal;
    EXPECT_EQ(a, b) << "case=" << GetParam();
    EXPECT_EQ(b, c) << "case=" << GetParam();
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalVariantTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
