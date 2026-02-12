#include "weakly_chordal.h"
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
            // chordal graph (always weakly chordal)
            n = 1 + rand() % 8;
            std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
            for (int v = 2; v <= n; ++v) {
                int u = 1 + rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
                adj[u][v] = adj[v][u] = true;
                for (int w = 1; w < v; ++w) {
                    if (w != u && adj[u][w] && rand() % 2 == 0) {
                        edges.push_back(std::make_pair(w, v));
                        adj[w][v] = adj[v][w] = true;
                    }
                }
            }
        } else if (trial % 3 == 1) {
            n = 1 + rand() % 8;
            double p = (rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + rand() % 8;
            double p = (rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        WeaklyChordalResult r1 = check_weakly_chordal(g, WeaklyChordalAlgorithm::CO_CHORDAL_BIPARTITE);
        WeaklyChordalResult r2 = check_weakly_chordal(g, WeaklyChordalAlgorithm::COMPLEMENT_BFS);

        if (r1.is_weakly_chordal != r2.is_weakly_chordal) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " CO=" << r1.is_weakly_chordal
                      << " BFS=" << r2.is_weakly_chordal << std::endl;
            mismatch++;
        }

        total++;
        if (r1.is_weakly_chordal) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
