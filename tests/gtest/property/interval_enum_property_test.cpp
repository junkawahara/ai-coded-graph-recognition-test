#include "chordal_enum.h"
#include "interval.h"
#include "interval_enum.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::IntervalAlgorithm;
using graph_recognition::IntervalResult;
using graph_recognition::check_interval;
using graph_recognition::ChordalEnumerationResult;
using graph_recognition::enumerate_chordal_graphs_reverse_search;
using graph_recognition::IntervalEnumerationResult;
using graph_recognition::enumerate_interval_graphs_reverse_search;

/**
 * @brief 区間モデルがグラフを正しく表現しているか検証する
 *
 * intervals[v] = (L, R) で、区間が重なる ⟺ 辺が存在 を確認。
 */
bool validate_interval_model(const Graph& g,
    const std::vector<std::pair<int, int>>& intervals) {
    for (int u = 1; u <= g.n; ++u) {
        if (intervals[u].first > intervals[u].second) return false;
    }
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            bool overlap = !(intervals[u].second < intervals[v].first ||
                             intervals[v].second < intervals[u].first);
            bool edge = g.has_edge(u, v);
            if (overlap != edge) return false;
        }
    }
    return true;
}

TEST(IntervalEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 7;

    for (int n = 1; n <= max_n; ++n) {
        ChordalEnumerationResult enumres =
            enumerate_chordal_graphs_reverse_search(n);

        int total = (int)enumres.graphs.size();
        int interval_by_filter = 0;

        for (int gi = 0; gi < total; ++gi) {
            const EnumeratedGraph& eg = enumres.graphs[gi];
            Graph g(n, eg.edges);

            IntervalResult r1 = check_interval(g, IntervalAlgorithm::BACKTRACKING);
            IntervalResult r2 = check_interval(g, IntervalAlgorithm::AT_FREE);

            // 両アルゴリズムの一致チェック
            ASSERT_EQ(r1.is_interval, r2.is_interval)
                << "n=" << n << " graph#" << gi;

            if (r1.is_interval) {
                interval_by_filter++;
                // BACKTRACKING の区間モデル検証
                ASSERT_TRUE(validate_interval_model(g, r1.intervals))
                    << "BT n=" << n << " graph#" << gi;
                // AT_FREE の区間モデル検証
                ASSERT_TRUE(validate_interval_model(g, r2.intervals))
                    << "AT n=" << n << " graph#" << gi;
            }
        }

        // 列挙の一致検証: interval 列挙器の件数が chordal 列挙 + interval
        // フィルタの件数と一致すること (集合としての一致は n<=6 で
        // interval_enum_full_property_test が全グラフ総当たりに対して検証
        // 済み。ここでは n=7 まで件数を照合してカバレッジを広げる)。
        IntervalEnumerationResult ienum = enumerate_interval_graphs_reverse_search(n);
        ASSERT_EQ((int)ienum.graphs.size(), interval_by_filter) << "n=" << n;
    }
}

}  // namespace
