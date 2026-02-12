#include "chordal_bipartite.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

int main() {
    srand(42);
    int total = 0, yes_count = 0, no_count = 0, mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // bipartite tree (always chordal bipartite)
            n = 2 + rand() % 8;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
            // check if bipartite (tree always is, but add extra edges carefully)
        } else if (trial % 3 == 1) {
            n = 1 + rand() % 9;
            double p = (rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + rand() % 9;
            double p = (rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        ChordalBipartiteResult r1 = check_chordal_bipartite(g, ChordalBipartiteAlgorithm::BISIMPLICIAL);
        ChordalBipartiteResult r2 = check_chordal_bipartite(g, ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL);
        ChordalBipartiteResult r3 = check_chordal_bipartite(g, ChordalBipartiteAlgorithm::CYCLE_CHECK);

        bool all_same = (r1.is_chordal_bipartite == r2.is_chordal_bipartite) &&
                        (r2.is_chordal_bipartite == r3.is_chordal_bipartite);
        if (!all_same) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " BISIM=" << r1.is_chordal_bipartite
                      << " FAST=" << r2.is_chordal_bipartite
                      << " CYCLE=" << r3.is_chordal_bipartite << std::endl;
            mismatch++;
        }

        total++;
        if (r1.is_chordal_bipartite) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
