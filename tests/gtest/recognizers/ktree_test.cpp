#include "recognizers/ktree.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_ktree;
using graph_recognition::KTreeResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "ktree";

class KTreeTest : public ::testing::TestWithParam<std::string> {};

TEST_P(KTreeTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    KTreeResult r = check_ktree(g);
    ASSERT_EQ(r.is_ktree, exp == "YES") << "case=" << stem;
    if (r.is_ktree) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_ktree_construction(
            g, r.k, r.construction_order))
            << "case=" << stem << " k=" << r.k;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, KTreeTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
