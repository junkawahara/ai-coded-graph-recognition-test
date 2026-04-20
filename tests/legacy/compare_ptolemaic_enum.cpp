#include "chordal_enum.h"
#include "ptolemaic_enum.h"
#include "ptolemaic.h"
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
        // Method 1: chordal enumeration + ptolemaic filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int ptolemaic_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            PtolemaicResult pr = check_ptolemaic(g);
            if (pr.is_ptolemaic) {
                ptolemaic_by_filter++;
            }
        }

        // Method 2: direct ptolemaic enumeration
        PtolemaicEnumerationResult ptol_res =
            enumerate_ptolemaic_graphs_reverse_search(n);
        int ptolemaic_by_enum = (int)ptol_res.graphs.size();

        // Verify each enumerated graph is indeed ptolemaic
        for (int gi = 0; gi < ptolemaic_by_enum; ++gi) {
            const EnumeratedGraph& eg = ptol_res.graphs[gi];
            Graph g(n, eg.edges);
            PtolemaicResult pr = check_ptolemaic(g);
            if (!pr.is_ptolemaic) {
                std::cerr << "INVALID n=" << n << " graph#" << gi
                          << " is not ptolemaic" << std::endl;
                mismatch++;
            }
        }

        // Compare counts
        if (ptolemaic_by_filter != ptolemaic_by_enum) {
            std::cerr << "MISMATCH n=" << n
                      << " filter=" << ptolemaic_by_filter
                      << " enum=" << ptolemaic_by_enum << std::endl;
            mismatch++;
        }

        std::cout << "n=" << n
                  << " chordal=" << total_chordal
                  << " ptolemaic=" << ptolemaic_by_enum << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
