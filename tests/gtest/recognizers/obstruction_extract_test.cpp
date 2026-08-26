#include "certificates/obstruction_extract.h"

#include "bf_oracles.h"
#include "certificates.h"
#include "recognizers/cactus.h"
#include "recognizers/comparability.h"
#include "recognizers/distance_hereditary.h"
#include "recognizers/planar.h"
#include "recognizers/ptolemaic.h"
#include "util/graph_utils.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_complement;
using graph_recognition::gtest_utils::bf_has_induced_cycle_ge;
using graph_recognition::gtest_utils::verify_obstruction;

namespace ext = graph_recognition::detail_obstruction;

namespace {

Graph make_cycle(int k) {
    std::vector<std::pair<int, int> > edges;
    for (int i = 1; i <= k; ++i) edges.push_back(std::make_pair(i, i % k + 1));
    return Graph(k, edges);
}

Obstruction hole_obstruction(const std::vector<int>& cycle) {
    return graph_recognition::make_obstruction(ObstructionKind::HOLE, cycle);
}

// All graphs on n vertices, as edge masks over the lexicographic pair list.
std::vector<std::pair<int, int> > all_pairs(int n) {
    std::vector<std::pair<int, int> > pairs;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) pairs.push_back(std::make_pair(u, v));
    }
    return pairs;
}

Graph graph_from_mask(int n, const std::vector<std::pair<int, int> >& pairs, int mask) {
    std::vector<std::pair<int, int> > edges;
    for (size_t i = 0; i < pairs.size(); ++i) {
        if (mask & (1 << i)) edges.push_back(pairs[i]);
    }
    return Graph(n, edges);
}

std::vector<std::vector<bool> > adj_matrix(const Graph& g) {
    std::vector<std::vector<bool> > a(g.n + 1, std::vector<bool>(g.n + 1, false));
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) a[u][g.adj[u][i]] = true;
    }
    return a;
}

bool bf_has_induced_p4(const Graph& g) {
    for (int a = 1; a <= g.n; ++a) {
        for (int b = 1; b <= g.n; ++b) {
            for (int c = 1; c <= g.n; ++c) {
                for (int d = 1; d <= g.n; ++d) {
                    if (a == b || a == c || a == d || b == c || b == d || c == d) continue;
                    if (!g.has_edge(a, b) || !g.has_edge(b, c) || !g.has_edge(c, d)) continue;
                    if (g.has_edge(a, c) || g.has_edge(a, d) || g.has_edge(b, d)) continue;
                    return true;
                }
            }
        }
    }
    return false;
}

TEST(ObstructionExtract, HoleThroughCenterReturnsTheCycle) {
    Graph g = make_cycle(5);
    std::vector<int> hole = ext::hole_through_center(g, 1, 2, 5);
    ASSERT_EQ(hole.size(), 5u);
    EXPECT_TRUE(verify_obstruction(g, hole_obstruction(hole)));
}

TEST(ObstructionExtract, HoleThroughCenterSkipsTheChordedSide) {
    // Two 4-cycles glued at vertex 1; the pair (2,5) is separated by N[1] only
    // through the other cycle, so the extraction must pick the right component.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    edges.push_back(std::make_pair(4, 1));
    edges.push_back(std::make_pair(1, 5));
    edges.push_back(std::make_pair(5, 6));
    edges.push_back(std::make_pair(6, 7));
    edges.push_back(std::make_pair(7, 1));
    Graph g(7, edges);

    std::vector<int> hole = ext::hole_through_center(g, 1, 2, 4);
    ASSERT_EQ(hole.size(), 4u);
    EXPECT_TRUE(verify_obstruction(g, hole_obstruction(hole)));

    // 2 and 5 lie in different components of G - N[1], so no hole runs through
    // both of them.
    EXPECT_TRUE(ext::hole_through_center(g, 1, 2, 5).empty());
}

