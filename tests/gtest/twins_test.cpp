#include "decompositions/twins.h"

#include "certificates.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::verify_twin_quotient;

namespace {

// Brute-force twin classes straight from the definition: u and v are twins
// when N[u] == N[v] (true) or N(u) == N(v) (false). The relation is an
// equivalence, so a single sweep assigning each vertex to the first class it
// matches is enough.
std::vector<int> bf_twin_classes(const Graph& g, TwinKind kind, int& num_classes) {
    int n = g.n;
    std::vector<int> cls(n + 1, 0);
    num_classes = 0;
    std::vector<int> reps;
    for (int v = 1; v <= n; ++v) {
        int found = 0;
        for (size_t i = 0; i < reps.size() && !found; ++i) {
            int u = reps[i];
            bool true_twin = g.has_edge(u, v);
            bool false_twin = !g.has_edge(u, v);
            for (int w = 1; w <= n; ++w) {
                if (w == u || w == v) continue;
                if (g.has_edge(u, w) != g.has_edge(v, w)) {
                    true_twin = false;
                    false_twin = false;
                    break;
                }
            }
            bool ok = (kind == TwinKind::TRUE_TWINS && true_twin) ||
                      (kind == TwinKind::FALSE_TWINS && false_twin) ||
                      (kind == TwinKind::BOTH && (true_twin || false_twin));
            if (ok) found = cls[u];
        }
        if (found) {
            cls[v] = found;
        } else {
            cls[v] = ++num_classes;
            reps.push_back(v);
        }
    }
    return cls;
}

bool same_partition(const std::vector<int>& a, const std::vector<int>& b, int n) {
    for (int u = 1; u <= n; ++u) {
        for (int v = 1; v <= n; ++v) {
            if ((a[u] == a[v]) != (b[u] == b[v])) return false;
        }
    }
    return true;
}

Graph make_star(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 2; v <= n; ++v) edges.push_back(std::make_pair(1, v));
    return Graph(n, edges);
}

Graph make_complete(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) edges.push_back(std::make_pair(u, v));
    return Graph(n, edges);
}

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

TEST(TwinsTest, EmptyGraphIsHandled) {
    Graph g;
    TwinQuotientResult q = contract_twins(g);
    EXPECT_EQ(q.quotient.n, 0);
    EXPECT_EQ(q.block_of.size(), 1u);
    EXPECT_EQ(q.members.size(), 1u);
    EXPECT_TRUE(verify_twin_quotient(g, q));
}

TEST(TwinsTest, CriticalCliquesOfCompleteGraphCollapseToOneVertex) {
    Graph g = make_complete(5);
    TwinQuotientResult q = critical_clique_quotient(g);
    ASSERT_EQ(q.quotient.n, 1);
    EXPECT_EQ(q.members[1].size(), 5u);
    EXPECT_TRUE(verify_twin_quotient(g, q));
}

TEST(TwinsTest, CriticalCliquesOfStarKeepCenterSeparate) {
    // The leaves are false twins, not true twins, so the critical cliques are
    // the center and each individual leaf.
    Graph g = make_star(5);
    TwinQuotientResult q = critical_clique_quotient(g);
    EXPECT_EQ(q.quotient.n, 5);
    EXPECT_TRUE(verify_twin_quotient(g, q));

    // Contracting false twins instead merges the four leaves.
    TwinQuotientResult f = contract_twins(g, TwinKind::FALSE_TWINS);
    ASSERT_EQ(f.quotient.n, 2);
    EXPECT_TRUE(verify_twin_quotient(g, f));
}

TEST(TwinsTest, IteratedContractionCollapsesStarAndCycle) {
    // K_{1,9}: leaves are false twins -> K2, whose vertices are true twins.
    EXPECT_EQ(contract_twins(make_star(10)).quotient.n, 1);
    // C4: opposite pairs are false twins -> K2 -> K1.
    EXPECT_EQ(contract_twins(make_cycle(4)).quotient.n, 1);
    // P5 has no twins at all.
    EXPECT_EQ(contract_twins(make_path(5)).quotient.n, 5);
    // C5 has no twins either.
    EXPECT_EQ(contract_twins(make_cycle(5)).quotient.n, 5);
}

