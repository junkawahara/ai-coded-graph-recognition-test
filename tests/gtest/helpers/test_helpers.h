#ifndef GRAPH_RECOGNITION_GTEST_TEST_HELPERS_H
#define GRAPH_RECOGNITION_GTEST_TEST_HELPERS_H

#include "util/graph.h"
#include <string>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace gtest_utils {

struct DirectedInput {
    int n = 0;
    std::vector<std::pair<int, int>> arcs;
};

Graph load_graph(const std::string& path);

DirectedInput load_arcs(const std::string& path);

std::string read_expected(const std::string& path);

int read_n(const std::string& path);

std::vector<std::string> list_in_files(const std::string& dir);

/**
 * Canonical form of a labeled graph under isomorphism: the lexicographically
 * smallest normalized (u < v, sorted) edge list over all vertex permutations.
 * Brute force over n! permutations -- intended for n <= 8.
 */
std::vector<std::pair<int, int>> canonical_edge_list(
    int n, const std::vector<std::pair<int, int>>& edges);

/**
 * Canonical form of a labeled digraph under isomorphism: the
 * lexicographically smallest sorted arc list over all vertex permutations
 * (arcs keep their direction, so there is no u < v normalization).
 * Brute force over n! permutations -- intended for n <= 8.
 */
std::vector<std::pair<int, int>> canonical_arc_list(
    int n, const std::vector<std::pair<int, int>>& arcs);

inline std::string test_path(const std::string& rel) {
    return std::string(TESTS_DATA_DIR) + "/" + rel;
}

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
