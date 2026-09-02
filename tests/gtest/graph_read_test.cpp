#include "util/graph.h"
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

TEST(GraphReadTest, HugeVertexCountFails) {
    // n = INT_MAX previously signed-overflowed Graph's adj(n + 1) into an
    // uncaught std::length_error; moderate bogus n allocated gigabytes.
    std::istringstream in1("2147483647 0\n");
    bool ok = true;
    Graph g1 = Graph::read(in1, ok);
    EXPECT_FALSE(ok);
    EXPECT_EQ(g1.n, 0);

    std::istringstream in2("100000000 0\n");
    ok = true;
    Graph g2 = Graph::read(in2, ok);
    EXPECT_FALSE(ok);
    EXPECT_EQ(g2.n, 0);

    std::istringstream in3("1000000 0\n");  // the documented maximum passes
    ok = false;
    Graph g3 = Graph::read(in3, ok);
    EXPECT_TRUE(ok);
    EXPECT_EQ(g3.n, Graph::max_read_vertices);
}

TEST(GraphReadTest, OutOfRangeEdgeFails) {
    // A 0-indexed edge list must not silently answer about a different
    // graph: the constructor drops the edge, so read reports the input bad.
    std::istringstream in("5 4\n0 1\n1 2\n2 3\n3 4\n");
    bool ok = true;
    Graph g = Graph::read(in, ok);
    EXPECT_FALSE(ok);
    EXPECT_EQ(g.n, 5);
    EXPECT_FALSE(g.has_edge(0, 1));

    std::istringstream in2("3 1\n1 4\n");
    ok = true;
    Graph::read(in2, ok);
    EXPECT_FALSE(ok);
}

TEST(GraphReadTest, SelfLoopFails) {
    std::istringstream in("3 2\n1 2\n2 2\n");
    bool ok = true;
    Graph::read(in, ok);
    EXPECT_FALSE(ok);
}

TEST(GraphReadTest, DuplicateEdgeIsStillValid) {
    // Duplicates describe the same simple graph, so they are not an error.
    std::istringstream in("3 3\n1 2\n1 2\n2 3\n");
    bool ok = false;
    Graph g = Graph::read(in, ok);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(g.has_edge(1, 2));
    EXPECT_EQ(g.adj[1].size(), 1u);
}

TEST(GraphConstructorTest, NegativeVertexCountIsTheEmptyGraph) {
    // A negative count would leave adj/adj_set empty while n stayed negative,
    // and the search primitives write to bucket/label arrays sized from n
    // without rechecking it (mcs_bucket's bucket_head[0] = 1 on an empty
    // vector was an out-of-bounds write).
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    Graph g(-1, edges);
    EXPECT_EQ(g.n, 0);
    EXPECT_EQ(g.adj.size(), 1u);
    EXPECT_EQ(g.adj_set.size(), 1u);
    EXPECT_FALSE(g.has_edge(1, 2));
}

}  // namespace
