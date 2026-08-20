#include "circular_arc.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::CircularArcAlgorithm;
using graph_recognition::Graph;
using graph_recognition::check_circular_arc;

// The two in-tree implementations are algorithmically unrelated (maximal
// clique enumeration + circular clique ordering vs endpoint-order
// backtracking + 2-SAT), so random agreement between them replaces the
// deleted external brute-force oracle.
TEST(CircularArcProperty, AlgorithmsAgreeOnRandomGraphs) {
    std::srand(42);

    // n <= 7: the backtracking side is exponential and already ~50 s at
    // n = 8 over this many trials.
    for (int trial = 0; trial < 1000; ++trial) {
        int n = 1 + std::rand() % 7;
        double p;
        if (trial % 3 == 0) {
            p = 0.25 + (std::rand() % 30) / 100.0;
        } else if (trial % 3 == 1) {
            p = 0.55 + (std::rand() % 40) / 100.0;  // dense: circular-arc likely
        } else {
            p = (std::rand() % 100) / 100.0;
        }
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                if ((std::rand() % 1000) / 1000.0 < p)
                    edges.push_back(std::make_pair(u, v));

        Graph g(n, edges);
        bool mc = check_circular_arc(g, CircularArcAlgorithm::MCCONNELL).is_circular_arc;
        bool bt = check_circular_arc(g, CircularArcAlgorithm::BACKTRACKING).is_circular_arc;
        ASSERT_EQ(mc, bt)
            << "MCCONNELL vs BACKTRACKING trial=" << trial << " n=" << n
            << " m=" << edges.size();
    }
}

}  // namespace
