#include "decompositions/planar_embedding.h"

#include "recognizers/planar.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

// Independent validation: the rotation must list exactly each vertex's
// neighbours, the traced faces must use every half-edge once, and Euler's
// formula must hold in each connected component.
bool embedding_describes(const Graph& g, const PlanarEmbeddingResult& r) {
    int n = g.n;
    if ((int)r.rotation.size() != n + 1) return false;
    for (int v = 1; v <= n; ++v) {
        std::vector<int> rot = r.rotation[v];
        std::vector<int> nbrs = g.adj[v];
        std::sort(rot.begin(), rot.end());
        std::sort(nbrs.begin(), nbrs.end());
        if (rot != nbrs) return false;
    }

    // Every half-edge on exactly one face, in a consistent direction.
    std::set<std::pair<int, int>> seen;
    long long total = 0;
    for (size_t i = 0; i < r.faces.size(); ++i) {
        const std::vector<int>& f = r.faces[i];
        if (f.empty()) return false;
        total += (long long)f.size();
        for (size_t j = 0; j < f.size(); ++j) {
            int a = f[j], b = f[(j + 1) % f.size()];
            if (!g.has_edge(a, b)) return false;
            if (!seen.insert(std::make_pair(a, b)).second) return false;
        }
    }
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (total != 2 * m) return false;

    // Euler per component.
    std::vector<int> comp(n + 1, 0);
    int count = 0;
    for (int s = 1; s <= n; ++s) {
        if (comp[s]) continue;
        ++count;
        std::vector<int> stack(1, s);
        comp[s] = count;
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (size_t i = 0; i < g.adj[x].size(); ++i) {
                int y = g.adj[x][i];
                if (comp[y]) continue;
                comp[y] = count;
                stack.push_back(y);
            }
        }
    }
    std::vector<long long> nv(count + 1, 0), ne(count + 1, 0), nf(count + 1, 0);
    for (int v = 1; v <= n; ++v) {
        nv[comp[v]]++;
        ne[comp[v]] += (long long)g.adj[v].size();
    }
    for (int c = 1; c <= count; ++c) ne[c] /= 2;
    for (size_t i = 0; i < r.faces.size(); ++i) nf[comp[r.faces[i][0]]]++;
    for (int c = 1; c <= count; ++c) {
        if (ne[c] == 0) {
            if (nv[c] != 1 || nf[c] != 0) return false;
            continue;
        }
        if (nv[c] - ne[c] + nf[c] != 2) return false;
    }
    return true;
}

Graph make_cycle(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(n, 1));
    return Graph(n, edges);
}

Graph make_complete(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) edges.push_back(std::make_pair(u, v));
    return Graph(n, edges);
}

PlanarEmbeddingResult dmp(const Graph& g) {
    return compute_planar_embedding(g, PlanarEmbeddingAlgorithm::DMP_GENERAL);
}

TEST(PlanarEmbeddingGeneralTest, EmptyGraph) {
    Graph g;
    PlanarEmbeddingResult r = dmp(g);
    EXPECT_TRUE(r.success);
    EXPECT_TRUE(r.faces.empty());
}

TEST(PlanarEmbeddingGeneralTest, IsolatedVerticesEmbed) {
    Graph g(3, std::vector<std::pair<int, int>>());
    PlanarEmbeddingResult r = dmp(g);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(r.faces.empty());
    EXPECT_TRUE(embedding_describes(g, r));
}

TEST(PlanarEmbeddingGeneralTest, TreeEmbedsWithOneFace) {
    // A path: one face that runs along both sides of every bridge, which is
    // exactly the case the 3-connected validation would reject.
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < 5; ++v) edges.push_back(std::make_pair(v, v + 1));
    Graph g(5, edges);
    PlanarEmbeddingResult r = dmp(g);
    ASSERT_TRUE(r.success);
    ASSERT_EQ(r.faces.size(), 1u);
    EXPECT_EQ(r.faces[0].size(), 8u);  // 2 * 4 edges
    EXPECT_TRUE(embedding_describes(g, r));
}

TEST(PlanarEmbeddingGeneralTest, CycleHasTwoFaces) {
    Graph g = make_cycle(5);
    PlanarEmbeddingResult r = dmp(g);
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.faces.size(), 2u);
    EXPECT_TRUE(embedding_describes(g, r));
}

TEST(PlanarEmbeddingGeneralTest, K4Embeds) {
    Graph g = make_complete(4);
    PlanarEmbeddingResult r = dmp(g);
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.faces.size(), 4u);
    EXPECT_TRUE(embedding_describes(g, r));
}

TEST(PlanarEmbeddingGeneralTest, K5AndK33AreRejected) {
    EXPECT_FALSE(dmp(make_complete(5)).success);

    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= 3; ++u)
        for (int v = 4; v <= 6; ++v) edges.push_back(std::make_pair(u, v));
    Graph k33(6, edges);
    EXPECT_FALSE(dmp(k33).success);
}

TEST(PlanarEmbeddingGeneralTest, TwoBlocksSharingACutVertex) {
    // Two triangles glued at vertex 1: the rotation at the cut vertex is the
    // two blocks' segments spliced together.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 1));
    edges.push_back(std::make_pair(1, 4));
    edges.push_back(std::make_pair(4, 5));
    edges.push_back(std::make_pair(5, 1));
    Graph g(5, edges);
    PlanarEmbeddingResult r = dmp(g);
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.rotation[1].size(), 4u);
    EXPECT_TRUE(embedding_describes(g, r));
}

