#ifndef GRAPH_RECOGNITION_TESTS_ENUM_PROPERTY_HELPERS_H
#define GRAPH_RECOGNITION_TESTS_ENUM_PROPERTY_HELPERS_H

#include "chordal_labeled_enum.h"  // EnumeratedGraph
#include "graph.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace gtest_utils {

/** @brief Normalizes an edge list to u < v, sorted */
inline std::vector<std::pair<int, int>> canonical_edges(
    std::vector<std::pair<int, int>> edges) {
    for (size_t i = 0; i < edges.size(); ++i) {
        if (edges[i].first > edges[i].second) {
            std::swap(edges[i].first, edges[i].second);
        }
    }
    std::sort(edges.begin(), edges.end());
    return edges;
}

/**
 * @brief Compares an enumeration against a brute-force filter, as sets
 *
 * Brute-forces all 2^C(n,2) labeled graphs on {1..n}, keeps those accepted
 * by `filter`, and requires `enumerated` to be exactly that SET of graphs.
 * Comparing sets instead of counts rejects a duplicate paired with an
 * omission (which leaves the count unchanged), and depending only on the
 * recognizer keeps any other enumerator out of the loop.
 */
template <typename FilterFn>
void expect_enum_equals_bruteforce(int n, FilterFn filter,
                                   const std::vector<EnumeratedGraph>& enumerated,
                                   const char* label) {
    int num_edges = n * (n - 1) / 2;
    std::vector<std::pair<int, int>> all_edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v)
            all_edges.push_back(std::make_pair(u, v));

    std::set<std::vector<std::pair<int, int>>> by_filter;
    for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < num_edges; ++i)
            if (mask & (1LL << i)) edges.push_back(all_edges[i]);
        Graph g(n, edges);
        if (filter(g)) by_filter.insert(edges);  // already canonical
    }

    std::set<std::vector<std::pair<int, int>>> by_enum;
    for (size_t gi = 0; gi < enumerated.size(); ++gi) {
        EXPECT_EQ(enumerated[gi].n, n)
            << label << ": wrong n on graph#" << gi;
        std::vector<std::pair<int, int>> key = canonical_edges(enumerated[gi].edges);
        EXPECT_TRUE(by_enum.insert(key).second)
            << label << ": duplicate enumerated graph, n=" << n
            << " graph#" << gi;
    }

    int missing = 0, extra = 0;
    for (std::set<std::vector<std::pair<int, int>>>::const_iterator it =
             by_filter.begin(); it != by_filter.end(); ++it)
        if (!by_enum.count(*it)) ++missing;
    for (std::set<std::vector<std::pair<int, int>>>::const_iterator it =
             by_enum.begin(); it != by_enum.end(); ++it)
        if (!by_filter.count(*it)) ++extra;
    EXPECT_EQ(missing, 0)
        << label << ": graphs accepted by the recognizer but not enumerated,"
        << " n=" << n << " (filter=" << by_filter.size()
        << ", enum=" << by_enum.size() << ")";
    EXPECT_EQ(extra, 0)
        << label << ": enumerated graphs rejected by the recognizer, n=" << n;
}

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
