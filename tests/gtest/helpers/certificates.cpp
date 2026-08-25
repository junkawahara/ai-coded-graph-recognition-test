#include "certificates.h"

#include "block_cut_tree.h"
#include "chordal.h"
#include "line_graph.h"
#include "md_tree.h"
#include "series_parallel.h"
#include "transitive_orientation.h"
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

}  // namespace gtest_utils
}  // namespace graph_recognition
