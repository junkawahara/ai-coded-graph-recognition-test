#include "certificates.h"

#include "decompositions/block_cut_tree.h"
#include "recognizers/chordal.h"
#include "certificates/forbidden_subgraph.h"
#include "recognizers/line_graph.h"
#include "decompositions/md_tree.h"
#include "recognizers/series_parallel.h"
#include "decompositions/transitive_orientation.h"
#include "decompositions/tree_decomposition.h"
#include "decompositions/twins.h"

#include <algorithm>
#include <climits>
#include <set>
#include <utility>

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

namespace {

bool nested_along(const Graph& g, const std::vector<int>& order,
                  const std::vector<int>& other, bool complemented) {
    std::vector<char> in_other(g.n + 1, 0);
    for (size_t i = 0; i < other.size(); ++i) in_other[other[i]] = 1;
    for (size_t i = 0; i + 1 < order.size(); ++i) {
        for (size_t j = 0; j < other.size(); ++j) {
            int w = other[j];
            if (!in_other[w]) continue;
            bool a = g.has_edge(order[i], w);
            bool b = g.has_edge(order[i + 1], w);
            if (complemented) {
                // Non-neighbourhoods grow, so neighbourhoods shrink.
                if (b && !a) return false;
            } else {
                if (a && !b) return false;
            }
        }
    }
    return true;
}

}  // namespace

bool verify_chain_orders(const Graph& g, const std::vector<int>& color,
                         const std::vector<int>& x_ordering,
                         const std::vector<int>& y_ordering, bool complemented) {
    int n = g.n;
    if (static_cast<int>(color.size()) != n + 1) return false;

    std::vector<int> expect_x, expect_y;
    for (int v = 1; v <= n; ++v) {
        if (color[v] == 0) {
            expect_x.push_back(v);
        } else if (color[v] == 1) {
            expect_y.push_back(v);
        } else {
            return false;
        }
    }
    std::vector<int> got_x = x_ordering, got_y = y_ordering;
    std::sort(got_x.begin(), got_x.end());
    std::sort(got_y.begin(), got_y.end());
    if (got_x != expect_x || got_y != expect_y) return false;

    // Inside a class: no edges for a chain graph, all edges for a cochain one.
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (color[u] != color[v]) continue;
            if (g.has_edge(u, v) != complemented) return false;
        }
    }

    if (!nested_along(g, x_ordering, expect_y, complemented)) return false;
    if (!nested_along(g, y_ordering, expect_x, complemented)) return false;
    return true;
}

bool verify_threshold_creation_sequence(const Graph& g, const std::vector<int>& order,
                                        const std::vector<int>& kind) {
    int n = g.n;
    if (static_cast<int>(order.size()) != n + 1) return false;
    if (static_cast<int>(kind.size()) != n + 1) return false;

    std::vector<int> seen(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        if (v < 1 || v > n || seen[v]) return false;
        seen[v] = 1;
    }
    if (n >= 1 && kind[1] != 0) return false;

    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        for (int j = 1; j < i; ++j) {
            bool adj = g.has_edge(v, order[j]);
            if (kind[i] == 0) {
                if (adj) return false;
            } else if (kind[i] == 1) {
                if (!adj) return false;
            } else {
                return false;
            }
        }
    }
    return true;
}

bool verify_tree_decomposition(const Graph& g, const TreeDecompositionResult& r) {
    int n = g.n;
    size_t k = r.bags.size();
    if (r.tree.size() != k) return false;
    if (n == 0) return k == 0 && r.width == -1;
    if (k == 0) return false;

    // The structure must be a tree: symmetric, connected, k-1 edges.
    size_t degree_sum = 0;
    for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < r.tree[i].size(); ++j) {
            int t = r.tree[i][j];
            if (t < 0 || t >= (int)k || t == (int)i) return false;
            if (std::find(r.tree[t].begin(), r.tree[t].end(), (int)i) == r.tree[t].end()) {
                return false;
            }
        }
        degree_sum += r.tree[i].size();
    }
    if (degree_sum % 2 != 0) return false;
    if (degree_sum / 2 != k - 1) return false;
    {
        std::vector<bool> seen(k, false);
        std::vector<int> stack(1, 0);
        seen[0] = true;
        size_t count = 1;
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (size_t j = 0; j < r.tree[x].size(); ++j) {
                int y = r.tree[x][j];
                if (seen[y]) continue;
                seen[y] = true;
                ++count;
                stack.push_back(y);
            }
        }
        if (count != k) return false;
    }

    // Every vertex is somewhere, and its bags form a connected subtree.
    int width = 0;
    for (size_t i = 0; i < k; ++i) {
        if ((int)r.bags[i].size() > width) width = (int)r.bags[i].size();
    }
    if (r.width != width - 1) return false;

    for (int v = 1; v <= n; ++v) {
        std::vector<int> holding;
        for (size_t i = 0; i < k; ++i) {
            if (std::find(r.bags[i].begin(), r.bags[i].end(), v) != r.bags[i].end()) {
                holding.push_back((int)i);
            }
        }
        if (holding.empty()) return false;

        std::set<int> in_holding(holding.begin(), holding.end());
        std::set<int> seen;
        std::vector<int> stack(1, holding[0]);
        seen.insert(holding[0]);
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (size_t j = 0; j < r.tree[x].size(); ++j) {
                int y = r.tree[x][j];
                if (!in_holding.count(y) || seen.count(y)) continue;
                seen.insert(y);
                stack.push_back(y);
            }
        }
        if (seen.size() != holding.size()) return false;
    }

    // Every edge is inside some bag.
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) continue;
            bool covered = false;
            for (size_t i = 0; i < k && !covered; ++i) {
                bool has_u = std::find(r.bags[i].begin(), r.bags[i].end(), u) != r.bags[i].end();
                bool has_v = std::find(r.bags[i].begin(), r.bags[i].end(), v) != r.bags[i].end();
                covered = has_u && has_v;
            }
            if (!covered) return false;
        }
    }
    return true;
}