// Every graph on up to 6 vertices: DMP must succeed exactly on the planar
// ones, and every embedding it produces must describe the graph.
TEST(PlanarEmbeddingGeneralTest, ExhaustiveAgreementWithPlanarityTest) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);

            PlanarEmbeddingResult r = dmp(g);
            ASSERT_EQ(r.success, check_planar(g).is_planar) << "n=" << n << " mask=" << mask;
            if (!r.success) continue;
            ASSERT_TRUE(embedding_describes(g, r)) << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace

// review_all_1.md 8: the Tutte variant does not check 3-connectivity. Outside
// its precondition it may succeed or fail, but success always comes with a
// validated embedding (a cycle succeeds, and so does anything on <= 3 vertices).
TEST(PlanarEmbeddingGeneralTest, TutteOutsidePreconditionNeverReturnsGarbage) {
    Graph c4 = make_cycle(4);
    PlanarEmbeddingResult r = compute_planar_embedding(c4, PlanarEmbeddingAlgorithm::TUTTE_3CONNECTED);
    EXPECT_TRUE(r.success);
    EXPECT_TRUE(embedding_describes(c4, r));

    std::vector<Graph> outside;
    outside.push_back(make_complete(3));
    outside.push_back(make_cycle(5));
    {
        // A path: not even 2-connected.
        std::vector<std::pair<int, int>> e;
        e.push_back(std::make_pair(1, 2));
        e.push_back(std::make_pair(2, 3));
        e.push_back(std::make_pair(3, 4));
        outside.push_back(Graph(4, e));
    }
    {
        // Two K4s sharing an edge: 2-connected but not 3-connected.
        std::vector<std::pair<int, int>> e;
        int a[4] = {1, 2, 3, 4}, b[4] = {1, 2, 5, 6};
        for (int i = 0; i < 4; ++i)
            for (int j = i + 1; j < 4; ++j) {
                e.push_back(std::make_pair(a[i], a[j]));
                if (i >= 2 || j >= 2) e.push_back(std::make_pair(b[i], b[j]));
            }
        outside.push_back(Graph(6, e));
    }
    for (size_t i = 0; i < outside.size(); ++i) {
        PlanarEmbeddingResult t = compute_planar_embedding(outside[i], PlanarEmbeddingAlgorithm::TUTTE_3CONNECTED);
        if (t.success) { EXPECT_TRUE(embedding_describes(outside[i], t)) << "graph " << i; }
    }
}

// review_all_1.md 12: a successor map made of several short cycles at one
// vertex must be rejected, not walked round repeatedly.
TEST(PlanarEmbeddingGeneralTest, RotationFromFacesRejectsSeveralCyclesAtAVertex) {
    std::vector<std::vector<int>> faces;
    int f1[] = {1, 2, 3}, f2[] = {1, 3, 2}, f3[] = {1, 4, 5}, f4[] = {1, 5, 4};
    faces.push_back(std::vector<int>(f1, f1 + 3));
    faces.push_back(std::vector<int>(f2, f2 + 3));
    faces.push_back(std::vector<int>(f3, f3 + 3));
    faces.push_back(std::vector<int>(f4, f4 + 3));
    std::vector<std::vector<int>> rotation;
    EXPECT_FALSE(detail::rotation_from_faces(5, faces, &rotation));

    // A genuine face list of the bowtie (the same two triangles plus the
    // outer face, which passes through vertex 1 twice) must still be accepted.
    Graph bowtie(5, std::vector<std::pair<int, int>>{{1, 2}, {2, 3}, {3, 1}, {1, 4}, {4, 5}, {5, 1}});
    PlanarEmbeddingResult r = dmp(bowtie);
    ASSERT_TRUE(r.success);
    std::vector<std::vector<int>> rebuilt;
    ASSERT_TRUE(detail::rotation_from_faces(5, r.faces, &rebuilt));
    for (int v = 1; v <= 5; ++v) {
        std::vector<int> a = rebuilt[v], b = r.rotation[v];
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        EXPECT_EQ(a, b) << "vertex " << v;
    }
}

// review_all_1.md 13: Euler's formula and the total face length do not rule
// out a half-edge that is missing or duplicated. The validator must check
// the half-edges themselves.
TEST(PlanarEmbeddingGeneralTest, ValidGeneralRejectsRepeatedOrNonEdgeHalfEdges) {
    Graph k3(3, std::vector<std::pair<int, int>>{{1, 2}, {2, 3}, {3, 1}});
    std::vector<std::vector<int>> faces;
    int tri[] = {1, 2, 3};
    faces.push_back(std::vector<int>(tri, tri + 3));
    faces.push_back(std::vector<int>(tri, tri + 3));
    // Same triangle twice in the same direction: 6 half-edges, Euler holds.
    EXPECT_FALSE(detail::embedding_is_valid_general(k3, faces));

    int rev[] = {1, 3, 2};
    faces[1] = std::vector<int>(rev, rev + 3);
    EXPECT_TRUE(detail::embedding_is_valid_general(k3, faces));

    // A path 1-2-3 has the single face 1,2,3,2. A closed walk of the same
    // length through the non-edge {3,1} must be rejected.
    Graph p3(3, std::vector<std::pair<int, int>>{{1, 2}, {2, 3}});
    std::vector<std::vector<int>> walk(1);
    int good[] = {1, 2, 3, 2};
    walk[0] = std::vector<int>(good, good + 4);
    EXPECT_TRUE(detail::embedding_is_valid_general(p3, walk));
    int bad[] = {1, 2, 3, 1};
    walk[0] = std::vector<int>(bad, bad + 4);
    EXPECT_FALSE(detail::embedding_is_valid_general(p3, walk));
    int out_of_range[] = {1, 2, 4, 2};
    walk[0] = std::vector<int>(out_of_range, out_of_range + 4);
    EXPECT_FALSE(detail::embedding_is_valid_general(p3, walk));
}
