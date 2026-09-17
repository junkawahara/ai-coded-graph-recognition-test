// Self-tests of the certificate verifiers in helpers/certificates.cpp.
//
// The recognizer tests only ever feed the verifiers genuine library output,
// so a verifier that accepts too much never fails there. Each test below
// forges a certificate that is wrong in exactly one way the verifier used to
// let through (review_all_1.md items 23, 25, 34) and asserts rejection, next
// to a correct certificate of the same shape that must still be accepted.

#include "certificates.h"
#include "certificates/forbidden_subgraph.h"
#include "decompositions/md_tree.h"
#include "decompositions/tree_decomposition.h"
#include "recognizers/series_parallel.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::MDNode;
using graph_recognition::MDNodeKind;
using graph_recognition::MDTree;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::SPReduction;
using graph_recognition::TreeDecompositionResult;
using graph_recognition::make_obstruction;
using graph_recognition::md_finalize;
using graph_recognition::gtest_utils::verify_md_tree;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::gtest_utils::verify_obstruction_for;
using graph_recognition::gtest_utils::verify_sp_reduction;
using graph_recognition::gtest_utils::verify_tree_decomposition;

namespace {

typedef std::vector<std::pair<int, int>> Edges;

Graph path(int n) {
    Edges e;
    for (int i = 1; i < n; ++i) e.push_back(std::make_pair(i, i + 1));
    return Graph(n, e);
}

Graph cycle(int n) {
    Edges e;
    for (int i = 1; i < n; ++i) e.push_back(std::make_pair(i, i + 1));
    e.push_back(std::make_pair(n, 1));
    return Graph(n, e);
}

Graph complete(int n) {
    Edges e;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) e.push_back(std::make_pair(u, v));
    }
    return Graph(n, e);
}

std::vector<int> range(int n) {
    std::vector<int> r;
    for (int i = 1; i <= n; ++i) r.push_back(i);
    return r;
}

// ---- item 23 (a): tree decomposition bags -------------------------------

TreeDecompositionResult single_bag(const std::vector<int>& bag, int width) {
    TreeDecompositionResult r;
    r.success = true;
    r.bags.push_back(bag);
    r.tree.push_back(std::vector<int>());
    r.width = width;
    return r;
}

TEST(CertificateVerifierTest, TreeDecompositionAcceptsTheK1Bag) {
    Graph k1(1, Edges());
    EXPECT_TRUE(verify_tree_decomposition(k1, single_bag(std::vector<int>(1, 1), 0)));
}

TEST(CertificateVerifierTest, TreeDecompositionRejectsOutOfRangeBagVertex) {
    Graph k1(1, Edges());
    std::vector<int> bag;
    bag.push_back(1);
    bag.push_back(2);  // no such vertex
    EXPECT_FALSE(verify_tree_decomposition(k1, single_bag(bag, 1)));
}

TEST(CertificateVerifierTest, TreeDecompositionRejectsRepeatedBagVertex) {
    Graph k1(1, Edges());
    std::vector<int> bag(2, 1);  // {1, 1}: width 1 is forged from a repeat
    EXPECT_FALSE(verify_tree_decomposition(k1, single_bag(bag, 1)));
}

// ---- item 23 (b): series-parallel reduction steps -----------------------

SPReduction step(int vertex, int kind, int u, int w) {
    SPReduction s;
    s.vertex = vertex;
    s.kind = kind;
    s.u = u;
    s.w = w;
    return s;
}

TEST(CertificateVerifierTest, SpReductionAcceptsAPendantPeeling) {
    Graph p3 = path(3);
    std::vector<SPReduction> steps;
    steps.push_back(step(1, 1, 2, 0));
    steps.push_back(step(3, 1, 2, 0));
    steps.push_back(step(2, 0, 0, 0));
    EXPECT_TRUE(verify_sp_reduction(p3, steps));
}

TEST(CertificateVerifierTest, SpReductionAcceptsASeriesStep) {
    Graph p3 = path(3);
    std::vector<SPReduction> steps;
    steps.push_back(step(2, 2, 1, 3));
    steps.push_back(step(1, 1, 3, 0));
    steps.push_back(step(3, 0, 0, 0));
    EXPECT_TRUE(verify_sp_reduction(p3, steps));
}