bool verify_indifference_order(const Graph& g, const std::vector<int>& order,
                               const std::vector<int>& number) {
    int n = g.n;
    if (static_cast<int>(order.size()) != n + 1) return false;
    if (static_cast<int>(number.size()) != n + 1) return false;

    std::vector<int> seen(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        if (v < 1 || v > n || seen[v]) return false;
        seen[v] = 1;
        if (number[v] != i) return false;
    }

    for (int v = 1; v <= n; ++v) {
        std::vector<int> positions;
        positions.push_back(number[v]);
        for (int u = 1; u <= n; ++u) {
            if (u != v && g.has_edge(u, v)) positions.push_back(number[u]);
        }
        std::sort(positions.begin(), positions.end());
        for (size_t i = 1; i < positions.size(); ++i) {
            if (positions[i] != positions[i - 1] + 1) return false;
        }
    }
    return true;
}

bool verify_tree_layout(const Graph& g, const std::vector<int>& parent) {
    int n = g.n;
    if (static_cast<int>(parent.size()) != n + 1) return false;

    // Walk each vertex to a root, which also rejects cycles.
    std::vector<std::vector<char>> ancestor(n + 1, std::vector<char>(n + 1, 0));
    for (int v = 1; v <= n; ++v) {
        int cur = parent[v];
        int steps = 0;
        while (cur != 0) {
            if (cur < 1 || cur > n) return false;
            if (++steps > n) return false;
            ancestor[cur][v] = 1;
            cur = parent[cur];
        }
    }

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) continue;
            if (!ancestor[u][v] && !ancestor[v][u]) return false;
        }
    }

    for (int x = 1; x <= n; ++x) {
        for (int z = 1; z <= n; ++z) {
            if (!ancestor[x][z] || !g.has_edge(x, z)) continue;
            for (int y = 1; y <= n; ++y) {
                if (!ancestor[x][y] || !ancestor[y][z]) continue;
                if (!g.has_edge(x, y) || !g.has_edge(y, z)) return false;
            }
        }
    }
    return true;
}

bool verify_seo(const Graph& g, const std::vector<int>& order,
                const std::vector<int>& number) {
    int n = g.n;
    if (static_cast<int>(order.size()) != n + 1) return false;
    if (static_cast<int>(number.size()) != n + 1) return false;

    std::vector<int> seen(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        if (v < 1 || v > n || seen[v]) return false;
        seen[v] = 1;
        if (number[v] != i) return false;
    }

    // closed[i][k] = 1 iff the vertex at position k lies in N[order[i]].
    std::vector<std::vector<char>> closed(n + 1, std::vector<char>(n + 1, 0));
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        closed[i][i] = 1;
        for (size_t t = 0; t < g.adj[v].size(); ++t) closed[i][number[g.adj[v][t]]] = 1;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            for (int k = 1; k <= n; ++k) {
                if (!closed[i][k] || !closed[j][k]) continue;
                for (int l = k + 1; l <= n; ++l) {
                    if (closed[i][l] && !closed[j][l]) return false;
                }
            }
        }
    }
    return true;
}

bool verify_circular_arc_model(const Graph& g,
                               const std::vector<std::pair<int, int>>& arcs, int len,
                               bool proper) {
    int n = g.n;
    if (static_cast<int>(arcs.size()) != n + 1) return false;
    if (n > 0 && len <= 0) return false;

    std::vector<std::vector<char>> covers(n + 1, std::vector<char>(len, 0));
    for (int v = 1; v <= n; ++v) {
        int a = arcs[v].first, b = arcs[v].second;
        if (a < 0 || a >= len || b < 0 || b >= len) return false;
        if (a == b) return false;  // an empty or full arc is not a model
        for (int s = 0; s < len; ++s) {
            bool in = a < b ? (a <= s && s < b) : (s >= a || s < b);
            covers[v][s] = in ? 1 : 0;
        }
    }

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            bool meet = false, u_only = false, v_only = false;
            for (int s = 0; s < len; ++s) {
                if (covers[u][s] && covers[v][s]) meet = true;
                else if (covers[u][s]) u_only = true;
                else if (covers[v][s]) v_only = true;
            }
            if (meet != g.has_edge(u, v)) return false;
            if (proper && meet && (!u_only || !v_only)) return false;
        }
    }
    return true;
}

