#include "recognizers/strongly_chordal.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::check_strongly_chordal;
using graph_recognition::StronglyChordalResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "strongly_chordal";

class StronglyChordalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StronglyChordalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    StronglyChordalResult r = check_strongly_chordal(g);
    ASSERT_EQ(r.is_strongly_chordal, exp == "YES") << "case=" << stem;

    StronglyChordalResult farber = check_strongly_chordal(
        g, graph_recognition::StronglyChordalAlgorithm::FARBER_SEO);
    ASSERT_EQ(farber.is_strongly_chordal, r.is_strongly_chordal) << "case=" << stem;
    if (farber.is_strongly_chordal) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_seo(
            g, farber.seo_order, farber.seo_number))
            << "case=" << stem;
    }
}

// The four variants take genuinely different routes -- PEO_MATRIX compares
// every pair of neighbourhoods against the definition, MCS_SEO only the
// consecutive pairs of the alive-degree order, FARBER_SEO builds the strong
// ordering -- so agreement on every small graph is what keeps the cheaper
// ones honest.
TEST(StronglyChordalVariantTest, ExhaustiveAgreementUpToFiveVertices) {
    using graph_recognition::StronglyChordalAlgorithm;
    const StronglyChordalAlgorithm algos[4] = {
        StronglyChordalAlgorithm::STRONG_ELIMINATION,
        StronglyChordalAlgorithm::PEO_MATRIX,
        StronglyChordalAlgorithm::MCS_SEO,
        StronglyChordalAlgorithm::FARBER_SEO};
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        const int m = static_cast<int>(all.size());
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);
            const bool expected =
                check_strongly_chordal(g, algos[0]).is_strongly_chordal;
            for (int i = 1; i < 4; ++i) {
                ASSERT_EQ(check_strongly_chordal(g, algos[i]).is_strongly_chordal,
                          expected)
                    << "n=" << n << " mask=" << mask << " algo=" << i;
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, StronglyChordalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
