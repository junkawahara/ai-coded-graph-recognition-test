#include "recognizers/cactus.h"
#include "enumerators/cactus_labeled_enum.h"
#include "enum_property_helpers.h"
#include "util/graph.h"
#include <gtest/gtest.h>

namespace {

using graph_recognition::Graph;
using graph_recognition::gtest_utils::expect_enum_equals_bruteforce;

// Method 1 brute-forces ALL 2^(n choose 2) labeled graphs through the
// recognizer (no other enumerator in the loop) and Method 2 is the direct
// enumeration; the two are compared as sets of edge lists, so duplicates,
// omissions, and invalid graphs are all caught individually.
TEST(CactusLabeledEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        auto enum_res = graph_recognition::enumerate_cactus_labeled_graphs_reverse_search(n);
        expect_enum_equals_bruteforce(
            n,
            [](const Graph& g) {
                return graph_recognition::check_cactus(g).is_cactus;
            },
            enum_res.graphs, "cactus");
    }
}

}  // namespace