bool verify_krausz_partition(const Graph& g, const LineGraphResult& r) {
    int n = g.n;
    if (static_cast<int>(r.vertex_to_root_edge.size()) != n + 1) return false;

    std::set<std::pair<int, int>> covered;
    std::vector<int> in_parts(n + 1, 0);
    for (size_t i = 0; i < r.krausz.size(); ++i) {
        const std::vector<int>& part = r.krausz[i];
        if (part.size() < 2) return false;
        for (size_t a = 0; a < part.size(); ++a) {
            int u = part[a];
            if (u < 1 || u > n) return false;
            ++in_parts[u];
            for (size_t b = a + 1; b < part.size(); ++b) {
                int v = part[b];
                if (!g.has_edge(u, v)) return false;
                std::pair<int, int> e = u < v ? std::make_pair(u, v) : std::make_pair(v, u);
                if (!covered.insert(e).second) return false;  // edge in two parts
            }
        }
    }
    for (int v = 1; v <= n; ++v) {
        if (in_parts[v] > 2) return false;
    }
    size_t edge_count = 0;
    for (int u = 1; u <= n; ++u) edge_count += g.adj[u].size();
    if (covered.size() != edge_count / 2) return false;

    // The root graph's line graph is g.
    std::set<std::pair<int, int>> root_edges;
    for (int v = 1; v <= n; ++v) {
        std::pair<int, int> e = r.vertex_to_root_edge[v];
        if (e.first < 1 || e.first > r.root_graph.n) return false;
        if (e.second < 1 || e.second > r.root_graph.n) return false;
        if (e.first == e.second) return false;
        if (!r.root_graph.has_edge(e.first, e.second)) return false;
        std::pair<int, int> key = e.first < e.second ? e : std::make_pair(e.second, e.first);
        if (!root_edges.insert(key).second) return false;  // two vertices on one edge
    }
    size_t root_edge_count = 0;
    for (int v = 1; v <= r.root_graph.n; ++v) root_edge_count += r.root_graph.adj[v].size();
    if (root_edges.size() != root_edge_count / 2) return false;

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            const std::pair<int, int>& a = r.vertex_to_root_edge[u];
            const std::pair<int, int>& b = r.vertex_to_root_edge[v];
            bool shares = a.first == b.first || a.first == b.second ||
                          a.second == b.first || a.second == b.second;
            if (shares != g.has_edge(u, v)) return false;
        }
    }
    return true;
}

bool verify_sp_reduction(const Graph& g, const std::vector<SPReduction>& steps) {
    int n = g.n;
    if (static_cast<int>(steps.size()) != n) return false;

    std::vector<std::set<int>> adj(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (size_t i = 0; i < g.adj[v].size(); ++i) adj[v].insert(g.adj[v][i]);
    }
    std::vector<char> alive(n + 1, 1);

    for (size_t i = 0; i < steps.size(); ++i) {
        const SPReduction& s = steps[i];
        int v = s.vertex;
        if (v < 1 || v > n || !alive[v]) return false;
        const std::set<int>& nb = adj[v];

        if (s.kind == 0) {
            if (!nb.empty()) return false;
        } else if (s.kind == 1) {
            if (nb.size() != 1 || *nb.begin() != s.u) return false;
            adj[s.u].erase(v);
        } else if (s.kind == 2 || s.kind == 3) {
            if (nb.size() != 2) return false;
            if (!nb.count(s.u) || !nb.count(s.w)) return false;
            bool had = adj[s.u].count(s.w) != 0;
            if ((s.kind == 3) != had) return false;
            adj[s.u].erase(v);
            adj[s.w].erase(v);
            if (s.kind == 2) {
                adj[s.u].insert(s.w);
                adj[s.w].insert(s.u);
            }
        } else {
            return false;
        }
        adj[v].clear();
        alive[v] = 0;
    }

    for (int v = 1; v <= n; ++v) {
        if (alive[v]) return false;
    }
    return true;
}

bool verify_ktree_construction(const Graph& g, int k, const std::vector<int>& order) {
    int n = g.n;
    if (k < 0 || static_cast<int>(order.size()) != n + 1) return false;
    if (n > 0 && k + 1 > n) return false;

    std::vector<int> seen(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        if (v < 1 || v > n || seen[v]) return false;
        seen[v] = 1;
    }
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        std::vector<int> earlier;
        for (int j = 1; j < i; ++j) {
            if (g.has_edge(v, order[j])) earlier.push_back(order[j]);
        }
        int expected = i <= k + 1 ? i - 1 : k;
        if (static_cast<int>(earlier.size()) != expected) return false;
        for (size_t a = 0; a < earlier.size(); ++a) {
            for (size_t b = a + 1; b < earlier.size(); ++b) {
                if (!g.has_edge(earlier[a], earlier[b])) return false;
            }
        }
    }
    return true;
}