TEST(CertificateVerifierTest, SpReductionRejectsASeriesStepNamingOneNeighbourTwice) {
    // The forged replay from the review: vertex 2 of P3 claims both of its
    // neighbours are vertex 1. Before the fix that inserted a self-loop at 1
    // and left 3 pointing at the dead vertex 2, and the remaining two pendant
    // steps then "succeeded" against that corrupted adjacency.
    Graph p3 = path(3);
    std::vector<SPReduction> steps;
    steps.push_back(step(2, 2, 1, 1));
    steps.push_back(step(3, 1, 2, 0));
    steps.push_back(step(1, 1, 1, 0));
    EXPECT_FALSE(verify_sp_reduction(p3, steps));
}

TEST(CertificateVerifierTest, SpReductionRejectsAParallelStepNamingOneNeighbourTwice) {
    Graph k3 = complete(3);
    std::vector<SPReduction> steps;
    steps.push_back(step(3, 3, 1, 1));
    steps.push_back(step(2, 1, 1, 0));
    steps.push_back(step(1, 0, 0, 0));
    EXPECT_FALSE(verify_sp_reduction(k3, steps));
}

TEST(CertificateVerifierTest, SpReductionRejectsAPendantStepNamingADeadVertex) {
    Graph p3 = path(3);
    std::vector<SPReduction> steps;
    steps.push_back(step(1, 1, 2, 0));
    steps.push_back(step(2, 1, 3, 0));
    steps.push_back(step(3, 1, 2, 0));  // 2 is gone; 3 is isolated by now
    EXPECT_FALSE(verify_sp_reduction(p3, steps));
}

// ---- item 23 (c): PRIME nodes whose quotient has a module ---------------

// A root of the given kind with one leaf child per vertex of g.
MDTree flat_tree(const Graph& g, MDNodeKind root_kind) {
    MDTree t;
    MDNode root;
    root.kind = root_kind;
    t.nodes.push_back(root);
    t.root = 0;
    for (int v = 1; v <= g.n; ++v) {
        MDNode leaf;
        leaf.kind = MDNodeKind::LEAF;
        leaf.vertex = v;
        leaf.parent = 0;
        t.nodes[0].children.push_back((int)t.nodes.size());
        t.nodes.push_back(leaf);
    }
    md_finalize(t, g);
    return t;
}

TEST(CertificateVerifierTest, MdTreeAcceptsAGenuinePrimeRoot) {
    // P4 and C5 are prime, so one PRIME node over the leaves is their tree.
    EXPECT_TRUE(verify_md_tree(path(4), flat_tree(path(4), MDNodeKind::PRIME), false));
    EXPECT_TRUE(verify_md_tree(cycle(5), flat_tree(cycle(5), MDNodeKind::PRIME), false));
}

TEST(CertificateVerifierTest, MdTreeAcceptsTheP3Cotree) {
    // P3 = 2 joined to the union {1, 3}: SERIES over (PARALLEL(1, 3), 2).
    Graph p3 = path(3);
    MDTree t;
    MDNode series, parallel, l1, l3, l2;
    series.kind = MDNodeKind::SERIES;
    parallel.kind = MDNodeKind::PARALLEL;
    parallel.parent = 0;
    l1.kind = l3.kind = l2.kind = MDNodeKind::LEAF;
    l1.vertex = 1;
    l3.vertex = 3;
    l2.vertex = 2;
    l1.parent = l3.parent = 1;
    l2.parent = 0;
    series.children.push_back(1);
    series.children.push_back(4);
    parallel.children.push_back(2);
    parallel.children.push_back(3);
    t.nodes.push_back(series);
    t.nodes.push_back(parallel);
    t.nodes.push_back(l1);
    t.nodes.push_back(l3);
    t.nodes.push_back(l2);
    t.root = 0;
    md_finalize(t, p3);
    EXPECT_TRUE(verify_md_tree(p3, t, false));
    EXPECT_TRUE(verify_md_tree(p3, t, true));
}

TEST(CertificateVerifierTest, MdTreeRejectsAPrimeRootOverP3) {
    // {1, 3} is a module of P3, so its quotient on three singletons is not
    // prime although it is neither complete nor edgeless.
    EXPECT_FALSE(verify_md_tree(path(3), flat_tree(path(3), MDNodeKind::PRIME), false));
}

TEST(CertificateVerifierTest, MdTreeRejectsAPrimeRootHidingATwinPair) {
    // P4 plus a false twin of vertex 1: {1, 5} is a non-trivial module, and
    // the flat quotient has 4 edges out of 10 pairs.
    Edges e;
    e.push_back(std::make_pair(1, 2));
    e.push_back(std::make_pair(2, 3));
    e.push_back(std::make_pair(3, 4));
    e.push_back(std::make_pair(5, 2));
    Graph g(5, e);
    EXPECT_FALSE(verify_md_tree(g, flat_tree(g, MDNodeKind::PRIME), false));
}

