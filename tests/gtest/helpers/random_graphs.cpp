#include "random_graphs.h"

namespace graph_recognition {
namespace gtest_utils {

std::vector<std::pair<int, int>> random_er(std::mt19937& rng, int n, double p) {
    std::vector<std::pair<int, int>> edges;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (dist(rng) < p) {
                edges.push_back(std::make_pair(u, v));
            }
        }
    }
    return edges;
}

std::vector<std::pair<int, int>> random_tree(std::mt19937& rng, int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 2; v <= n; ++v) {
        std::uniform_int_distribution<int> dist(1, v - 1);
        int u = dist(rng);
        edges.push_back(std::make_pair(u, v));
    }
    return edges;
}

}  // namespace gtest_utils
}  // namespace graph_recognition
