#include "certificates.h"

#include "block_cut_tree.h"
#include "chordal.h"
#include "twins.h"

#include <algorithm>
#include <set>

namespace graph_recognition {
namespace gtest_utils {

namespace {

// Connectivity of the subgraph induced on `verts` after deleting `skip`.
bool block_connected_without(const Graph& g, const std::vector<int>& verts, int skip) {
    std::set<int> alive(verts.begin(), verts.end());
    alive.erase(skip);
    if (alive.empty()) return true;
    std::vector<int> stack(1, *alive.begin());
    std::set<int> seen;
    seen.insert(stack[0]);
    while (!stack.empty()) {
        int v = stack.back();
        stack.pop_back();
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (!alive.count(u) || seen.count(u)) continue;
            seen.insert(u);
            stack.push_back(u);
        }
    }
    return seen.size() == alive.size();
}

}  // namespace

bool verify_block_cut_tree(const Graph& g, const BlockCutTreeResult& r) {
    int n = g.n;
    size_t b = r.blocks.size();
    if (r.block_edges.size() != b) return false;
    if (static_cast<int>(r.is_cut.size()) != n + 1) return false;
    if (static_cast<int>(r.block_of.size()) != n + 1) return false;
    if (static_cast<int>(r.node_of_cut_vertex.size()) != n + 1) return false;

    // The blocks partition the edge set.
    std::set<std::pair<int, int>> all_edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) all_edges.insert(std::make_pair(u, v));
        }
    }
    std::set<std::pair<int, int>> seen_edges;
    for (size_t i = 0; i < b; ++i) {
        std::set<int> endpoints;
        for (size_t j = 0; j < r.block_edges[i].size(); ++j) {
            std::pair<int, int> e = r.block_edges[i][j];
            if (e.first >= e.second) return false;
            if (!all_edges.count(e)) return false;
            if (!seen_edges.insert(e).second) return false;
            endpoints.insert(e.first);
            endpoints.insert(e.second);
        }
        // Vertex set matches the endpoints, except for K1 blocks.
        std::vector<int> verts = r.blocks[i];
        for (size_t j = 1; j < verts.size(); ++j) {
            if (verts[j - 1] >= verts[j]) return false;
        }
        if (r.block_edges[i].empty()) {
            if (verts.size() != 1) return false;
            if (!g.adj[verts[0]].empty()) return false;
        } else {
            if (std::vector<int>(endpoints.begin(), endpoints.end()) != verts) return false;
        }
        // Biconnected: deleting any single vertex keeps the block connected.
        // (Also true for K1 and K2, where the check is vacuous.)
        for (size_t j = 0; j < verts.size(); ++j) {
            if (!block_connected_without(g, verts, verts[j])) return false;
        }
    }
    if (seen_edges != all_edges) return false;

    // Maximality: two distinct blocks share at most one vertex, otherwise
    // their union would still be biconnected.
    for (size_t i = 0; i < b; ++i) {
        for (size_t j = i + 1; j < b; ++j) {
            std::vector<int> shared;
            std::set_intersection(r.blocks[i].begin(), r.blocks[i].end(),
                                  r.blocks[j].begin(), r.blocks[j].end(),
                                  std::back_inserter(shared));
            if (shared.size() > 1) return false;
        }
    }

    // block_of, is_cut and bridges are all derived from the blocks.
    for (int v = 1; v <= n; ++v) {
        std::vector<int> expect;
        for (size_t i = 0; i < b; ++i) {
            if (std::binary_search(r.blocks[i].begin(), r.blocks[i].end(), v)) {
                expect.push_back((int)i);
            }
        }
        if (expect.empty()) return false;  // every vertex lies in a block
        std::vector<int> got = r.block_of[v];
        std::sort(got.begin(), got.end());
        if (got != expect) return false;
        if ((r.is_cut[v] != 0) != (expect.size() > 1)) return false;
    }
    std::vector<std::pair<int, int>> expect_bridges;
    for (size_t i = 0; i < b; ++i) {
        if (r.block_edges[i].size() == 1) expect_bridges.push_back(r.block_edges[i][0]);
    }
    std::sort(expect_bridges.begin(), expect_bridges.end());
    if (r.bridges != expect_bridges) return false;

    // Block-cut forest: bipartite between block nodes and cut-vertex nodes,
    // edges exactly the incidences, and acyclic.
    size_t nodes = r.tree.size();
    if (r.cut_vertex_of_node.size() != nodes) return false;
    if (nodes < b) return false;
    for (size_t i = 0; i < b; ++i) {
        if (r.cut_vertex_of_node[i] != 0) return false;
    }
    size_t num_cut = 0;
    for (int v = 1; v <= n; ++v) {
        if (!r.is_cut[v]) {
            if (r.node_of_cut_vertex[v] != -1) return false;
            continue;
        }
        ++num_cut;
        int node = r.node_of_cut_vertex[v];
        if (node < (int)b || node >= (int)nodes) return false;
        if (r.cut_vertex_of_node[node] != v) return false;
    }
    if (nodes != b + num_cut) return false;

    size_t tree_edges = 0;
    for (size_t i = 0; i < b; ++i) {
        std::set<int> expect_nbrs;
        for (size_t j = 0; j < r.blocks[i].size(); ++j) {
            int v = r.blocks[i][j];
            if (r.is_cut[v]) expect_nbrs.insert(r.node_of_cut_vertex[v]);
        }
        std::set<int> got_nbrs(r.tree[i].begin(), r.tree[i].end());
        if (got_nbrs != expect_nbrs) return false;
        if (r.tree[i].size() != expect_nbrs.size()) return false;
        tree_edges += r.tree[i].size();
    }
    for (size_t node = b; node < nodes; ++node) {
        int v = r.cut_vertex_of_node[node];
        std::set<int> expect_nbrs(r.block_of[v].begin(), r.block_of[v].end());
        std::set<int> got_nbrs(r.tree[node].begin(), r.tree[node].end());
        if (got_nbrs != expect_nbrs) return false;
        if (r.tree[node].size() != expect_nbrs.size()) return false;
        tree_edges += r.tree[node].size();
    }
    if (tree_edges % 2 != 0) return false;
    tree_edges /= 2;

    // Acyclic: edges == nodes - components.
    std::vector<bool> seen_node(nodes, false);
    size_t comps = 0;
    for (size_t s = 0; s < nodes; ++s) {
        if (seen_node[s]) continue;
        ++comps;
        std::vector<int> stack(1, (int)s);
        seen_node[s] = true;
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (size_t i = 0; i < r.tree[x].size(); ++i) {
                int y = r.tree[x][i];
                if (seen_node[y]) continue;
                seen_node[y] = true;
                stack.push_back(y);
            }
        }
    }
    if (tree_edges != nodes - comps) return false;
    return true;
}

