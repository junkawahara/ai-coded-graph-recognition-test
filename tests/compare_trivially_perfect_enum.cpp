#include "trivially_perfect_enum.h"
#include "trivially_perfect.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>

using namespace graph_recognition;

int main(int argc, char* argv[]) {
    int max_n = 5;
    if (argc >= 2) max_n = atoi(argv[1]);
    int mismatch = 0;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute-force enumerate all labeled graphs + TP filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                all_edges.push_back(std::make_pair(u, v));
            }
        }

        int tp_by_filter = 0;
        for (int mask = 0; mask < (1 << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1 << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            TriviallyPerfectResult tr = check_trivially_perfect(g);
            if (tr.is_trivially_perfect) {
                tp_by_filter++;
            }
        }

        // Method 2: direct TP enumeration via UVD tree
        TriviallyPerfectEnumerationResult tp_res =
            enumerate_trivially_perfect_graphs_uvd(n);
        int tp_by_enum = (int)tp_res.graphs.size();

        // Verify each enumerated graph is indeed trivially perfect
        for (int gi = 0; gi < tp_by_enum; ++gi) {
            const EnumeratedGraph& eg = tp_res.graphs[gi];
            Graph g(n, eg.edges);
            TriviallyPerfectResult tr = check_trivially_perfect(g);
            if (!tr.is_trivially_perfect) {
                std::cerr << "INVALID n=" << n << " graph#" << gi
                          << " is not trivially perfect" << std::endl;
                mismatch++;
            }
        }

        // Compare counts
        if (tp_by_filter != tp_by_enum) {
            std::cerr << "MISMATCH n=" << n
                      << " filter=" << tp_by_filter
                      << " enum=" << tp_by_enum << std::endl;
            mismatch++;
        }

        std::cout << "n=" << n
                  << " total=" << (1 << num_edges)
                  << " tp=" << tp_by_enum << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
