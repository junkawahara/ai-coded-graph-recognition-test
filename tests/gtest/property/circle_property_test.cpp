#include "circle.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::CircleAlgorithm;
using graph_recognition::check_circle;

unsigned long long rng_state = 88172645463325252ULL;
unsigned long long xrand() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}

// Naji's linear system must agree with DOW backtracking on all labeled
// graphs with n <= 6.
TEST(CircleProperty, NajiAgreesWithDowExhaustive) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            bool naji = check_circle(g, CircleAlgorithm::NAJI_SYSTEM).is_circle;
            bool dow = check_circle(g, CircleAlgorithm::DOW_BACKTRACKING).is_circle;
            ASSERT_EQ(naji, dow) << "n=" << n << " mask=" << mask;
        }
    }
}

// Random graphs of several densities at n = 7, 8 (kept small because the
// DOW reference is exponential: a NO answer costs ~0.1 s at n = 7 and
// ~1.3 s at n = 8).
TEST(CircleProperty, NajiAgreesWithDowRandom) {
    const int counts[2] = {150, 30};

    for (int ni = 0; ni < 2; ++ni) {
        int n = 7 + ni;
        for (int t = 0; t < counts[ni]; ++t) {
            unsigned long long denom = 2 + xrand() % 5;  // edge prob 1/2 .. 1/6
            std::vector<std::pair<int, int>> edges;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (xrand() % denom == 0) edges.push_back(std::make_pair(u, v));
            Graph g(n, edges);
            bool naji = check_circle(g, CircleAlgorithm::NAJI_SYSTEM).is_circle;
            bool dow = check_circle(g, CircleAlgorithm::DOW_BACKTRACKING).is_circle;
            ASSERT_EQ(naji, dow) << "n=" << n << " trial=" << t;
        }
    }
}

}  // namespace
