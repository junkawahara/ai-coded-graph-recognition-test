#include "even_hole_free.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_even_hole_free;
using graph_recognition::EvenHoleFreeResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "even_hole_free";

class EvenHoleFreeTest : public ::testing::TestWithParam<std::string> {};

TEST_P(EvenHoleFreeTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    EvenHoleFreeResult r = check_even_hole_free(g);
    ASSERT_EQ(r.is_even_hole_free, exp == "YES") << "case=" << stem;
    if (!r.is_even_hole_free) {
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::EVEN_HOLE) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, EvenHoleFreeTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
