#include "chordal_enum.h"
#include "split_enum.h"
#include "split.h"
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
        // Method 1: chordal enumeration + split filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int split_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            SplitResult sr = check_split(g);
            if (sr.is_split) {
                split_by_filter++;
            }
        }

        // Method 2: direct split enumeration
        SplitEnumerationResult split_res =
            enumerate_split_graphs_reverse_search(n);
        int split_by_enum = (int)split_res.graphs.size();

        // Verify each enumerated graph is indeed split
        for (int gi = 0; gi < split_by_enum; ++gi) {
            const EnumeratedGraph& eg = split_res.graphs[gi];
            Graph g(n, eg.edges);
            SplitResult sr = check_split(g);
            if (!sr.is_split) {
                std::cerr << "INVALID n=" << n << " graph#" << gi
                          << " is not split" << std::endl;
                mismatch++;
            }
        }

        // Compare counts
        if (split_by_filter != split_by_enum) {
            std::cerr << "MISMATCH n=" << n
                      << " filter=" << split_by_filter
                      << " enum=" << split_by_enum << std::endl;
            mismatch++;
        }

        std::cout << "n=" << n
                  << " chordal=" << total_chordal
                  << " split=" << split_by_enum << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