bool verify_split_partition(const Graph& g, const std::vector<int>& side) {
    int n = g.n;
    if (static_cast<int>(side.size()) != n + 1) return false;
    std::vector<int> clique, independent;
    for (int v = 1; v <= n; ++v) {
        if (side[v] == 1) {
            clique.push_back(v);
        } else if (side[v] == 2) {
            independent.push_back(v);
        } else {
            return false;
        }
    }
    for (size_t i = 0; i < clique.size(); ++i) {
        for (size_t j = i + 1; j < clique.size(); ++j) {
            if (!g.has_edge(clique[i], clique[j])) return false;
        }
    }
    for (size_t i = 0; i < independent.size(); ++i) {
        for (size_t j = i + 1; j < independent.size(); ++j) {
            if (g.has_edge(independent[i], independent[j])) return false;
        }
    }
    return true;
}

bool verify_transitive_orientation(const Graph& g, const TransitiveOrientationResult& r) {
    int n = g.n;
    if (static_cast<int>(r.dir.size()) != n + 1) return false;

    // dir agrees with the edge set and is antisymmetric.
    for (int u = 1; u <= n; ++u) {
        if (static_cast<int>(r.dir[u].size()) != n + 1) return false;
        for (int v = 1; v <= n; ++v) {
            if (u == v) {
                if (r.dir[u][v] != 0) return false;
                continue;
            }
            if (!g.has_edge(u, v)) {
                if (r.dir[u][v] != 0) return false;
                continue;
            }
            if (r.dir[u][v] != 1 && r.dir[u][v] != -1) return false;
            if (r.dir[u][v] != -r.dir[v][u]) return false;
        }
    }

    // The arc list lists every edge exactly once, agreeing with dir.
    std::set<std::pair<int, int>> listed;
    for (size_t i = 0; i < r.orientation.size(); ++i) {
        int u = r.orientation[i].first, v = r.orientation[i].second;
        if (u < 1 || u > n || v < 1 || v > n || u == v) return false;
        if (!g.has_edge(u, v)) return false;
        if (r.dir[u][v] != 1) return false;
        std::pair<int, int> key = u < v ? std::make_pair(u, v) : std::make_pair(v, u);
        if (!listed.insert(key).second) return false;
    }
    size_t edge_count = 0;
    for (int u = 1; u <= n; ++u) edge_count += g.adj[u].size();
    if (listed.size() != edge_count / 2) return false;

    // Transitivity, straight from the definition.
    for (int u = 1; u <= n; ++u) {
        for (int v = 1; v <= n; ++v) {
            if (r.dir[u][v] != 1) continue;
            for (int w = 1; w <= n; ++w) {
                if (r.dir[v][w] != 1) continue;
                if (r.dir[u][w] != 1) return false;
            }
        }
    }
    return true;
}

