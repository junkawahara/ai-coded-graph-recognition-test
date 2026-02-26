#include "biconvex_bipartite.h"
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

        if (trial % 5 == 0) {
            // biconvex by construction: both sides have interval neighborhoods
            // X = {1..half}, Y = {half+1..n}
            n = 2 + rand() % 7;
            int half = n / 2;
            if (half < 1) half = 1;
            int other = n - half;
            // Assign Y-positions to X vertices (for X-side C1P)
            // and X-positions to Y vertices (for Y-side C1P)
            // Simple: make a chain graph (always biconvex)
            // Sort X by degree ascending, connect each x to suffix of Y
            for (int x = 1; x <= half; ++x) {
                int lo = half - x; // higher index x -> more neighbors
                if (lo >= other) lo = other - 1;
                for (int j = lo; j < other; ++j) {
                    edges.push_back(std::make_pair(x, half + 1 + j));
                }
            }
        } else if (trial % 5 == 1) {
            // random bipartite graph
            n = 2 + rand() % 7;
            int half = n / 2;
            if (half < 1) half = 1;
            for (int u = 1; u <= half; ++u)
                for (int v = half + 1; v <= n; ++v)
                    if (rand() % 3 == 0)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 2) {
            // random graph (may not be bipartite)
            n = 1 + rand() % 8;
            double p = (rand() % 60 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 3) {
            // tree (always bipartite, always biconvex)
            n = 2 + rand() % 7;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
        } else {
            // path or cycle
            n = 3 + rand() % 6;
            for (int v = 1; v < n; ++v) {
                edges.push_back(std::make_pair(v, v + 1));
            }
            // even cycle: biconvex; odd cycle: not bipartite
            if (rand() % 2 == 0 && n >= 4) {
                edges.push_back(std::make_pair(1, n));
            }
        }

        Graph g(n, edges);
        BiconvexBipartiteResult r1 =
            check_biconvex_bipartite(g, BiconvexBipartiteAlgorithm::C1P);
        BiconvexBipartiteResult r2 =
            check_biconvex_bipartite(g, BiconvexBipartiteAlgorithm::BRUTE_FORCE);

        if (r1.is_biconvex_bipartite != r2.is_biconvex_bipartite) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " C1P=" << r1.is_biconvex_bipartite
                      << " BRUTE=" << r2.is_biconvex_bipartite << std::endl;
            std::cerr << "  edges:";
            for (size_t i = 0; i < edges.size(); ++i) {
                std::cerr << " (" << edges[i].first << "," << edges[i].second << ")";
            }
            std::cerr << std::endl;
            mismatch++;
        }

        total++;
        if (r1.is_biconvex_bipartite) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
