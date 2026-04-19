#ifndef GRAPH_RECOGNITION_GTEST_TEST_HELPERS_H
#define GRAPH_RECOGNITION_GTEST_TEST_HELPERS_H

#include "graph.h"
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

inline std::string test_path(const std::string& rel) {
    return std::string(TESTS_DATA_DIR) + "/" + rel;
}

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
