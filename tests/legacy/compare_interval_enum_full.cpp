#include "chordal_enum.h"
#include "interval_enum.h"
#include "interval.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>

using namespace graph_recognition;

int main(int argc, char* argv[]) {
    int max_n = 6;
    if (argc >= 2) max_n = atoi(argv[1]);
    int mismatch = 0;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: chordal enumeration + interval filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int interval_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            IntervalResult ir = check_interval(g);
            if (ir.is_interval) {
                interval_by_filter++;
            }
        }

        // Method 2: direct interval enumeration
        IntervalEnumerationResult interval_res =
            enumerate_interval_graphs_reverse_search(n);
        int interval_by_enum = (int)interval_res.graphs.size();

        // Verify each enumerated graph is indeed interval
        for (int gi = 0; gi < interval_by_enum; ++gi) {
            const EnumeratedGraph& eg = interval_res.graphs[gi];
            Graph g(n, eg.edges);
            IntervalResult ir = check_interval(g);
            if (!ir.is_interval) {
                std::cerr << "INVALID n=" << n << " graph#" << gi
                          << " is not interval" << std::endl;
                mismatch++;
            }
        }

        // Compare counts
        if (interval_by_filter != interval_by_enum) {
            std::cerr << "MISMATCH n=" << n
                      << " filter=" << interval_by_filter
                      << " enum=" << interval_by_enum << std::endl;
            mismatch++;
        }

        std::cout << "n=" << n
                  << " chordal=" << total_chordal
                  << " interval=" << interval_by_enum << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