// ---- item 25: NO certificates tied to the class -------------------------

TEST(CertificateVerifierTest, ClassRuleRejectsC4AsAChordalBipartiteWitness) {
    Graph c4 = cycle(4);
    Obstruction o = make_obstruction(ObstructionKind::EVEN_HOLE, range(4));
    EXPECT_TRUE(verify_obstruction(c4, o));  // a valid even hole as such
    EXPECT_FALSE(verify_obstruction_for("chordal_bipartite", c4, o));
    EXPECT_TRUE(verify_obstruction_for("even_hole_free", c4, o));
}

TEST(CertificateVerifierTest, ClassRuleAcceptsC6AsAChordalBipartiteWitness) {
    Graph c6 = cycle(6);
    Obstruction o = make_obstruction(ObstructionKind::EVEN_HOLE, range(6));
    EXPECT_TRUE(verify_obstruction_for("chordal_bipartite", c6, o));
}

TEST(CertificateVerifierTest, ClassRuleRejectsAKindTheClassDoesNotDocument) {
    Graph c5 = cycle(5);
    Obstruction hole = make_obstruction(ObstructionKind::HOLE, range(5));
    EXPECT_TRUE(verify_obstruction_for("chordal", c5, hole));
    EXPECT_FALSE(verify_obstruction_for("bipartite", c5, hole));  // wants ODD_CYCLE
    Obstruction odd = make_obstruction(ObstructionKind::ODD_CYCLE, range(5));
    EXPECT_TRUE(verify_obstruction_for("bipartite", c5, odd));
    EXPECT_FALSE(verify_obstruction_for("chordal", c5, odd));
}

TEST(CertificateVerifierTest, ClassRuleChecksTheComplementSide) {
    // C5 is self-complementary, so the same hole is valid on both sides of
    // the graph; only the class contract tells them apart.
    Graph c5 = cycle(5);
    Obstruction in_g = make_obstruction(ObstructionKind::HOLE, range(5), false);
    // The complement of the 5-cycle 1-2-3-4-5 is the 5-cycle 1-3-5-2-4.
    std::vector<int> co_order;
    co_order.push_back(1); co_order.push_back(3); co_order.push_back(5);
    co_order.push_back(2); co_order.push_back(4);
    Obstruction in_co = make_obstruction(ObstructionKind::HOLE, co_order, true);
    EXPECT_TRUE(verify_obstruction(c5, in_co));
    EXPECT_FALSE(verify_obstruction_for("chordal", c5, in_co));
    EXPECT_FALSE(verify_obstruction_for("co_chordal", c5, in_g));
    EXPECT_TRUE(verify_obstruction_for("co_chordal", c5, in_co));
    EXPECT_TRUE(verify_obstruction_for("weakly_chordal", c5, in_g));
    EXPECT_TRUE(verify_obstruction_for("weakly_chordal", c5, in_co));
}

TEST(CertificateVerifierTest, ClassRuleRejectsAnUnknownClass) {
    Graph c5 = cycle(5);
    Obstruction hole = make_obstruction(ObstructionKind::HOLE, range(5));
    EXPECT_FALSE(verify_obstruction_for("no_such_class", c5, hole));
}

// ---- item 34 (mirror of the Python check): ODD_CYCLE may have chords ----

TEST(CertificateVerifierTest, OddCycleAcceptsChordsButOtherCyclesDoNot) {
    Graph k5 = complete(5);
    EXPECT_TRUE(verify_obstruction(k5, make_obstruction(ObstructionKind::ODD_CYCLE, range(5))));
    EXPECT_TRUE(verify_obstruction_for("bipartite", k5,
                                       make_obstruction(ObstructionKind::ODD_CYCLE, range(5))));
    EXPECT_FALSE(verify_obstruction(k5, make_obstruction(ObstructionKind::ODD_HOLE, range(5))));
    EXPECT_FALSE(verify_obstruction(k5, make_obstruction(ObstructionKind::HOLE, range(5))));
    EXPECT_FALSE(verify_obstruction(
        k5, make_obstruction(ObstructionKind::ODD_CYCLE_LE1_CHORD, range(5))));
    // An even closed walk is never an odd cycle, chords or not.
    Graph k4 = complete(4);
    EXPECT_FALSE(verify_obstruction(k4, make_obstruction(ObstructionKind::ODD_CYCLE, range(4))));
}

}  // namespace
