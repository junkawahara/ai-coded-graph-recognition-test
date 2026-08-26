#include "recognizers/triangle_free.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::TriangleFreeAlgorithm;
using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_triangle_free;
using graph_recognition::TriangleFreeResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "triangle_free";

class TriangleFreeTest : public ::testing::TestWithParam<std::string> {};

TEST_P(TriangleFreeTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    const TriangleFreeAlgorithm algos[] = {TriangleFreeAlgorithm::BRUTE,
                                           TriangleFreeAlgorithm::EDGE_PAIR};
    for (size_t i = 0; i < 2; ++i) {
        TriangleFreeResult r = check_triangle_free(g, algos[i]);
        ASSERT_EQ(r.is_triangle_free, exp == "YES") << "case=" << stem << " algo=" << i;
        if (r.is_triangle_free) continue;
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::TRIANGLE)
            << "case=" << stem << " algo=" << i;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction))
            << "case=" << stem << " algo=" << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, TriangleFreeTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
