// The four recognizers whose search recurses once per edge / clique / vertex
// used to overflow the default 8 MiB stack on large sparse inputs (SIGSEGV,
// which no exception boundary can report). They now refuse such inputs with
// std::runtime_error at a documented limit (the stack-overflow exception to
// the complexity policy in CLAUDE.md); see review_all_1.md #3-#6.

#include "recognizers/circular_arc.h"
#include "recognizers/even_hole_free.h"
#include "recognizers/line_graph.h"
#include "recognizers/meyniel.h"
#include "recognizers/proper_circular_arc.h"
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

Graph make_cycle(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(n, 1));
    return Graph(n, edges);
}

std::string message_of(void (*body)()) {
    try {
        body();
    } catch (const std::runtime_error& e) {
        return e.what();
    }
    return "";
}

// --- line_graph: depth = m ---

TEST(RecursionLimitTest, LineGraphRefusesTheLimitEdgeCount) {
    // A path with exactly line_graph_recursion_limit edges is a line graph
    // (of a longer path), but the search is refused.
    Graph g = make_path(line_graph_recursion_limit + 1);
    EXPECT_THROW(check_line_graph(g, LineGraphAlgorithm::BRUTE), std::runtime_error);
    EXPECT_THROW(check_line_graph(g, LineGraphAlgorithm::KRAUSZ), std::runtime_error);
    std::string msg = message_of([]() {
        check_line_graph(make_path(line_graph_recursion_limit + 1));
    });
    EXPECT_NE(msg.find("recursion limit"), std::string::npos) << msg;
    EXPECT_NE(msg.find("overflow"), std::string::npos) << msg;
}

TEST(RecursionLimitTest, LineGraphAnswersJustBelowTheLimit) {
    Graph g = make_path(line_graph_recursion_limit);  // m = limit - 1
    LineGraphResult r = check_line_graph(g);
    EXPECT_TRUE(r.is_line_graph);
    EXPECT_EQ(r.root_graph.n, line_graph_recursion_limit + 1);
}

// --- circular_arc: depth = number of maximal cliques (MCCONNELL) or n (BACKTRACKING) ---

TEST(RecursionLimitTest, CircularArcRefusesTheLimitCliqueCount) {
    // P_{k+1} has k maximal cliques (its edges).
    Graph g = make_path(circular_arc_recursion_limit + 1);
    EXPECT_THROW(check_circular_arc(g), std::runtime_error);
    EXPECT_THROW(check_circular_arc(g, CircularArcAlgorithm::BACKTRACKING), std::runtime_error);
    // proper_circular_arc runs the same endpoint search after its claw-free
    // filter, which a path passes.
    EXPECT_THROW(check_proper_circular_arc(g), std::runtime_error);
    std::string msg = message_of([]() {
        check_circular_arc(make_path(circular_arc_recursion_limit + 1));
    });
    EXPECT_NE(msg.find("maximal cliques"), std::string::npos) << msg;
}

TEST(RecursionLimitTest, CircularArcAnswersJustBelowTheLimit) {
    Graph g = make_path(circular_arc_recursion_limit);  // limit - 1 cliques
    EXPECT_TRUE(check_circular_arc(g).is_circular_arc);
    // A cycle has n maximal cliques for n >= 4, so C_{limit-1} is fine too.
    EXPECT_TRUE(check_circular_arc(make_cycle(circular_arc_recursion_limit - 1)).is_circular_arc);
}

// --- meyniel: depth <= n ---

TEST(RecursionLimitTest, MeynielRefusesTheLimitVertexCount) {
    Graph g = make_path(meyniel_recursion_limit);
    EXPECT_THROW(check_meyniel(g), std::runtime_error);
    std::string msg = message_of([]() { check_meyniel(make_path(meyniel_recursion_limit)); });
    EXPECT_NE(msg.find("meyniel"), std::string::npos) << msg;
    EXPECT_NE(msg.find("overflow"), std::string::npos) << msg;
}

TEST(RecursionLimitTest, MeynielAnswersJustBelowTheLimit) {
    // A star is decided quickly (no long paths) and is Meyniel.
    const int n = meyniel_recursion_limit - 1;
    std::vector<std::pair<int, int>> edges;
    for (int v = 2; v <= n; ++v) edges.push_back(std::make_pair(1, v));
    EXPECT_TRUE(check_meyniel(Graph(n, edges)).is_meyniel);
}

// --- even_hole_free: depth <= n ---

TEST(RecursionLimitTest, EvenHoleFreeRefusesTheLimitVertexCount) {
    Graph g = make_path(even_hole_free_recursion_limit);
    EXPECT_THROW(check_even_hole_free(g), std::runtime_error);
    std::string msg = message_of([]() {
        check_even_hole_free(make_path(even_hole_free_recursion_limit));
    });
    EXPECT_NE(msg.find("even_hole_free"), std::string::npos) << msg;
}

TEST(RecursionLimitTest, EvenHoleFreeAnswersJustBelowTheLimit) {
    const int n = even_hole_free_recursion_limit - 1;
    std::vector<std::pair<int, int>> edges;
    for (int v = 2; v <= n; ++v) edges.push_back(std::make_pair(1, v));
    EXPECT_TRUE(check_even_hole_free(Graph(n, edges)).is_even_hole_free);
}

}  // namespace
