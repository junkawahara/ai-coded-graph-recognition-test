#include "util/graph.h"
#include "recognizers/planar.h"
#include "decompositions/planar_embedding.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <set>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::PlanarEmbeddingAlgorithm;
using graph_recognition::PlanarEmbeddingResult;
using graph_recognition::check_planar;
using graph_recognition::compute_planar_embedding;

// The rotation must list each vertex's neighbours, every half-edge must lie
// on exactly one face, and Euler's formula must hold per component.
bool embedding_describes(const Graph& g, const PlanarEmbeddingResult& r) {
    int n = g.n;
    if ((int)r.rotation.size() != n + 1) return false;
    for (int v = 1; v <= n; ++v) {
        std::vector<int> rot = r.rotation[v], nbrs = g.adj[v];
        std::sort(rot.begin(), rot.end());
        std::sort(nbrs.begin(), nbrs.end());
        if (rot != nbrs) return false;
    }

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

// Exhaustive continuation of the unit test at n = 7, plus random larger
// graphs where a maximal planar graph is thinned to a random subgraph.
TEST(PlanarEmbeddingProperty, ExhaustiveOnSevenVerticesAndRandomLarger) {
    const int n = 7;
    std::vector<std::pair<int, int>> all;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
    int m = (int)all.size();
    for (int mask = 0; mask < (1 << m); ++mask) {
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < m; ++i)
            if (mask & (1 << i)) edges.push_back(all[i]);
        Graph g(n, edges);
        PlanarEmbeddingResult r =
            compute_planar_embedding(g, PlanarEmbeddingAlgorithm::DMP_GENERAL);
        ASSERT_EQ(r.success, check_planar(g).is_planar) << "mask=" << mask;
        if (r.success) ASSERT_TRUE(embedding_describes(g, r)) << "mask=" << mask;
    }

    std::srand(90125);
    for (int trial = 0; trial < 400; ++trial) {
        int k = 4 + std::rand() % 12;
        double p = (std::rand() % 60 + 5) / 100.0;
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= k; ++u)
            for (int v = u + 1; v <= k; ++v)
                if ((std::rand() % 1000) / 1000.0 < p) edges.push_back(std::make_pair(u, v));
        Graph g(k, edges);
        PlanarEmbeddingResult r =
            compute_planar_embedding(g, PlanarEmbeddingAlgorithm::DMP_GENERAL);
        ASSERT_EQ(r.success, check_planar(g).is_planar) << "trial=" << trial << " n=" << k;
        if (r.success) ASSERT_TRUE(embedding_describes(g, r)) << "trial=" << trial;
    }
}

}  // namespace