bool verify_md_tree(const Graph& g, const MDTree& t, bool expect_cotree) {
    int n = g.n;
    size_t count = t.nodes.size();
    if (n == 0) return count == 0 && t.root == -1;
    if (count == 0 || t.root < 0 || t.root >= (int)count) return false;
    if (static_cast<int>(t.leaf_of.size()) != n + 1) return false;

    // Parent/child links agree, and only the root has no parent.
    std::vector<int> child_count(count, 0);
    for (size_t i = 0; i < count; ++i) {
        const MDNode& node = t.nodes[i];
        if (node.parent == -1) {
            if ((int)i != t.root) return false;
        } else {
            if (node.parent < 0 || node.parent >= (int)count) return false;
            const std::vector<int>& sibs = t.nodes[node.parent].children;
            if (std::find(sibs.begin(), sibs.end(), (int)i) == sibs.end()) return false;
        }
        for (size_t j = 0; j < node.children.size(); ++j) {
            int c = node.children[j];
            if (c < 0 || c >= (int)count) return false;
            if (t.nodes[c].parent != (int)i) return false;
            ++child_count[c];
        }
    }
    for (size_t i = 0; i < count; ++i) {
        if ((int)i == t.root) {
            if (child_count[i] != 0) return false;
        } else if (child_count[i] != 1) {
            return false;
        }
    }

    // Leaves are exactly the vertices, each once.
    std::vector<int> leaf_seen(n + 1, 0);
    for (size_t i = 0; i < count; ++i) {
        const MDNode& node = t.nodes[i];
        if (node.kind == MDNodeKind::LEAF) {
            if (!node.children.empty()) return false;
            int v = node.vertex;
            if (v < 1 || v > n || leaf_seen[v]) return false;
            leaf_seen[v] = 1;
            if (t.leaf_of[v] != (int)i) return false;
            if (node.vertices.size() != 1 || node.vertices[0] != v) return false;
        } else {
            // An internal node with fewer than two children is not a
            // decomposition step and must not appear.
            if (node.children.size() < 2) return false;
        }
    }
    for (int v = 1; v <= n; ++v) {
        if (!leaf_seen[v]) return false;
    }

    for (size_t i = 0; i < count; ++i) {
        const MDNode& node = t.nodes[i];
        // vertices is the union of the children's, ascending and duplicate-free.
        for (size_t j = 1; j < node.vertices.size(); ++j) {
            if (node.vertices[j - 1] >= node.vertices[j]) return false;
        }
        if (!node.children.empty()) {
            std::vector<int> united;
            for (size_t j = 0; j < node.children.size(); ++j) {
                const std::vector<int>& cv = t.nodes[node.children[j]].vertices;
                united.insert(united.end(), cv.begin(), cv.end());
            }
            std::sort(united.begin(), united.end());
            if (united != node.vertices) return false;
            // Children are ordered by smallest vertex.
            for (size_t j = 1; j < node.children.size(); ++j) {
                const std::vector<int>& prev = t.nodes[node.children[j - 1]].vertices;
                const std::vector<int>& cur = t.nodes[node.children[j]].vertices;
                if (prev.empty() || cur.empty() || prev[0] >= cur[0]) return false;
            }
        }

        // Every node's vertex set is a module of g.
        std::vector<char> inside(n + 1, 0);
        for (size_t j = 0; j < node.vertices.size(); ++j) inside[node.vertices[j]] = 1;
        for (int x = 1; x <= n; ++x) {
            if (inside[x]) continue;
            bool first = true, joined = false;
            for (size_t j = 0; j < node.vertices.size(); ++j) {
                bool adj = g.has_edge(x, node.vertices[j]);
                if (first) {
                    joined = adj;
                    first = false;
                } else if (adj != joined) {
                    return false;
                }
            }
        }

        // The quotient records the adjacency between children, and the label
        // says which of the three shapes it has.
        int k = (int)node.children.size();
        if (node.quotient.n != k) return false;
        int quotient_edges = 0;
        for (int a = 0; a < k; ++a) {
            const std::vector<int>& va = t.nodes[node.children[a]].vertices;
            for (int b = a + 1; b < k; ++b) {
                const std::vector<int>& vb = t.nodes[node.children[b]].vertices;
                bool joined = g.has_edge(va[0], vb[0]);
                for (size_t x = 0; x < va.size(); ++x) {
                    for (size_t y = 0; y < vb.size(); ++y) {
                        if (g.has_edge(va[x], vb[y]) != joined) return false;
                    }
                }
                if (node.quotient.has_edge(a + 1, b + 1) != joined) return false;
                if (joined) ++quotient_edges;
            }
        }
        int pairs = k * (k - 1) / 2;
        switch (node.kind) {
            case MDNodeKind::LEAF:
                if (k != 0) return false;
                break;
            case MDNodeKind::SERIES:
                if (quotient_edges != pairs) return false;
                break;
            case MDNodeKind::PARALLEL:
                if (quotient_edges != 0) return false;
                break;
            case MDNodeKind::PRIME:
                if (expect_cotree) return false;
                if (quotient_edges == 0 || quotient_edges == pairs) return false;
                break;
        }
        // A cotree alternates: a union of unions or a join of joins would not
        // have used maximal (co-)components.
        if (expect_cotree && node.parent >= 0 && t.nodes[node.parent].kind == node.kind) {
            return false;
        }
    }

    // Substituting the children back into the quotients must rebuild g.
    Graph rebuilt = md_rebuild_graph(t, n);
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (rebuilt.has_edge(u, v) != g.has_edge(u, v)) return false;
        }
    }
    return true;
}

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

namespace {

// --- NO-certificate verification ---------------------------------------
//
// Everything below re-derives the definitions rather than calling into
// include/: the point of a certificate is that a bug in the recognizer cannot
// also make the check pass. Adjacency goes through obs_adjacent(), which
// inverts the graph when the certificate lives in the complement, and the
// searches are the naive O(n^2) ones for the same reason.

bool obs_adjacent(const Graph& g, bool in_complement, int u, int v) {
    if (u == v) return false;
    bool e = g.has_edge(u, v);
    return in_complement ? !e : e;
}

// Distinct vertices inside [1, n]; expect == 0 means "any number of them".
bool obs_vertices_valid(const Graph& g, const std::vector<int>& vs, size_t expect) {
    if (expect != 0 && vs.size() != expect) return false;
    std::set<int> seen;
    for (size_t i = 0; i < vs.size(); ++i) {
        if (vs[i] < 1 || vs[i] > g.n) return false;
        if (!seen.insert(vs[i]).second) return false;
    }
    return true;
}

bool obs_reachable(const Graph& g, bool in_complement, int s, int t,
                   const std::vector<char>& blocked) {
    if (s < 1 || s > g.n || t < 1 || t > g.n) return false;
    if (blocked[s] || blocked[t]) return false;
    if (s == t) return true;
    std::vector<char> seen(g.n + 1, 0);
    std::vector<int> stack(1, s);
    seen[s] = 1;
    while (!stack.empty()) {
        int v = stack.back();
        stack.pop_back();
        for (int w = 1; w <= g.n; ++w) {
            if (seen[w] || blocked[w]) continue;
            if (!obs_adjacent(g, in_complement, v, w)) continue;
            if (w == t) return true;
            seen[w] = 1;
            stack.push_back(w);
        }
    }
    return false;
}

int obs_component_count(const Graph& g, bool in_complement,
                        const std::vector<char>& blocked) {
    std::vector<char> seen(g.n + 1, 0);
    int count = 0;
    for (int s = 1; s <= g.n; ++s) {
        if (blocked[s] || seen[s]) continue;
        ++count;
        std::vector<int> stack(1, s);
        seen[s] = 1;
        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            for (int w = 1; w <= g.n; ++w) {
                if (seen[w] || blocked[w]) continue;
                if (!obs_adjacent(g, in_complement, v, w)) continue;
                seen[w] = 1;
                stack.push_back(w);
            }
        }
    }
    return count;
}

