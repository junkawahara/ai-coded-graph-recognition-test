#ifndef GRAPH_RECOGNITION_GTEST_CERTIFICATES_H
#define GRAPH_RECOGNITION_GTEST_CERTIFICATES_H

#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

struct ChordalResult;

namespace gtest_utils {

bool verify_interval_model(const Graph& g,
                           const std::vector<std::pair<int, int>>& intervals);

bool verify_bipartite_coloring(const Graph& g, const std::vector<int>& color);

bool verify_chordal_peo(const Graph& g, const ChordalResult& r);

bool verify_circle_dow(const Graph& g, const std::vector<int>& dow);

bool verify_convex_bipartite(const Graph& g, const std::vector<int>& color,
                             const std::vector<int>& ordering);

bool verify_biconvex_bipartite(const Graph& g, const std::vector<int>& color,
                               const std::vector<int>& x_ordering,
                               const std::vector<int>& y_ordering);

bool verify_kregular_k(const Graph& g, int k);

bool verify_strongly_regular_params(const Graph& g, int k, int lambda, int mu);

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
