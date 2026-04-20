#include "permutation_enum.h"
#include "permutation.h"
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
        // Method 1: brute force all 2^(n choose 2) graphs + filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        int perm_by_filter = 0;
        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            PermutationResult res = check_permutation(g);
            if (res.is_permutation) {
                perm_by_filter++;
            }
        }

        // Method 2: enumeration
        PermutationEnumerationResult enum_res =
            enumerate_permutation_graphs_reverse_search(n);
        int perm_by_enum = (int)enum_res.graphs.size();

        // Verify each enumerated graph is indeed permutation
        for (int gi = 0; gi < perm_by_enum; ++gi) {
            const EnumeratedGraph& eg = enum_res.graphs[gi];
            Graph g(n, eg.edges);
            PermutationResult res = check_permutation(g);
            if (!res.is_permutation) {
                std::cerr << "INVALID n=" << n << " graph#" << gi
                          << " is not permutation" << std::endl;
                mismatch++;
            }
        }

        // Compare counts
        if (perm_by_filter != perm_by_enum) {
            std::cerr << "MISMATCH n=" << n
                      << " filter=" << perm_by_filter
                      << " enum=" << perm_by_enum << std::endl;
            mismatch++;
        }

        std::cout << "n=" << n
                  << " total=" << (1LL << num_edges)
                  << " permutation=" << perm_by_enum << std::endl;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