// Edge count of a shortest s-t path, or -1 when they are disconnected.
int obs_distance(const Graph& g, bool in_complement, int s, int t) {
    if (s == t) return 0;
    std::vector<int> dist(g.n + 1, -1);
    std::vector<int> queue(1, s);
    dist[s] = 0;
    for (size_t head = 0; head < queue.size(); ++head) {
        int v = queue[head];
        for (int w = 1; w <= g.n; ++w) {
            if (dist[w] >= 0) continue;
            if (!obs_adjacent(g, in_complement, v, w)) continue;
            dist[w] = dist[v] + 1;
            if (w == t) return dist[w];
            queue.push_back(w);
        }
    }
    return -1;
}

enum ObsParity { OBS_ANY_PARITY, OBS_ODD, OBS_EVEN };

// A cycle listed in cyclic order: simple, closed, long enough, right parity,
// and carrying at most max_chords chords.
bool obs_check_cycle_vertices(const Graph& g, bool in_complement,
                              const std::vector<int>& c, size_t min_len,
                              ObsParity parity, int max_chords) {
    if (c.size() < 3 || c.size() < min_len) return false;
    if (!obs_vertices_valid(g, c, 0)) return false;
    if (parity == OBS_ODD && c.size() % 2 == 0) return false;
    if (parity == OBS_EVEN && c.size() % 2 != 0) return false;

    size_t k = c.size();
    for (size_t i = 0; i < k; ++i) {
        if (!obs_adjacent(g, in_complement, c[i], c[(i + 1) % k])) return false;
    }
    int chords = 0;
    for (size_t i = 0; i < k; ++i) {
        for (size_t j = i + 2; j < k; ++j) {
            if (i == 0 && j == k - 1) continue;  // the closing edge, not a chord
            if (obs_adjacent(g, in_complement, c[i], c[j])) ++chords;
        }
    }
    return chords <= max_chords;
}

bool obs_check_cycle(const Graph& g, const Obstruction& o, size_t min_len,
                     ObsParity parity, int max_chords) {
    return obs_check_cycle_vertices(g, o.in_complement, o.vertices, min_len, parity,
                                    max_chords);
}

// A fixed pattern: the listed index pairs must be edges and every other pair a
// non-edge. The tables live at the call sites, restated from the definition of
// each pattern.
bool obs_check_pattern(const Graph& g, const Obstruction& o, size_t size,
                       const int edges[][2], size_t edge_count) {
    if (!obs_vertices_valid(g, o.vertices, size)) return false;
    std::vector<std::vector<char>> want(size, std::vector<char>(size, 0));
    for (size_t i = 0; i < edge_count; ++i) {
        want[edges[i][0]][edges[i][1]] = 1;
        want[edges[i][1]][edges[i][0]] = 1;
    }
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = i + 1; j < size; ++j) {
            bool a = obs_adjacent(g, o.in_complement, o.vertices[i], o.vertices[j]);
            if (a != (want[i][j] != 0)) return false;
        }
    }
    return true;
}

// An induced path listed from one endpoint to the other.
bool obs_check_induced_path(const Graph& g, bool in_complement,
                            const std::vector<int>& p) {
    if (p.size() < 2) return false;
    if (!obs_vertices_valid(g, p, 0)) return false;
    for (size_t i = 0; i + 1 < p.size(); ++i) {
        if (!obs_adjacent(g, in_complement, p[i], p[i + 1])) return false;
    }
    for (size_t i = 0; i < p.size(); ++i) {
        for (size_t j = i + 2; j < p.size(); ++j) {
            if (obs_adjacent(g, in_complement, p[i], p[j])) return false;
        }
    }
    return true;
}

bool obs_check_asteroidal_triple(const Graph& g, const Obstruction& o) {
    if (!obs_vertices_valid(g, o.vertices, 3)) return false;
    int t[3] = {o.vertices[0], o.vertices[1], o.vertices[2]};
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            if (obs_adjacent(g, o.in_complement, t[i], t[j])) return false;
        }
    }
    // Each pair must survive the removal of the third closed neighbourhood.
    for (int k = 0; k < 3; ++k) {
        int z = t[k], x = t[(k + 1) % 3], y = t[(k + 2) % 3];
        std::vector<char> blocked(g.n + 1, 0);
        blocked[z] = 1;
        for (int w = 1; w <= g.n; ++w) {
            if (obs_adjacent(g, o.in_complement, z, w)) blocked[w] = 1;
        }
        if (!obs_reachable(g, o.in_complement, x, y, blocked)) return false;
    }
    return true;
}

