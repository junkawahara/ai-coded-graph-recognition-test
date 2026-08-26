#include "recognizers/distance_hereditary.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::DistanceHereditaryAlgorithm;
using graph_recognition::DistanceHereditaryResult;
using graph_recognition::check_distance_hereditary;

// ---- Independent brute-force oracle -------------------------------------
// Straight from the definition: G is distance-hereditary iff every connected
// induced subgraph preserves distances, i.e. for every vertex subset S and
// every pair u, v connected within G[S], d_{G[S]}(u, v) = d_G(u, v).
// The three library implementations are all twin/pendant elimination
// variants, so this definitional check is a genuinely independent oracle.

const int kInf = 1 << 20;

// BFS distances from src within the induced subgraph G[mask] (mask over
// 0-based vertex ids; mask == full set gives distances in G itself).
void bf_bfs(int n, const std::vector<std::vector<bool>>& adj, int mask,
            int src, std::vector<int>& dist) {
    dist.assign(n, kInf);
    dist[src] = 0;
    std::vector<int> queue;
    queue.push_back(src);
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int u = queue[qi];
        for (int v = 0; v < n; ++v) {
            if (!adj[u + 1][v + 1] || !((mask >> v) & 1)) continue;
            if (dist[v] != kInf) continue;
            dist[v] = dist[u] + 1;
            queue.push_back(v);
        }
    }
}

bool bf_is_distance_hereditary(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    int full = (1 << n) - 1;
    // Distances in G
    std::vector<std::vector<int>> dg(n);
    for (int v = 0; v < n; ++v) bf_bfs(n, adj, full, v, dg[v]);

    std::vector<int> dist;
    for (int mask = 0; mask <= full; ++mask) {
        for (int u = 0; u < n; ++u) {
            if (!((mask >> u) & 1)) continue;
            bf_bfs(n, adj, mask, u, dist);
            for (int v = 0; v < n; ++v) {
                if (v == u || !((mask >> v) & 1)) continue;
                if (dist[v] != kInf && dist[v] != dg[u][v]) return false;
            }
        }
    }
    return true;
}

TEST(DistanceHereditaryProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // DH graph: pendant/twin addition
            n = 1 + std::rand() % 9;
            std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                if (std::rand() % 2 == 0) {
                    // pendant: connect to u only
                    edges.push_back(std::make_pair(u, v));
                    adj[u][v] = adj[v][u] = true;
                } else {
                    // true twin: connect to u and all of N(u)
                    edges.push_back(std::make_pair(u, v));
                    adj[u][v] = adj[v][u] = true;
                    for (int w = 1; w < v; ++w) {
                        if (w != u && adj[u][w]) {
                            edges.push_back(std::make_pair(w, v));
                            adj[w][v] = adj[v][w] = true;
                        }
                    }
                }
            }
        } else if (trial % 3 == 1) {
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        DistanceHereditaryResult r1 = check_distance_hereditary(g, DistanceHereditaryAlgorithm::SORTED_TWINS);
        DistanceHereditaryResult r2 = check_distance_hereditary(g, DistanceHereditaryAlgorithm::HASH_TWINS);
        DistanceHereditaryResult r3 = check_distance_hereditary(g, DistanceHereditaryAlgorithm::HASHMAP_TWINS);
        bool bf = bf_is_distance_hereditary(n, edges);

        ASSERT_EQ(r1.is_distance_hereditary, r2.is_distance_hereditary)
            << "SORTED vs HASH trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r2.is_distance_hereditary, r3.is_distance_hereditary)
            << "HASH vs HASHMAP trial=" << trial << " n=" << n << " m=" << edges.size();
        DistanceHereditaryResult r4 =
            check_distance_hereditary(g, DistanceHereditaryAlgorithm::SPLIT_DECOMPOSITION);
        ASSERT_EQ(r1.is_distance_hereditary, bf)
            << "impl vs definitional oracle trial=" << trial << " n=" << n << " m=" << edges.size();
        // split decomposition は twin 消去と何も共有しない独立確認
        ASSERT_EQ(r4.is_distance_hereditary, bf)
            << "SPLIT_DECOMPOSITION trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
