#ifndef GRAPH_RECOGNITION_PERMUTATION_EDGE_ENUM_H
#define GRAPH_RECOGNITION_PERMUTATION_EDGE_ENUM_H

/**
 * @file permutation_edge_enum.h
 * @brief Modular canonical strings and edge-deletion permutation enumeration
 *
 * Implements the fixed-order reverse-search framework of Yamazaki, Saitoh,
 * Kiyomi and Uehara, TCS 806 (2020), sections 3 and 5:
 * https://doi.org/10.1016/j.tcs.2019.04.017
 *
 * A canonical permutation labels the top line 1..n. The parent adds the
 * lexicographically first missing edge that preserves permutation membership,
 * then canonicalizes again. Every noncomplete permutation graph admits such
 * an addition, so the parent relation is a tree rooted at K_n. Its inverse
 * tries only edge deletions, rejecting siblings with equal canonical strings
 * and children whose canonical parent differs from the current graph.
 *
 * The module ordering below is an implementation-specific canonical ordering:
 * series/parallel children are sorted by (size, canonical permutation), and
 * prime quotients try all four diagrams (line exchange and simultaneous
 * reversal), inflating each with the canonical child permutations. Comparing
 * the inflated strings includes the children in prime-node symmetry breaking.
 * No factorial general-purpose graph canonicalization is used here.
 *
 * The existing closure-based modular decomposition and matrix-based
 * recognition/realizer are polynomial, but are not the paper's linear-time
 * primitives. The paper's delay bound is therefore not claimed. DFS has at
 * most n(n-1)/2 edges in a path and polynomial working space (including the
 * per-parent sibling sets); callers may additionally retain all outputs.
 */

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "decompositions/components.h"
#include "decompositions/modular_decomposition.h"
#include "recognizers/permutation.h"

namespace graph_recognition {
namespace detail {

/** @brief Substitute canonical module diagrams into a quotient diagram. */
inline std::vector<int> permutation_inflate_modules(
    const std::vector<int>& children,
    const std::vector<std::vector<int>>& strings,
    const std::vector<int>& top, const std::vector<int>& bottom) {
    std::vector<int> offset(children.size());
    int size = 0;
    for (int child : bottom) {
        offset[child] = size;
        size += static_cast<int>(strings[children[child]].size());
    }
    std::vector<int> result;
    result.reserve(size);
    for (int child : top)
        for (int position : strings[children[child]])
            result.push_back(offset[child] + position);
    return result;
}

/**
 * @brief Isomorphism-invariant permutation of a known permutation graph.
 *
 * The inversion graph of the returned 1-based permutation is isomorphic to g.
 * Equal children of series/parallel nodes can be exchanged freely. A prime
 * permutation quotient has just the four diagram orientations considered
 * below, including any coincidences between them. Children are substituted
 * independently, so reflecting the quotient need not reflect a child.
 */
inline std::vector<int> permutation_canonical_string(const Graph& g) {
    const MDTree tree = modular_decomposition(g);
    if (tree.root < 0) return std::vector<int>();
    std::vector<std::vector<int>> strings(tree.nodes.size());
    for (std::size_t i = tree.nodes.size(); i-- > 0;) {
        const MDNode& node = tree.nodes[i];
        if (node.kind == MDNodeKind::LEAF) {
            strings[i].push_back(1);
            continue;
        }
        const int k = static_cast<int>(node.children.size());
        std::vector<int> top(k), bottom(k);
        if (node.kind != MDNodeKind::PRIME) {
            std::iota(top.begin(), top.end(), 0);
            std::sort(top.begin(), top.end(), [&](int a, int b) {
                const auto& x = strings[node.children[a]];
                const auto& y = strings[node.children[b]];
                return x.size() != y.size() ? x.size() < y.size() : x < y;
            });
            bottom = top;
            if (node.kind == MDNodeKind::SERIES)
                std::reverse(bottom.begin(), bottom.end());
            strings[i] = permutation_inflate_modules(node.children, strings, top, bottom);
            continue;
        }

        const auto diagram = build_permutation_realizer(node.quotient);
        if (!diagram.is_permutation)
            throw std::logic_error("permutation_canonical_string: non-permutation quotient");
        for (int exchange = 0; exchange < 2; ++exchange) {
            for (int reverse = 0; reverse < 2; ++reverse) {
                for (int v = 1; v <= k; ++v) {
                    int a = exchange ? diagram.pos2[v] : diagram.pos1[v];
                    int b = exchange ? diagram.pos1[v] : diagram.pos2[v];
                    if (reverse) {
                        a = k + 1 - a;
                        b = k + 1 - b;
                    }
                    top[a - 1] = v - 1;
                    bottom[b - 1] = v - 1;
                }
                auto candidate = permutation_inflate_modules(node.children, strings, top, bottom);
                if (strings[i].empty() || candidate < strings[i])
                    strings[i] = std::move(candidate);
            }
        }
    }
    return strings[tree.root];
}

/** @brief Sorted edges of the inversion graph, labeled by top-line position. */
inline std::vector<std::pair<int, int>> permutation_string_edges(
    const std::vector<int>& permutation) {
    std::vector<std::pair<int, int>> edges;
    const int n = static_cast<int>(permutation.size());
    for (int u = 0; u < n; ++u)
        for (int v = u + 1; v < n; ++v)
            if (permutation[u] > permutation[v]) edges.emplace_back(u + 1, v + 1);
    return edges;
}

/** @brief Canonical parent of a noncomplete canonical permutation graph. */
inline std::vector<int> permutation_edge_parent(const std::vector<int>& canonical) {
    const int n = static_cast<int>(canonical.size());
    auto edges = permutation_string_edges(canonical);
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (canonical[u] > canonical[v]) continue;
            edges.emplace_back(u + 1, v + 1);
            const Graph parent(n, edges);
            if (check_permutation(parent).is_permutation)
                return permutation_canonical_string(parent);
            edges.pop_back();
        }
    }
    throw std::logic_error("permutation_edge_parent: no parent for a complete graph");
}

template <typename Emit>
inline void permutation_edge_enum_dfs(const std::vector<int>& canonical,
                                      bool connected_only, Emit& emit) {
    const int n = static_cast<int>(canonical.size());
    const auto edges = permutation_string_edges(canonical);
    emit(n, edges);
    std::set<std::vector<int>> child_forms;
    for (std::size_t e = 0; e < edges.size(); ++e) {
        auto child_edges = edges;
        child_edges.erase(child_edges.begin() + e);
        const Graph child(n, child_edges);
        // Deletions cannot reconnect components; every connected child's
        // parent is connected because the parent adds an edge.
        if (connected_only && connected_components(child).count > 1) continue;
        if (!check_permutation(child).is_permutation) continue;
        const auto child_form = permutation_canonical_string(child);
        if (!child_forms.insert(child_form).second) continue;
        if (permutation_edge_parent(child_form) == canonical)
            permutation_edge_enum_dfs(child_form, connected_only, emit);
    }
}

/** @brief Includes disconnected graphs and the empty graph (also if connected_only). */
template <typename Emit>
inline void enumerate_permutation_modular_edges(int n, bool connected_only, Emit emit) {
    std::vector<int> root(n);
    for (int i = 0; i < n; ++i) root[i] = n - i;
    permutation_edge_enum_dfs(root, connected_only, emit);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
