#include "chordal_enum.h"
#include "proper_interval_enum.h"
#include "proper_interval.h"
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
        // Method 1: chordal enumeration + proper interval filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int pi_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            ProperIntervalResult pr = check_proper_interval(g);
            if (pr.is_proper_interval) {
                pi_by_filter++;
            }
        }

        // Method 2: direct proper interval enumeration
        ProperIntervalEnumerationResult pi_res =
            enumerate_proper_interval_graphs_reverse_search(n);
        int pi_by_enum = (int)pi_res.graphs.size();

        // Verify each enumerated graph is indeed proper interval
        for (int gi = 0; gi < pi_by_enum; ++gi) {
            const EnumeratedGraph& eg = pi_res.graphs[gi];
            Graph g(n, eg.edges);
            ProperIntervalResult pr = check_proper_interval(g);
            if (!pr.is_proper_interval) {
                std::cerr << "INVALID n=" << n << " graph#" << gi
                          << " is not proper interval" << std::endl;
                mismatch++;
            }
        }

        // Compare counts
        if (pi_by_filter != pi_by_enum) {
            std::cerr << "MISMATCH n=" << n
                      << " filter=" << pi_by_filter
                      << " enum=" << pi_by_enum << std::endl;
            mismatch++;
        }

        std::cout << "n=" << n
                  << " chordal=" << total_chordal
                  << " proper_interval=" << pi_by_enum << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