TEST(ObstructionExtract, HoleThroughCenterRejectsIllFormedTriples) {
    Graph c4 = make_cycle(4);
    // A repeated neighbor bounds nothing.
    EXPECT_TRUE(ext::hole_through_center(c4, 1, 2, 2).empty());
    // A non-neighbor of the center cannot start a hole through it.
    EXPECT_TRUE(ext::hole_through_center(c4, 1, 2, 3).empty());

    // Adjacent neighbors close a triangle, not a hole.
    std::vector<std::pair<int, int> > tri;
    tri.push_back(std::make_pair(1, 2));
    tri.push_back(std::make_pair(2, 3));
    tri.push_back(std::make_pair(1, 3));
    Graph triangle(3, tri);
    EXPECT_TRUE(ext::hole_through_center(triangle, 1, 2, 3).empty());

    // The genuine C4 triple does yield the hole.
    std::vector<int> hole = ext::hole_through_center(c4, 2, 1, 3);
    ASSERT_EQ(hole.size(), 4u);
    EXPECT_TRUE(verify_obstruction(c4, hole_obstruction(hole)));
}

TEST(ObstructionExtract, FindHoleAgreesWithBruteForceOnAllSixVertexGraphs) {
    const int n = 6;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        std::vector<int> hole = ext::find_hole(g);
        bool bf = bf_has_induced_cycle_ge(n, adj_matrix(g), 4);
        ASSERT_EQ(!hole.empty(), bf) << "mask=" << mask;
        if (!hole.empty()) {
            ASSERT_TRUE(verify_obstruction(g, hole_obstruction(hole))) << "mask=" << mask;
        }
    }
}

TEST(ObstructionExtract, OddCycleFromConflictClosesThroughTheMeetingPoint) {
    Graph g = make_cycle(5);
    // BFS from 1: 2 and 5 are children of 1, 3 a child of 2, 4 a child of 5,
    // and the edge 3-4 closes the cycle with both endpoints at even depth.
    std::vector<int> parent(6, 0);
    parent[2] = 1;
    parent[5] = 1;
    parent[3] = 2;
    parent[4] = 5;

    std::vector<int> cyc = ext::odd_cycle_from_conflict(parent, 3, 4);
    ASSERT_EQ(cyc.size(), 5u);
    Obstruction o = graph_recognition::make_obstruction(ObstructionKind::ODD_CYCLE, cyc);
    EXPECT_TRUE(verify_obstruction(g, o));
}

TEST(ObstructionExtract, OddCycleFromConflictHandlesAnAncestorEndpoint) {
    // Triangle 1-2-3 with the BFS tree 1 -> 2 -> 3; the conflict edge is 1-3.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(1, 3));
    Graph g(3, edges);

    std::vector<int> parent(4, 0);
    parent[2] = 1;
    parent[3] = 2;

    std::vector<int> cyc = ext::odd_cycle_from_conflict(parent, 3, 1);
    ASSERT_EQ(cyc.size(), 3u);
    Obstruction o = graph_recognition::make_obstruction(ObstructionKind::ODD_CYCLE, cyc);
    EXPECT_TRUE(verify_obstruction(g, o));
}

TEST(ObstructionExtract, FindInducedP4AgreesWithBruteForceOnAllFiveVertexGraphs) {
    const int n = 5;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        std::vector<int> p4 = ext::find_induced_p4(g);
        ASSERT_EQ(!p4.empty(), bf_has_induced_p4(g)) << "mask=" << mask;
        if (!p4.empty()) {
            Obstruction o = graph_recognition::make_obstruction(ObstructionKind::P4, p4);
            ASSERT_TRUE(verify_obstruction(g, o)) << "mask=" << mask;
        }
    }
}

TEST(ObstructionExtract, PatternFromNonNestedCoversAllFourCases) {
    // u=1, v=2, x=3 in N(1)\N[2], y=4 in N(2)\N[1]; the four cases differ only
    // in whether uv and xy are edges.
    struct Case {
        bool uv;
        bool xy;
        ObstructionKind expected;
    };
    const Case cases[] = {{true, true, ObstructionKind::C4},
                          {true, false, ObstructionKind::P4},
                          {false, true, ObstructionKind::P4},
                          {false, false, ObstructionKind::TWO_K2}};

    for (size_t i = 0; i < 4; ++i) {
        std::vector<std::pair<int, int> > edges;
        edges.push_back(std::make_pair(1, 3));
        edges.push_back(std::make_pair(2, 4));
        if (cases[i].uv) edges.push_back(std::make_pair(1, 2));
        if (cases[i].xy) edges.push_back(std::make_pair(3, 4));
        Graph g(4, edges);

        Obstruction o = ext::pattern_from_non_nested(g, 1, 2, 3, 4);
        EXPECT_EQ(o.kind, cases[i].expected) << "case=" << i;
        EXPECT_TRUE(verify_obstruction(g, o)) << "case=" << i;
    }
}

