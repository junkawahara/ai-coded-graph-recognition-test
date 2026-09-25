#ifndef GRAPH_RECOGNITION_SNARKHUNTER_ENUM_H
#define GRAPH_RECOGNITION_SNARKHUNTER_ENUM_H

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/snark.h"
#include "util/canonical_augmentation.h"
#include "util/cubic_graph_canonical.h"

namespace graph_recognition {
namespace detail {

// Connected simple cubic intermediates, with zero-based adjacency masks.
typedef std::vector<unsigned long long> SnarkhunterGraph;
typedef std::pair<int, int> SnarkhunterEdge;

inline void snarkhunter_add(SnarkhunterGraph& g, int u, int v) {
    g[u] |= 1ULL << v;
    g[v] |= 1ULL << u;
}

inline void snarkhunter_remove(SnarkhunterGraph& g, int u, int v) {
    g[u] &= ~(1ULL << v);
    g[v] &= ~(1ULL << u);
}

inline std::vector<SnarkhunterEdge> snarkhunter_edges(const SnarkhunterGraph& g) {
    std::vector<SnarkhunterEdge> edges;
    for (int u = 0; u < static_cast<int>(g.size()); ++u)
        for (int v = u + 1; v < static_cast<int>(g.size()); ++v)
            if ((g[u] >> v) & 1ULL) edges.push_back(std::make_pair(u, v));
    return edges;
}

inline std::vector<int> snarkhunter_code(const SnarkhunterGraph& g) {
    return cubic_graph_canonical_code(static_cast<int>(g.size()),
        bitmask_graph_edges(static_cast<int>(g.size()), g));
}

inline SnarkhunterGraph snarkhunter_from_code(const std::vector<int>& code) {
    SnarkhunterGraph g(code.size() / 3, 0);
    for (std::size_t i = 0; i < code.size(); ++i) g[i / 3] |= 1ULL << code[i];
    return g;
}

inline void snarkhunter_subdivide(SnarkhunterGraph& g, SnarkhunterEdge e, int x) {
    snarkhunter_remove(g, e.first, e.second);
    snarkhunter_add(g, e.first, x);
    snarkhunter_add(g, e.second, x);
}

// Brinkmann--Goedgebeur--McKay (2011), Figure 1. Adjacent edge pairs
// implement triangle insertion; they must be retained at intermediate levels.
inline SnarkhunterGraph snarkhunter_insert(const SnarkhunterGraph& g,
    SnarkhunterEdge e, SnarkhunterEdge f) {
    const int n = static_cast<int>(g.size());
    SnarkhunterGraph child = g;
    child.resize(n + 2, 0);
    snarkhunter_subdivide(child, e, n);
    snarkhunter_subdivide(child, f, n + 1);
    snarkhunter_add(child, n, n + 1);
    return child;
}

// Inverse insertion. Reject loops, existing/duplicate edges and disconnected
// results. Testing only whether xy is a bridge is insufficient (Figure 1).
inline bool snarkhunter_reduce(const SnarkhunterGraph& g, int x, int y,
    SnarkhunterGraph& parent) {
    std::vector<int> a, b;
    const int n = static_cast<int>(g.size());
    for (int v = 0; v < n; ++v) {
        if (v != y && ((g[x] >> v) & 1ULL)) a.push_back(v);
        if (v != x && ((g[y] >> v) & 1ULL)) b.push_back(v);
    }
    if (a.size() != 2 || b.size() != 2 || a == b ||
        ((g[a[0]] >> a[1]) & 1ULL) || ((g[b[0]] >> b[1]) & 1ULL)) return false;
    std::vector<int> label(n, -1);
    int next = 0;
    for (int v = 0; v < n; ++v) if (v != x && v != y) label[v] = next++;
    parent.assign(n - 2, 0);
    for (const auto& e : snarkhunter_edges(g))
        if (label[e.first] >= 0 && label[e.second] >= 0)
            snarkhunter_add(parent, label[e.first], label[e.second]);
    snarkhunter_add(parent, label[a[0]], label[a[1]]);
    snarkhunter_add(parent, label[b[0]], label[b[1]]);
    return bitmask_graph_connected(n - 2, parent);
}

inline bool snarkhunter_is_prime(const SnarkhunterGraph& g) {
    SnarkhunterGraph parent;
    for (const auto& e : snarkhunter_edges(g))
        if (snarkhunter_reduce(g, e.first, e.second, parent)) return false;
    return true;
}

// Figure 2(a): subdivide an edge and attach a subdivided K4 by a bridge.
// Figure 2(b): replace an edge by K4-e, attaching its two degree-2 vertices.
// Figure 2(c): subdivide two edges and join the new vertices through K4-e.
inline SnarkhunterGraph snarkhunter_prime_expand(const SnarkhunterGraph& g,
    int operation, SnarkhunterEdge e, SnarkhunterEdge f = SnarkhunterEdge()) {
    const int n = static_cast<int>(g.size());
    SnarkhunterGraph child = g;
    child.resize(n + (operation == 1 ? 4 : 6), 0);
    if (operation == 0) {
        snarkhunter_subdivide(child, e, n);
        for (int u = n + 2; u < n + 6; ++u)
            for (int v = u + 1; v < n + 6; ++v) snarkhunter_add(child, u, v);
        snarkhunter_subdivide(child, std::make_pair(n + 2, n + 3), n + 1);
        snarkhunter_add(child, n, n + 1);
    } else {
        const int d = operation == 1 ? n : n + 2;
        for (int u = d; u < d + 4; ++u)
            for (int v = u + 1; v < d + 4; ++v)
                if (u != d || v != d + 3) snarkhunter_add(child, u, v);
        if (operation == 1) {
            snarkhunter_remove(child, e.first, e.second);
            snarkhunter_add(child, e.first, d);
            snarkhunter_add(child, e.second, d + 3);
        } else {
            snarkhunter_subdivide(child, e, n);
            snarkhunter_subdivide(child, f, n + 1);
            snarkhunter_add(child, n, d);
            snarkhunter_add(child, n + 1, d + 3);
        }
    }
    return child;
}

// Lemma 2.2: all primes arise from K4 by these three operations, keeping
// only prime children. Level sets suffice here: the primes are very sparse.
inline std::vector<std::set<std::vector<int> > > snarkhunter_primes(int n) {
    std::vector<std::set<std::vector<int> > > levels(n + 1);
    if (n < 4) return levels;
    SnarkhunterGraph root(4, 0);
    for (int u = 0; u < 4; ++u)
        for (int v = u + 1; v < 4; ++v) snarkhunter_add(root, u, v);
    levels[4].insert(snarkhunter_code(root));
    for (int k = 4; k + 4 <= n; k += 2) {
        for (const auto& code : levels[k]) {
            const auto g = snarkhunter_from_code(code);
            const auto edges = snarkhunter_edges(g);
            const auto accept = [&](SnarkhunterGraph child) {
                if (snarkhunter_is_prime(child))
                    levels[child.size()].insert(snarkhunter_code(child));
            };
            for (std::size_t i = 0; i < edges.size(); ++i) {
                accept(snarkhunter_prime_expand(g, 1, edges[i]));
                if (k + 6 > n) continue;
                accept(snarkhunter_prime_expand(g, 0, edges[i]));
                for (std::size_t j = i + 1; j < edges.size(); ++j)
                    accept(snarkhunter_prime_expand(g, 2, edges[i], edges[j]));
            }
        }
    }
    return levels;
}

inline int snarkhunter_popcount(unsigned int mask) {
    int count = 0;
    for (; mask; mask &= mask - 1) ++count;
    return count;
}

// Smallest available colour domain first. On success the colouring itself
// is retained for the even-2-factor look-ahead, unlike a boolean recognizer.
inline bool snarkhunter_color_dfs(const std::vector<SnarkhunterEdge>& edges,
    std::vector<int>& colors, std::vector<unsigned int>& used, int remaining) {
    if (!remaining) return true;
    int best = -1, choices = 4;
    unsigned int allowed = 0;
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        if (colors[i] >= 0) continue;
        const unsigned int mask = 7U & ~(used[edges[i].first] | used[edges[i].second]);
        const int count = snarkhunter_popcount(mask);
        if (!count) return false;
        if (count < choices) { best = i; choices = count; allowed = mask; }
    }
    const auto e = edges[best];
    for (int c = 0; c < 3; ++c) {
        const unsigned int bit = 1U << c;
        if (!(allowed & bit)) continue;
        colors[best] = c;
        used[e.first] |= bit;
        used[e.second] |= bit;
        if (snarkhunter_color_dfs(edges, colors, used, remaining - 1)) return true;
        used[e.first] &= ~bit;
        used[e.second] &= ~bit;
    }
    colors[best] = -1;
    return false;
}

inline bool snarkhunter_coloring(int n, const std::vector<SnarkhunterEdge>& edges,
    std::vector<int>& colors) {
    colors.assign(edges.size(), -1);
    std::vector<unsigned int> used(n, 0);
    int remaining = static_cast<int>(edges.size()), c = 0;
    // Fix all colours at vertex zero; every colouring has such a permutation.
    for (std::size_t i = 0; i < edges.size(); ++i) {
        if (edges[i].first != 0 && edges[i].second != 0) continue;
        colors[i] = c;
        used[edges[i].first] |= 1U << c;
        used[edges[i].second] |= 1U << c;
        ++c;
        --remaining;
    }
    return snarkhunter_color_dfs(edges, colors, used, remaining);
}

// Theorem 3.3 of Brinkmann et al. (2013). A row labels the even cycles
// using two colours; edges of the excluded colour have label -1.
inline std::vector<std::vector<int> > snarkhunter_even_cycles(int n,
    const std::vector<SnarkhunterEdge>& edges, const std::vector<int>& colors) {
    std::vector<std::vector<int> > cycles(3, std::vector<int>(edges.size(), -1));
    for (int excluded = 0; excluded < 3; ++excluded) {
        std::vector<std::vector<int> > adj(n);
        for (std::size_t i = 0; i < edges.size(); ++i) if (colors[i] != excluded) {
            adj[edges[i].first].push_back(edges[i].second);
            adj[edges[i].second].push_back(edges[i].first);
        }
        std::vector<int> component(n, -1);
        for (int s = 0; s < n; ++s) {
            if (component[s] >= 0) continue;
            std::vector<int> queue(1, s);
            component[s] = s;
            for (std::size_t i = 0; i < queue.size(); ++i)
                for (int w : adj[queue[i]]) if (component[w] < 0) {
                    component[w] = s;
                    queue.push_back(w);
                }
        }
        for (std::size_t i = 0; i < edges.size(); ++i)
            if (colors[i] != excluded) cycles[excluded][i] = component[edges[i].first];
    }
    return cycles;
}

inline std::vector<std::vector<int> > snarkhunter_lookahead(int n,
    const std::vector<SnarkhunterEdge>& edges) {
    std::vector<int> colors;
    if (!snarkhunter_coloring(n, edges, colors)) return {};
    auto cycles = snarkhunter_even_cycles(n, edges, colors);
    // One Kempe exchange on a non-Hamiltonian two-colour cycle yields two
    // further 2-factors. It is only a certificate of colorability: a pair
    // not rejected still needs an exact terminal coloring test.
    for (int excluded = 0; excluded < 3; ++excluded) {
        for (int label = 0; label < n; ++label) {
            const int length = static_cast<int>(std::count(
                cycles[excluded].begin(), cycles[excluded].end(), label));
            if (!length || length == n) continue;
            auto changed = colors;
            for (std::size_t i = 0; i < edges.size(); ++i)
                if (cycles[excluded][i] == label) changed[i] = 3 - excluded - colors[i];
            auto extra = snarkhunter_even_cycles(n, edges, changed);
            cycles.insert(cycles.end(), extra.begin(), extra.end());
            return cycles;
        }
    }
    return cycles;
}

inline bool snarkhunter_colorable_pair(const std::vector<std::vector<int> >& cycles,
    std::size_t a, std::size_t b) {
    for (const auto& row : cycles) if (row[a] >= 0 && row[a] == row[b]) return true;
    return false;
}

// Each original short cycle survives every insertion, with at most two of
// its edges subdivided per step. Edge-disjoint cycles have additive deficits.
inline bool snarkhunter_girth_feasible(const SnarkhunterGraph& g, int steps) {
    const int n = static_cast<int>(g.size());
    SnarkhunterGraph occupied(n, 0);
    int deficit = 0;
    for (int length = 3; length <= 4; ++length) {
        for (int a = 0; a < n; ++a) for (int b = a + 1; b < n; ++b) {
            if (!((g[a] >> b) & 1ULL)) continue;
            for (int c = a + 1; c < n; ++c) {
                if (c == b || !((g[b] >> c) & 1ULL)) continue;
                for (int d = (length == 3 ? a : b + 1);
                     d < (length == 3 ? a + 1 : n); ++d) {
                    if (d == c || !((g[c] >> d) & 1ULL) ||
                        (length == 4 && !((g[d] >> a) & 1ULL))) continue;
                    // A triangle cannot disappear in the final girth-5
                    // insertion: subdividing two of its edges creates a triangle.
                    if (length == 3 && steps <= 1) return false;
                    const int vertices[] = {a, b, c, d, a};
                    bool disjoint = true;
                    for (int i = 0; i < length; ++i)
                        if ((occupied[vertices[i]] >> vertices[i + 1]) & 1ULL)
                            disjoint = false;
                    if (!disjoint) continue;
                    deficit += 5 - length;
                    if (deficit > 2 * steps) return false;
                    for (int i = 0; i < length; ++i)
                        snarkhunter_add(occupied, vertices[i], vertices[i + 1]);
                }
            }
        }
    }
    return true;
}

struct SnarkhunterStats {
    std::size_t edge_pairs = 0;
    std::size_t lookahead_rejections = 0;
    std::size_t terminal_color_tests = 0;
    std::size_t canonicalizations = 0;
};

// The canonical graph's first reducible edge fixes a unique parent class.
// Matching parent codes and suppressing equal siblings gives one path per
// class, independently of the labeling and of which prime root it reaches.
inline bool snarkhunter_has_parent(const std::vector<int>& child_code,
    const std::vector<int>& parent_code) {
    const auto child = snarkhunter_from_code(child_code);
    SnarkhunterGraph parent;
    for (const auto& e : snarkhunter_edges(child))
        if (snarkhunter_reduce(child, e.first, e.second, parent))
            return snarkhunter_code(parent) == parent_code;
    return false;
}

template <typename Callback>
inline void snarkhunter_extend(const std::vector<int>& code, int target,
    bool snarks_only, bool lookahead, SnarkhunterStats& stats, Callback& emit) {
    const auto g = snarkhunter_from_code(code);
    const int n = static_cast<int>(g.size());
    if (n == target) { emit(bitmask_graph_edges(n, g)); return; }
    if (snarks_only && !snarkhunter_girth_feasible(g, (target - n) / 2)) return;
    const auto edges = snarkhunter_edges(g);
    const bool terminal = n + 2 == target;
    std::vector<std::vector<int> > cycles;
    if (snarks_only && terminal && lookahead) cycles = snarkhunter_lookahead(n, edges);
    std::set<std::vector<int> > siblings;
    for (std::size_t a = 0; a < edges.size(); ++a) {
        for (std::size_t b = a + 1; b < edges.size(); ++b) {
            ++stats.edge_pairs;
            if (snarks_only && terminal && snarkhunter_colorable_pair(cycles, a, b)) {
                ++stats.lookahead_rejections;
                continue;
            }
            const auto child = snarkhunter_insert(g, edges[a], edges[b]);
            if (snarks_only && !snarkhunter_girth_feasible(child, (target - n - 2) / 2)) continue;
            if (snarks_only && terminal) {
                ++stats.terminal_color_tests;
                std::vector<int> colors;
                if (snarkhunter_coloring(n + 2, snarkhunter_edges(child), colors)) continue;
                const Graph graph(n + 2, bitmask_graph_edges(n + 2, child));
                if (has_bridge(graph) || !is_cyclically_4_edge_connected(graph)) continue;
            }
            ++stats.canonicalizations;
            auto child_code = snarkhunter_code(child);
            if (!siblings.insert(child_code).second || !snarkhunter_has_parent(child_code, code)) continue;
            snarkhunter_extend(child_code, target, snarks_only, lookahead, stats, emit);
        }
    }
}

// The cubic mode supports independent coverage tests of the construction,
// including the prime roots, before applying any snark-specific pruning.
template <typename Callback>
inline void snarkhunter_enumerate(int n, Callback emit, bool snarks_only = true,
    bool lookahead = true, SnarkhunterStats* measurements = 0) {
    if (measurements) *measurements = SnarkhunterStats();
    if (n < (snarks_only ? 10 : 4) || n % 2) return;
    SnarkhunterStats stats;
    // A prime has a bridge or a diamond, hence cannot be a snark.
    const auto primes = snarkhunter_primes(snarks_only ? n - 2 : n);
    for (const auto& level : primes)
        for (const auto& code : level)
            snarkhunter_extend(code, n, snarks_only, lookahead, stats, emit);
    if (measurements) *measurements = stats;
}

}  // namespace detail
}  // namespace graph_recognition

#endif
