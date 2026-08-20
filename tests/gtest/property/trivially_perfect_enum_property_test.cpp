#include "trivially_perfect.h"
#include "trivially_perfect_enum.h"
#include "enum_property_helpers.h"
#include "graph.h"
#include <gtest/gtest.h>

namespace {

using graph_recognition::Graph;
using graph_recognition::gtest_utils::expect_enum_equals_bruteforce;

// Method 1 brute-forces ALL 2^(n choose 2) labeled graphs through the
// recognizer (no other enumerator in the loop) and Method 2 is the direct
// enumeration; the two are compared as sets of edge lists, so duplicates,
// omissions, and invalid graphs are all caught individually.
TEST(TriviallyPerfectEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 5;

    for (int n = 1; n <= max_n; ++n) {
        auto enum_res = graph_recognition::enumerate_trivially_perfect_graphs_uvd(n);
        expect_enum_equals_bruteforce(
            n,
            [](const Graph& g) {
                return graph_recognition::check_trivially_perfect(g).is_trivially_perfect;
            },
            enum_res.graphs, "trivially_perfect");
    }
}

}  // namespace
