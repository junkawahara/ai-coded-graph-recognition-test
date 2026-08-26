#include "recognizers/kregular.h"
#include "enumerators/kregular_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::KRegularLabeledEnumerationResult;
using graph_recognition::check_kregular;
using graph_recognition::enumerate_kregular_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "kregular_labeled_enum";

class KRegularLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(KRegularLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    std::ifstream fin(test_path(std::string(kDir) + "/" + stem + ".in").c_str());
    int n = 0, k = 0;
    fin >> n >> k;
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    KRegularLabeledEnumerationResult res = enumerate_kregular_labeled_graphs_reverse_search(n, k);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int>>> seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int>> key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        graph_recognition::KRegularResult r = check_kregular(g);
        EXPECT_TRUE(r.is_kregular) << "invalid graph in case=" << stem;
        EXPECT_EQ(r.k, k) << "wrong k in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, KRegularLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