TEST(ObstructionExtract, SplitObstructionFromHoleIsInducedInTheGraph) {
    for (int k = 4; k <= 8; ++k) {
        Graph g = make_cycle(k);
        std::vector<int> hole;
        for (int i = 1; i <= k; ++i) hole.push_back(i);

        Obstruction o = ext::split_obstruction_from_hole(hole, false);
        ASSERT_NE(o.kind, ObstructionKind::NONE) << "k=" << k;
        EXPECT_TRUE(verify_obstruction(g, o)) << "k=" << k;

        // The same hole seen from the complement side must yield a pattern
        // induced in the complement graph itself.
        Graph gc = build_complement(g);
        Obstruction oc = ext::split_obstruction_from_hole(hole, true);
        ASSERT_NE(oc.kind, ObstructionKind::NONE) << "k=" << k;
        EXPECT_TRUE(verify_obstruction(gc, oc)) << "k=" << k;
    }
}

TEST(ObstructionExtract, TpObstructionFromHoleIsInducedInTheGraph) {
    for (int k = 4; k <= 8; ++k) {
        Graph g = make_cycle(k);
        std::vector<int> hole;
        for (int i = 1; i <= k; ++i) hole.push_back(i);

        Obstruction o = ext::tp_obstruction_from_hole(hole);
        EXPECT_EQ(o.kind, k == 4 ? ObstructionKind::C4 : ObstructionKind::P4) << "k=" << k;
        EXPECT_TRUE(verify_obstruction(g, o)) << "k=" << k;
    }
}

TEST(ObstructionExtract, ForcingCycleExistsForEveryNonComparabilityGraph) {
    // Golumbic's theorem says a non-comparability graph always has an
    // implication class meeting its own reverse. This checks that the
    // Gamma-only search really finds one, on every graph of six vertices.
    const int n = 6;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        bool is_comparability = graph_recognition::check_comparability(g).is_comparability;
        Obstruction o = graph_recognition::build_comparability_obstruction(g);
        ASSERT_EQ(o.has_witness(), !is_comparability) << "mask=" << mask;
        if (o.has_witness()) {
            ASSERT_TRUE(verify_obstruction(g, o)) << "mask=" << mask;
        }
    }
}

TEST(ObstructionExtract, CactusWitnessExistsForEveryNonCactusGraph) {
    const int n = 6;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        bool is_cactus = graph_recognition::check_cactus(g).is_cactus;
        Obstruction o = graph_recognition::build_cactus_obstruction(g);
        ASSERT_EQ(o.has_witness(), !is_cactus) << "mask=" << mask;
        if (o.has_witness()) {
            ASSERT_TRUE(verify_obstruction(g, o)) << "mask=" << mask;
        }
    }
}

TEST(ObstructionExtract, PtolemaicWitnessExistsForEveryNonPtolemaicGraph) {
    const int n = 6;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        graph_recognition::PtolemaicResult r = graph_recognition::check_ptolemaic(g);
        if (r.is_ptolemaic) {
            ASSERT_FALSE(r.obstruction.has_witness()) << "mask=" << mask;
            continue;
        }
        ASSERT_TRUE(r.obstruction.has_witness()) << "mask=" << mask;
        ASSERT_TRUE(verify_obstruction(g, r.obstruction)) << "mask=" << mask;
    }
}

TEST(ObstructionExtract, PlanarMinorModelExistsForEveryNonPlanarGraph) {
    // Wagner's theorem, checked constructively: the branch sets have to be
    // disjoint, individually connected, and pairwise joined by a real edge.
    const int n = 6;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        bool is_planar = graph_recognition::check_planar(g).is_planar;
        Obstruction o = graph_recognition::build_planar_obstruction(g);
        ASSERT_EQ(o.has_witness(), !is_planar) << "mask=" << mask;
        if (o.has_witness()) {
            ASSERT_TRUE(verify_obstruction(g, o)) << "mask=" << mask;
        }
    }
}

