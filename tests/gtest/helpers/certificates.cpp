#include "certificates.h"

#include "chordal.h"

#include <algorithm>

namespace graph_recognition {
namespace gtest_utils {

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
    if (static_cast<int>(r.later.size()) < n + 1) return false;
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& lv = r.later[v];
        for (size_t i = 0; i < lv.size(); ++i) {
            for (size_t j = i + 1; j < lv.size(); ++j) {
                if (!g.has_edge(lv[i], lv[j])) return false;
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