bool obs_check_forcing_cycle(const Graph& g, const Obstruction& o) {
    const std::vector<int>& vs = o.vertices;
    if (vs.size() < 6 || vs.size() % 2 != 0) return false;
    size_t k = vs.size() / 2;

    for (size_t i = 0; i < k; ++i) {
        int a = vs[2 * i], b = vs[2 * i + 1];
        if (a < 1 || a > g.n || b < 1 || b > g.n) return false;
        if (!obs_adjacent(g, o.in_complement, a, b)) return false;
    }
    // Gamma: two arcs of a transitive orientation force each other when they
    // share their tail and their heads are non-adjacent, or share their head
    // and their tails are non-adjacent.
    for (size_t i = 0; i + 1 < k; ++i) {
        int a = vs[2 * i], b = vs[2 * i + 1];
        int c = vs[2 * i + 2], d = vs[2 * i + 3];
        bool step = false;
        if (a == c && b != d && !obs_adjacent(g, o.in_complement, b, d)) step = true;
        if (b == d && a != c && !obs_adjacent(g, o.in_complement, a, c)) step = true;
        if (!step) return false;
    }
    // The chain must close on the reverse of the arc it started from.
    return vs[2 * (k - 1)] == vs[1] && vs[2 * (k - 1) + 1] == vs[0];
}

bool obs_check_minor(const Graph& g, const Obstruction& o, size_t branch_count,
                     const int req[][2], size_t req_count) {
    if (o.vertex_sets.size() != branch_count) return false;
    std::vector<char> used(g.n + 1, 0);
    for (size_t i = 0; i < branch_count; ++i) {
        const std::vector<int>& s = o.vertex_sets[i];
        if (s.empty()) return false;
        for (size_t j = 0; j < s.size(); ++j) {
            int v = s[j];
            if (v < 1 || v > g.n || used[v]) return false;
            used[v] = 1;
        }
    }
    for (size_t i = 0; i < branch_count; ++i) {
        const std::vector<int>& s = o.vertex_sets[i];
        std::vector<char> blocked(g.n + 1, 1);
        for (size_t j = 0; j < s.size(); ++j) blocked[s[j]] = 0;
        for (size_t j = 1; j < s.size(); ++j) {
            if (!obs_reachable(g, o.in_complement, s[0], s[j], blocked)) return false;
        }
    }
    for (size_t i = 0; i < req_count; ++i) {
        const std::vector<int>& a = o.vertex_sets[req[i][0]];
        const std::vector<int>& b = o.vertex_sets[req[i][1]];
        bool found = false;
        for (size_t x = 0; x < a.size() && !found; ++x) {
            for (size_t y = 0; y < b.size() && !found; ++y) {
                if (obs_adjacent(g, o.in_complement, a[x], b[y])) found = true;
            }
        }
        if (!found) return false;
    }
    return true;
}

std::set<std::pair<int, int> > obs_cycle_edges(const std::vector<int>& c) {
    std::set<std::pair<int, int> > edges;
    for (size_t i = 0; i < c.size(); ++i) {
        int u = c[i], v = c[(i + 1) % c.size()];
        edges.insert(std::make_pair(u < v ? u : v, u < v ? v : u));
    }
    return edges;
}

bool obs_check_two_cycles(const Graph& g, const Obstruction& o) {
    if (o.vertex_sets.size() != 2 || o.vertices.size() != 2) return false;
    int u = o.vertices[0], v = o.vertices[1];
    if (u < 1 || u > g.n || v < 1 || v > g.n) return false;
    if (!obs_adjacent(g, o.in_complement, u, v)) return false;

    std::pair<int, int> shared(u < v ? u : v, u < v ? v : u);
    for (int i = 0; i < 2; ++i) {
        const std::vector<int>& c = o.vertex_sets[i];
        if (!obs_check_cycle_vertices(g, o.in_complement, c, 3, OBS_ANY_PARITY,
                                      INT_MAX)) {
            return false;
        }
        if (!obs_cycle_edges(c).count(shared)) return false;
    }
    // Two cycles sharing an edge only rule out a cactus when they differ.
    return obs_cycle_edges(o.vertex_sets[0]) != obs_cycle_edges(o.vertex_sets[1]);
}

bool obs_check_parity_paths(const Graph& g, const Obstruction& o) {
    if (o.vertices.size() != 2 || o.vertex_sets.size() != 2) return false;
    int u = o.vertices[0], v = o.vertices[1];
    if (u == v) return false;
    for (int i = 0; i < 2; ++i) {
        const std::vector<int>& p = o.vertex_sets[i];
        if (!obs_check_induced_path(g, o.in_complement, p)) return false;
        if (p.front() != u || p.back() != v) return false;
    }
    size_t l0 = o.vertex_sets[0].size() - 1;
    size_t l1 = o.vertex_sets[1].size() - 1;
    return l0 % 2 != l1 % 2;
}

