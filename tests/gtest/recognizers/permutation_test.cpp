#include "permutation.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_permutation_obstruction;
using graph_recognition::check_permutation;
using graph_recognition::PermutationResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "permutation";

class PermutationTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PermutationTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    PermutationResult r = check_permutation(g);
    ASSERT_EQ(r.is_permutation, exp == "YES") << "case=" << stem;
    if (!r.is_permutation) {
        Obstruction built = build_permutation_obstruction(g);
        EXPECT_EQ(built.kind, ObstructionKind::FORCING_CYCLE) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PermutationTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
