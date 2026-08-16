#include "graph.h"
#include <gtest/gtest.h>

#include <sstream>
#include <string>

using graph_recognition::Graph;

namespace {

TEST(GraphReadTest, ValidInputSetsOk) {
    std::istringstream in("3 2\n1 2\n2 3\n");
    bool ok = false;
    Graph g = Graph::read(in, ok);
    EXPECT_TRUE(ok);
    EXPECT_EQ(g.n, 3);
    EXPECT_TRUE(g.has_edge(1, 2));
    EXPECT_TRUE(g.has_edge(2, 3));
    EXPECT_FALSE(g.has_edge(1, 3));
}

TEST(GraphReadTest, EmptyGraphIsValid) {
    std::istringstream in("0 0\n");
    bool ok = false;
    Graph g = Graph::read(in, ok);
    EXPECT_TRUE(ok);
    EXPECT_EQ(g.n, 0);
}

TEST(GraphReadTest, MissingHeaderFailsButLegacyOverloadReturnsEmpty) {
    std::istringstream in1("");
    bool ok = true;
    Graph g = Graph::read(in1, ok);
    EXPECT_FALSE(ok);
    EXPECT_EQ(g.n, 0);

    std::istringstream in2("");
    Graph g2 = Graph::read(in2);
    EXPECT_EQ(g2.n, 0);
}

TEST(GraphReadTest, NonNumericHeaderFails) {
    std::istringstream in("abc def\n");
    bool ok = true;
    Graph::read(in, ok);
    EXPECT_FALSE(ok);
}

TEST(GraphReadTest, NegativeCountsFail) {
    std::istringstream in1("-1 0\n");
    bool ok = true;
    Graph::read(in1, ok);
    EXPECT_FALSE(ok);

    std::istringstream in2("3 -1\n");
    ok = true;
    Graph::read(in2, ok);
    EXPECT_FALSE(ok);
}

TEST(GraphReadTest, TruncatedEdgeListFails) {
    std::istringstream in("4 3\n1 2\n2 3\n");
    bool ok = true;
    Graph g = Graph::read(in, ok);
    EXPECT_FALSE(ok);
    // The edges read before truncation are still returned.
    EXPECT_EQ(g.n, 4);
    EXPECT_TRUE(g.has_edge(1, 2));
    EXPECT_TRUE(g.has_edge(2, 3));
}

TEST(GraphReadTest, HugeEdgeCountDoesNotPreallocate) {
    // A bogus header (m = INT_MAX) must not request gigabytes up front;
    // the read fails at the first missing edge line instead.
    std::istringstream in("2 2147483647\n");
    bool ok = true;
    Graph g = Graph::read(in, ok);
    EXPECT_FALSE(ok);
    EXPECT_EQ(g.n, 2);
}

}  // namespace