bool obs_check_non_shortest_path(const Graph& g, const Obstruction& o) {
    const std::vector<int>& p = o.vertices;
    if (!obs_check_induced_path(g, o.in_complement, p)) return false;
    int d = obs_distance(g, o.in_complement, p.front(), p.back());
    if (d < 0) return false;
    return static_cast<int>(p.size()) - 1 > d;
}

}  // namespace

bool verify_obstruction(const Graph& g, const Obstruction& o) {
    // Pattern tables, restated from each definition. Index 0 is the first
    // vertex of Obstruction::vertices.
    static const int kTriangle[][2] = {{0, 1}, {1, 2}, {0, 2}};
    static const int kP3[][2] = {{0, 1}, {1, 2}};
    static const int kP4[][2] = {{0, 1}, {1, 2}, {2, 3}};
    static const int kP5[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
    static const int kTwoK2[][2] = {{0, 1}, {2, 3}};
    static const int kClaw[][2] = {{0, 1}, {0, 2}, {0, 3}};
    static const int kDiamond[][2] = {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}};
    static const int kBull[][2] = {{0, 1}, {1, 2}, {0, 2}, {0, 3}, {1, 4}};
    static const int kGem[][2] = {{0, 1}, {1, 2}, {2, 3}, {4, 0}, {4, 1}, {4, 2}, {4, 3}};
    static const int kK5Pairs[][2] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 2},
                                      {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}};
    static const int kK33Pairs[][2] = {{0, 3}, {0, 4}, {0, 5}, {1, 3}, {1, 4},
                                       {1, 5}, {2, 3}, {2, 4}, {2, 5}};

    switch (o.kind) {
        case ObstructionKind::NONE:
            return false;
        case ObstructionKind::HOLE:
            return obs_check_cycle(g, o, 4, OBS_ANY_PARITY, 0);
        case ObstructionKind::ODD_CYCLE:
            return obs_check_cycle(g, o, 3, OBS_ODD, INT_MAX);
        case ObstructionKind::ODD_HOLE:
            return obs_check_cycle(g, o, 5, OBS_ODD, 0);
        case ObstructionKind::EVEN_HOLE:
            return obs_check_cycle(g, o, 4, OBS_EVEN, 0);
        case ObstructionKind::ODD_CYCLE_LE1_CHORD:
            return obs_check_cycle(g, o, 5, OBS_ODD, 1);
        case ObstructionKind::TWO_CYCLES_SHARING_EDGE:
            return obs_check_two_cycles(g, o);
        case ObstructionKind::INDUCED_PATH_WRONG_PARITY:
            return obs_check_parity_paths(g, o);
        case ObstructionKind::NON_SHORTEST_INDUCED_PATH:
            return obs_check_non_shortest_path(g, o);
        case ObstructionKind::TRIANGLE:
            return obs_check_pattern(g, o, 3, kTriangle, 3);
        case ObstructionKind::P3:
            return obs_check_pattern(g, o, 3, kP3, 2);
        case ObstructionKind::P4:
            return obs_check_pattern(g, o, 4, kP4, 3);
        case ObstructionKind::P5:
            return obs_check_pattern(g, o, 5, kP5, 4);
        case ObstructionKind::C4:
            return obs_check_cycle(g, o, 4, OBS_EVEN, 0) && o.vertices.size() == 4;
        case ObstructionKind::C5:
            return obs_check_cycle(g, o, 5, OBS_ODD, 0) && o.vertices.size() == 5;
        case ObstructionKind::TWO_K2:
            return obs_check_pattern(g, o, 4, kTwoK2, 2);
        case ObstructionKind::CLAW:
            return obs_check_pattern(g, o, 4, kClaw, 3);
        case ObstructionKind::DIAMOND:
            return obs_check_pattern(g, o, 4, kDiamond, 5);
        case ObstructionKind::BULL:
            return obs_check_pattern(g, o, 5, kBull, 5);
        case ObstructionKind::GEM:
            return obs_check_pattern(g, o, 5, kGem, 7);
        case ObstructionKind::ASTEROIDAL_TRIPLE:
            return obs_check_asteroidal_triple(g, o);
        case ObstructionKind::CUT_VERTEX: {
            if (!obs_vertices_valid(g, o.vertices, 1)) return false;
            std::vector<char> none(g.n + 1, 0);
            std::vector<char> without(g.n + 1, 0);
            without[o.vertices[0]] = 1;
            return obs_component_count(g, o.in_complement, without) >
                   obs_component_count(g, o.in_complement, none);
        }
        case ObstructionKind::DISCONNECTED_PAIR: {
            if (!obs_vertices_valid(g, o.vertices, 2)) return false;
            std::vector<char> none(g.n + 1, 0);
            return !obs_reachable(g, o.in_complement, o.vertices[0], o.vertices[1], none);
        }
        case ObstructionKind::FORCING_CYCLE:
            return obs_check_forcing_cycle(g, o);
        case ObstructionKind::K5_MINOR:
            return obs_check_minor(g, o, 5, kK5Pairs, 10);
        case ObstructionKind::K33_MINOR:
            return obs_check_minor(g, o, 6, kK33Pairs, 9);
    }
    return false;
}

}  // namespace gtest_utils
}  // namespace graph_recognition
