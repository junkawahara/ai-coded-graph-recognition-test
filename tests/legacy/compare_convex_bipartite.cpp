#include "convex_bipartite.h"
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

        if (trial % 4 == 0) {
            // convex bipartite by construction: interval neighborhoods
            n = 2 + rand() % 7;
            int half = n / 2;
            if (half < 1) half = 1;
            int other = n - half;
            // X = {1..half}, Y = {half+1..n}
            for (int x = 1; x <= half; ++x) {
                if (other == 0) break;
                int lo = rand() % other;
                int hi = lo + rand() % (other - lo);
                for (int j = lo; j <= hi; ++j) {
                    edges.push_back(std::make_pair(x, half + 1 + j));
                }
            }
        } else if (trial % 4 == 1) {
            // random bipartite graph
            n = 2 + rand() % 7;
            int half = n / 2;
            if (half < 1) half = 1;
            for (int u = 1; u <= half; ++u)
                for (int v = half + 1; v <= n; ++v)
                    if (rand() % 3 == 0)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 4 == 2) {
            // random graph (may not be bipartite)
            n = 1 + rand() % 8;
            double p = (rand() % 60 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // tree (always bipartite, always convex bipartite)
            n = 2 + rand() % 7;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
        }

        Graph g(n, edges);
        ConvexBipartiteResult r1 = check_convex_bipartite(g, ConvexBipartiteAlgorithm::C1P);
        ConvexBipartiteResult r2 = check_convex_bipartite(g, ConvexBipartiteAlgorithm::BRUTE_FORCE);

        if (r1.is_convex_bipartite != r2.is_convex_bipartite) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " C1P=" << r1.is_convex_bipartite
                      << " BRUTE=" << r2.is_convex_bipartite << std::endl;
            std::cerr << "  edges:";
            for (size_t i = 0; i < edges.size(); ++i) {
                std::cerr << " (" << edges[i].first << "," << edges[i].second << ")";
            }
            std::cerr << std::endl;
            mismatch++;
        }

        total++;
        if (r1.is_convex_bipartite) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