bool verify_twin_quotient(const Graph& g, const TwinQuotientResult& q) {
    int n = g.n;
    int k = q.quotient.n;
    if (static_cast<int>(q.block_of.size()) != n + 1) return false;
    if (static_cast<int>(q.members.size()) != k + 1) return false;

    // block_of and members must describe the same partition of 1..n.
    size_t covered = 0;
    for (int i = 1; i <= k; ++i) {
        if (q.members[i].empty()) return false;
        for (size_t j = 0; j < q.members[i].size(); ++j) {
            int v = q.members[i][j];
            if (v < 1 || v > n) return false;
            if (j > 0 && q.members[i][j - 1] >= v) return false;
            if (q.block_of[v] != i) return false;
        }
        covered += q.members[i].size();
    }
    if (covered != static_cast<size_t>(n)) return false;
    for (int v = 1; v <= n; ++v) {
        if (q.block_of[v] < 1 || q.block_of[v] > k) return false;
    }

    // Every block must be a module: two blocks are either completely joined or
    // completely non-adjacent, and the quotient records exactly which.
    for (int i = 1; i <= k; ++i) {
        for (int j = i + 1; j <= k; ++j) {
            bool joined = g.has_edge(q.members[i][0], q.members[j][0]);
            for (size_t a = 0; a < q.members[i].size(); ++a) {
                for (size_t b = 0; b < q.members[j].size(); ++b) {
                    if (g.has_edge(q.members[i][a], q.members[j][b]) != joined) return false;
                }
            }
            if (q.quotient.has_edge(i, j) != joined) return false;
        }
    }
    return true;
}

bool verify_interval_model(const Graph& g,
                           const std::vector<std::pair<int, int>>& intervals) {
    int n = g.n;
    if (static_cast<int>(intervals.size()) < n + 1) {
        return false;
    }
    for (int v = 1; v <= n; ++v) {
        if (intervals[v].first > intervals[v].second) {
            return false;
        }
    }
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            bool overlap = !(intervals[i].second < intervals[j].first ||
                             intervals[j].second < intervals[i].first);
            if (overlap != g.has_edge(i, j)) {
                return false;
            }
        }
    }
    return true;
}

bool verify_bipartite_coloring(const Graph& g, const std::vector<int>& color) {
    int n = g.n;
    if (static_cast<int>(color.size()) < n + 1) return false;
    for (int v = 1; v <= n; ++v) {
        if (color[v] != 0 && color[v] != 1) return false;
    }
    for (int u = 1; u <= n; ++u) {
        const std::vector<int>& adj = g.adj[u];
        for (size_t i = 0; i < adj.size(); ++i) {
            int v = adj[i];
            if (color[u] == color[v]) return false;
        }
    }
    return true;
}

