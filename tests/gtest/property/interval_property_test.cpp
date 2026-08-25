#include "graph.h"
#include "interval.h"

#include "certificates.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::IntervalAlgorithm;
using graph_recognition::IntervalResult;
using graph_recognition::check_interval;
using graph_recognition::gtest_utils::verify_interval_model;

// A random interval graph: draw n intervals and join the overlapping pairs.
std::vector<std::pair<int, int>> gen_random_interval_graph(int n) {
    std::vector<std::pair<int, int>> model(n + 1);
    for (int v = 1; v <= n; ++v) {
        int a = std::rand() % (2 * n);
        int b = a + std::rand() % n;
        model[v] = std::make_pair(a, b);
    }
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            bool overlap = !(model[u].second < model[v].first || model[v].second < model[u].first);
            if (overlap) edges.push_back(std::make_pair(u, v));
        }
    }
    return edges;
}

TEST(IntervalProperty, AllAlgorithmsAgreeAndModelsAreValid) {
    std::srand(20260825);

    for (int trial = 0; trial < 3000; ++trial) {
        int n = 1 + std::rand() % 9;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            edges = gen_random_interval_graph(n);
        } else if (trial % 3 == 1) {
            double p = (std::rand() % 60 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // Perturb an interval graph by one random edge flip: this lands
            // near the boundary of the class, where the variants are most
            // likely to disagree.
            edges = gen_random_interval_graph(n);
            if (n >= 2) {
                int u = 1 + std::rand() % n;
                int v = 1 + std::rand() % n;
                if (u != v) {
                    if (u > v) std::swap(u, v);
                    std::vector<std::pair<int, int>> kept;
                    bool removed = false;
                    for (size_t i = 0; i < edges.size(); ++i) {
                        if (edges[i] == std::make_pair(u, v)) {
                            removed = true;
                        } else {
                            kept.push_back(edges[i]);
                        }
                    }
                    if (!removed) kept.push_back(std::make_pair(u, v));
                    edges.swap(kept);
                }
            }
        }

        Graph g(n, edges);
        IntervalResult bt = check_interval(g, IntervalAlgorithm::BACKTRACKING);
        IntervalResult af = check_interval(g, IntervalAlgorithm::AT_FREE);
        IntervalResult pq = check_interval(g, IntervalAlgorithm::PQ_TREE);

        ASSERT_EQ(bt.is_interval, af.is_interval)
            << "BACKTRACKING vs AT_FREE trial=" << trial << " n=" << n;
        ASSERT_EQ(bt.is_interval, pq.is_interval)
            << "BACKTRACKING vs PQ_TREE trial=" << trial << " n=" << n;

        if (bt.is_interval) {
            ASSERT_TRUE(verify_interval_model(g, bt.intervals)) << "trial=" << trial;
            ASSERT_TRUE(verify_interval_model(g, af.intervals)) << "trial=" << trial;
            ASSERT_TRUE(verify_interval_model(g, pq.intervals)) << "trial=" << trial;
        }
    }
}

}  // namespace
