#include "chordal_enum.h"
#include "interval.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>

using namespace graph_recognition;

/**
 * @brief 区間モデルがグラフを正しく表現しているか検証する
 *
 * intervals[v] = (L, R) で、区間が重なる ⟺ 辺が存在 を確認。
 */
static bool validate_interval_model(const Graph& g,
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

int main(int argc, char* argv[]) {
    int max_n = 7;
    if (argc >= 2) max_n = atoi(argv[1]);
    int mismatch = 0;

    for (int n = 1; n <= max_n; ++n) {
        ChordalEnumerationResult enumres =
            enumerate_chordal_graphs_reverse_search(n);

        int total = (int)enumres.graphs.size();
        int interval_count = 0;

        for (int gi = 0; gi < total; ++gi) {
            const EnumeratedGraph& eg = enumres.graphs[gi];
            Graph g(n, eg.edges);

            IntervalResult r1 = check_interval(g, IntervalAlgorithm::BACKTRACKING);
            IntervalResult r2 = check_interval(g, IntervalAlgorithm::AT_FREE);

            // 両アルゴリズムの一致チェック
            if (r1.is_interval != r2.is_interval) {
                std::cerr << "MISMATCH n=" << n << " graph#" << gi
                          << " BT=" << r1.is_interval
                          << " AT=" << r2.is_interval << std::endl;
                std::cerr << "  edges:";
                for (size_t e = 0; e < eg.edges.size(); ++e) {
                    std::cerr << " " << eg.edges[e].first
                              << "-" << eg.edges[e].second;
                }
                std::cerr << std::endl;
                mismatch++;
                continue;
            }

            if (r1.is_interval) {
                interval_count++;

                // BACKTRACKING の区間モデル検証
                if (!validate_interval_model(g, r1.intervals)) {
                    std::cerr << "INVALID MODEL (BT) n=" << n
                              << " graph#" << gi << std::endl;
                    mismatch++;
                }
                // AT_FREE の区間モデル検証
                if (!validate_interval_model(g, r2.intervals)) {
                    std::cerr << "INVALID MODEL (AT) n=" << n
                              << " graph#" << gi << std::endl;
                    mismatch++;
                }
            }
        }

        std::cout << "n=" << n
                  << " chordal=" << total
                  << " interval=" << interval_count << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