bool verify_chordal_peo(const Graph& g, const ChordalResult& r) {
    int n = g.n;
    const std::vector<int>& number = r.mcs_result.number;
    if (static_cast<int>(number.size()) < n + 1) return false;
    if (static_cast<int>(r.later.size()) < n + 1) return false;

    // number must be a permutation of 1..n
    std::vector<int> seen(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        int p = number[v];
        if (p < 1 || p > n || seen[p]) return false;
        seen[p] = 1;
    }

    // For each v, later[v] must be exactly the neighbors u with number[u] > number[v],
    // and this set must form a clique. A bug that returns an empty later[v] is rejected
    // here because the recomputation would disagree.
    for (int v = 1; v <= n; ++v) {
        std::vector<int> expected;
        const std::vector<int>& adj = g.adj[v];
        for (size_t i = 0; i < adj.size(); ++i) {
            int u = adj[i];
            if (number[u] > number[v]) expected.push_back(u);
        }
        std::sort(expected.begin(), expected.end());
        std::vector<int> actual = r.later[v];
        std::sort(actual.begin(), actual.end());
        if (expected != actual) return false;

        for (size_t i = 0; i < r.later[v].size(); ++i) {
            for (size_t j = i + 1; j < r.later[v].size(); ++j) {
                if (!g.has_edge(r.later[v][i], r.later[v][j])) return false;
            }
        }
    }
    return true;
}

bool verify_circle_dow(const Graph& g, const std::vector<int>& dow) {
    int n = g.n;
    if (static_cast<int>(dow.size()) != 2 * n) return false;
    std::vector<std::pair<int, int>> pos(n + 1, std::make_pair(-1, -1));
    for (int i = 0; i < 2 * n; ++i) {
        int v = dow[i];
        if (v < 1 || v > n) return false;
        if (pos[v].first == -1) pos[v].first = i;
        else if (pos[v].second == -1) pos[v].second = i;
        else return false;
    }
    for (int v = 1; v <= n; ++v) {
        if (pos[v].first == -1 || pos[v].second == -1) return false;
    }
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            int a = pos[u].first, b = pos[u].second;
            int c = pos[v].first, d = pos[v].second;
            bool cross = (a < c && c < b && b < d) || (c < a && a < d && d < b);
            if (cross != g.has_edge(u, v)) return false;
        }
    }
    return true;
}

static bool check_convex_side(const Graph& g,
                              const std::vector<int>& color,
                              int other_color,
                              const std::vector<int>& ordering) {
    int n = g.n;
    std::vector<int> pos(n + 1, -1);
    for (size_t i = 0; i < ordering.size(); ++i) {
        int v = ordering[i];
        if (v < 1 || v > n) return false;
        if (color[v] != other_color) return false;
        if (pos[v] != -1) return false;
        pos[v] = static_cast<int>(i);
    }
    for (int u = 1; u <= n; ++u) {
        if (color[u] == other_color) continue;
        const std::vector<int>& adj = g.adj[u];
        if (adj.empty()) continue;
        int mn = static_cast<int>(ordering.size()), mx = -1;
        for (size_t i = 0; i < adj.size(); ++i) {
            int p = pos[adj[i]];
            if (p == -1) return false;
            if (p < mn) mn = p;
            if (p > mx) mx = p;
        }
        for (int p = mn; p <= mx; ++p) {
            if (!g.has_edge(u, ordering[p])) return false;
        }
    }
    return true;
}

bool verify_convex_bipartite(const Graph& g, const std::vector<int>& color,
                             const std::vector<int>& ordering) {
    if (!verify_bipartite_coloring(g, color)) return false;
    return check_convex_side(g, color, 1, ordering);
}

bool verify_biconvex_bipartite(const Graph& g, const std::vector<int>& color,
                               const std::vector<int>& x_ordering,
                               const std::vector<int>& y_ordering) {
    if (!verify_bipartite_coloring(g, color)) return false;
    if (!check_convex_side(g, color, 1, y_ordering)) return false;
    if (!check_convex_side(g, color, 0, x_ordering)) return false;
    return true;
}

bool verify_kregular_k(const Graph& g, int k) {
    if (k < 0) return false;
    for (int v = 1; v <= g.n; ++v) {
        if (static_cast<int>(g.adj[v].size()) != k) return false;
    }
    return true;
}

bool verify_strongly_regular_params(const Graph& g, int k, int lambda, int mu) {
    int n = g.n;
    if (!verify_kregular_k(g, k)) return false;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            int common = 0;
            const std::vector<int>& adj_u = g.adj[u];
            for (size_t i = 0; i < adj_u.size(); ++i) {
                int w = adj_u[i];
                if (w != v && g.has_edge(v, w)) common++;
            }
            int expected = g.has_edge(u, v) ? lambda : mu;
            if (common != expected) return false;
        }
    }
    return true;
}

}  // namespace gtest_utils
}  // namespace graph_recognition
