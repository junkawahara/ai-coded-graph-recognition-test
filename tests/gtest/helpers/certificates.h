#ifndef GRAPH_RECOGNITION_GTEST_CERTIFICATES_H
#define GRAPH_RECOGNITION_GTEST_CERTIFICATES_H

#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {
namespace gtest_utils {

bool verify_interval_model(const Graph& g,
                           const std::vector<std::pair<int, int>>& intervals);

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
