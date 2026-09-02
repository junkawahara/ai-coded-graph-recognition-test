#include "recognizers/chordal.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using graph_recognition::ChordalAlgorithm;
using graph_recognition::ChordalResult;
using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_chordal;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_chordal_peo;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "chordal";

class ChordalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ChordalResult r = check_chordal(g);
    ASSERT_EQ(r.is_chordal, exp == "YES") << "case=" << stem;
    if (r.is_chordal) {
        EXPECT_TRUE(verify_chordal_peo(g, r)) << "case=" << stem;
    } else {
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::HOLE) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

class ChordalVariantTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalVariantTest, AllAlgorithmsAgree) {
    Graph g = load_graph(test_path(std::string(kDir) + "/" + GetParam() + ".in"));
    const ChordalAlgorithm algos[] = {ChordalAlgorithm::MCS_PEO,
                                      ChordalAlgorithm::BUCKET_MCS_PEO,
                                      ChordalAlgorithm::LEXBFS_PEO};
    ChordalResult first = check_chordal(g, algos[0]);
    for (size_t i = 0; i < 3; ++i) {
        ChordalResult r = check_chordal(g, algos[i]);
        EXPECT_EQ(r.is_chordal, first.is_chordal) << "case=" << GetParam() << " algo=" << i;
        // The variants reach different orderings and so different holes, but
        // each one has to be a hole of g.
        if (!r.is_chordal) {
            EXPECT_TRUE(verify_obstruction(g, r.obstruction))
                << "case=" << GetParam() << " algo=" << i;
        }
    }
}

// A value outside the enum must not turn into a mathematical NO: the
// default-constructed result has no PEO and no hole either, so a caller
// cannot tell a configuration mistake from an answer about the graph.
TEST(ChordalAlgorithmSelectorTest, OutOfRangeValueRunsTheDefault) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    Graph g(3, edges);
    ChordalResult bogus = check_chordal(g, static_cast<ChordalAlgorithm>(99));
    ChordalResult def = check_chordal(g);
    EXPECT_TRUE(bogus.is_chordal);
    EXPECT_EQ(bogus.mcs_result.order, def.mcs_result.order);
    EXPECT_EQ(bogus.parent, def.parent);
}

// The negative-count graph the constructor clamps must still be answered
// (bucket MCS used to write out of bounds on it).
TEST(ChordalAlgorithmSelectorTest, ClampedNegativeVertexCountIsChordal) {
    Graph g(-1, std::vector<std::pair<int, int>>());
    const ChordalAlgorithm algos[3] = {ChordalAlgorithm::MCS_PEO,
                                       ChordalAlgorithm::BUCKET_MCS_PEO,
                                       ChordalAlgorithm::LEXBFS_PEO};
    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(check_chordal(g, algos[i]).is_chordal) << "algo=" << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalVariantTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
