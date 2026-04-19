#ifndef GRAPH_RECOGNITION_GTEST_RANDOM_GRAPHS_H
#define GRAPH_RECOGNITION_GTEST_RANDOM_GRAPHS_H

#include <random>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace gtest_utils {

std::vector<std::pair<int, int>> random_er(std::mt19937& rng, int n, double p);

std::vector<std::pair<int, int>> random_tree(std::mt19937& rng, int n);

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
