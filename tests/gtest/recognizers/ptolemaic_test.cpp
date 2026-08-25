#include "ptolemaic.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_ptolemaic;
using graph_recognition::PtolemaicResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "ptolemaic";

class PtolemaicTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PtolemaicTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    PtolemaicResult r = check_ptolemaic(g);
    ASSERT_EQ(r.is_ptolemaic, exp == "YES") << "case=" << stem;
    if (!r.is_ptolemaic) {
        EXPECT_TRUE(r.obstruction.kind == ObstructionKind::HOLE ||
                    r.obstruction.kind == ObstructionKind::GEM)
            << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PtolemaicTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