TEST(ObstructionExtract, NonIsometricPathExistsForEveryNonDhGraph) {
    const int n = 6;
    std::vector<std::pair<int, int> > pairs = all_pairs(n);
    const int total = 1 << static_cast<int>(pairs.size());
    for (int mask = 0; mask < total; ++mask) {
        Graph g = graph_from_mask(n, pairs, mask);
        bool is_dh = graph_recognition::check_distance_hereditary(g).is_distance_hereditary;
        Obstruction o = graph_recognition::build_distance_hereditary_obstruction(g);
        ASSERT_EQ(o.has_witness(), !is_dh) << "mask=" << mask;
        if (o.has_witness()) {
            ASSERT_TRUE(verify_obstruction(g, o)) << "mask=" << mask;
        }
    }
}

TEST(ObstructionExtract, VerifyObstructionRejectsInvalidCertificates) {
    Graph c5 = make_cycle(5);
    Obstruction empty;
    EXPECT_FALSE(verify_obstruction(c5, empty));

    // A cycle with a repeated vertex is not simple.
    std::vector<int> repeated;
    repeated.push_back(1);
    repeated.push_back(2);
    repeated.push_back(3);
    repeated.push_back(2);
    EXPECT_FALSE(verify_obstruction(
        c5, graph_recognition::make_obstruction(ObstructionKind::HOLE, repeated)));

    // A triangle is not an odd hole (too short).
    std::vector<std::pair<int, int> > tri_edges;
    tri_edges.push_back(std::make_pair(1, 2));
    tri_edges.push_back(std::make_pair(2, 3));
    tri_edges.push_back(std::make_pair(1, 3));
    Graph tri(3, tri_edges);
    std::vector<int> tri_cyc;
    tri_cyc.push_back(1);
    tri_cyc.push_back(2);
    tri_cyc.push_back(3);
    EXPECT_TRUE(verify_obstruction(
        tri, graph_recognition::make_obstruction(ObstructionKind::ODD_CYCLE, tri_cyc)));
    EXPECT_FALSE(verify_obstruction(
        tri, graph_recognition::make_obstruction(ObstructionKind::ODD_HOLE, tri_cyc)));

    // A chorded 4-cycle is no hole: K4 minus nothing has both diagonals.
    std::vector<std::pair<int, int> > k4_edges = all_pairs(4);
    Graph k4(4, k4_edges);
    std::vector<int> quad;
    for (int i = 1; i <= 4; ++i) quad.push_back(i);
    EXPECT_FALSE(verify_obstruction(
        k4, graph_recognition::make_obstruction(ObstructionKind::HOLE, quad)));

    // A triangle plus a pendant is not a claw.
    std::vector<std::pair<int, int> > claw_like = tri_edges;
    claw_like.push_back(std::make_pair(1, 4));
    Graph almost(4, claw_like);
    std::vector<int> center_first;
    center_first.push_back(1);
    center_first.push_back(2);
    center_first.push_back(3);
    center_first.push_back(4);
    EXPECT_FALSE(verify_obstruction(
        almost, graph_recognition::make_obstruction(ObstructionKind::CLAW, center_first)));
}

TEST(ObstructionExtract, VerifyObstructionAcceptsComplementSidePatterns) {
    // 2K2 in the complement of C4 is the pair of diagonals.
    Graph c4 = make_cycle(4);
    std::vector<int> diag;
    diag.push_back(1);
    diag.push_back(3);
    diag.push_back(2);
    diag.push_back(4);
    Obstruction o = graph_recognition::make_obstruction(ObstructionKind::TWO_K2, diag, true);
    EXPECT_TRUE(verify_obstruction(c4, o));

    Obstruction primal = graph_recognition::make_obstruction(ObstructionKind::TWO_K2, diag);
    EXPECT_FALSE(verify_obstruction(c4, primal));
}

}  // namespace