TEST(TwinsTest, IteratedContractionReachesAFixpoint) {
    // One more round on the result must not merge anything further.
    Graph inputs[] = {make_star(6), make_cycle(4), make_complete(4), make_path(6)};
    for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i) {
        TwinQuotientResult q = contract_twins(inputs[i]);
        TwinQuotientResult again = contract_twins(q.quotient);
        EXPECT_EQ(again.quotient.n, q.quotient.n) << "input index " << i;
    }
}

TEST(TwinsTest, BlockOfAndMembersAgreeAfterIteration) {
    Graph g = make_star(10);
    TwinQuotientResult q = contract_twins(g);
    // Composition across rounds must still describe a partition of the
    // original vertex set, not of the intermediate quotient.
    ASSERT_EQ(q.block_of.size(), (size_t)g.n + 1);
    EXPECT_TRUE(verify_twin_quotient(g, q));
    size_t covered = 0;
    for (size_t i = 1; i < q.members.size(); ++i) covered += q.members[i].size();
    EXPECT_EQ(covered, (size_t)g.n);
}

// Exhaustive agreement with the definition, plus the structural invariants of
// a single-kind contraction: true-twin blocks are cliques, false-twin blocks
// are independent sets, and neither leaves any twins behind.
TEST(TwinsTest, ExhaustiveAgreementWithBruteForce) {
    const TwinKind kinds[] = {TwinKind::TRUE_TWINS, TwinKind::FALSE_TWINS};
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);

            for (size_t ki = 0; ki < 2; ++ki) {
                TwinKind kind = kinds[ki];
                int expect_classes = 0;
                std::vector<int> expect = bf_twin_classes(g, kind, expect_classes);

                int got_classes = 0;
                std::vector<int> got = detail::twin_classes(g, kind, got_classes);
                ASSERT_EQ(got_classes, expect_classes) << "n=" << n << " mask=" << mask;
                EXPECT_TRUE(same_partition(got, expect, n)) << "n=" << n << " mask=" << mask;

                TwinQuotientResult q = contract_twins(g, kind);
                EXPECT_TRUE(verify_twin_quotient(g, q)) << "n=" << n << " mask=" << mask;

                // Each block is a clique (true twins) or independent (false twins).
                for (size_t b = 1; b < q.members.size(); ++b) {
                    for (size_t x = 0; x < q.members[b].size(); ++x) {
                        for (size_t y = x + 1; y < q.members[b].size(); ++y) {
                            bool adj = g.has_edge(q.members[b][x], q.members[b][y]);
                            EXPECT_EQ(adj, kind == TwinKind::TRUE_TWINS)
                                << "n=" << n << " mask=" << mask;
                        }
                    }
                }

                // A single round is already a fixpoint for one kind.
                int again = 0;
                detail::twin_classes(q.quotient, kind, again);
                EXPECT_EQ(again, q.quotient.n) << "n=" << n << " mask=" << mask;
            }

            // The iterated contraction must leave no twins of either kind.
            TwinQuotientResult both = contract_twins(g, TwinKind::BOTH);
            EXPECT_TRUE(verify_twin_quotient(g, both)) << "n=" << n << " mask=" << mask;
            int leftover = 0;
            detail::twin_classes(both.quotient, TwinKind::BOTH, leftover);
            EXPECT_EQ(leftover, both.quotient.n) << "n=" << n << " mask=" << mask;
        }
    }
}

// Class ids must not depend on the union-by-rank shape: they are numbered by
// increasing smallest member.
TEST(TwinsTest, ClassIdsFollowSmallestMember) {
    Graph g = make_star(6);
    TwinQuotientResult q = contract_twins(g, TwinKind::FALSE_TWINS);
    ASSERT_EQ(q.quotient.n, 2);
    EXPECT_EQ(q.members[1][0], 1);  // the center
    EXPECT_EQ(q.members[2][0], 2);  // the leaf block starts at vertex 2
    for (size_t i = 1; i < q.members.size(); ++i) {
        for (size_t j = 1; j < q.members[i].size(); ++j) {
            EXPECT_LT(q.members[i][j - 1], q.members[i][j]);
        }
    }
}

}  // namespace
