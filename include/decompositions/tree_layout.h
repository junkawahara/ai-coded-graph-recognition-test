#ifndef GRAPH_RECOGNITION_TREE_LAYOUT_H
#define GRAPH_RECOGNITION_TREE_LAYOUT_H

/**
 * @file tree_layout.h
 * @brief Indifference tree-layouts and their block trees
 *
 * An indifference tree-layout of a graph is a rooted tree on its vertices in
 * which every vertex's neighbourhood is an interval of the root-to-vertex path
 * extended downwards -- the tree generalization of the vertex order that
 * defines proper interval graphs. A chordal graph admits one exactly when it
 * is proper chordal (Paul & Protopapas, STACS 2024).
 *
 * The construction goes through the *block tree* of a layout rooted at a given
 * vertex (Algorithm 1 of the paper), whose blocks are then checked for the
 * nested-convex condition (Algorithm 2).
 *
 * Complexity: the algorithm of the paper is O(n^4), but the nested-convex
 * verification here enumerates all vertex orderings of a block
 * (std::next_permutation), so the worst case is factorial in the block size.
 */

#include "util/graph.h"
#include <algorithm>
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Block tree of an indifference tree-layout
 */
struct LayoutBlockTree {
    std::vector<std::vector<int>> blocks;   /**< blocks[i] = vertex set of block i */
    std::vector<int> parent;                /**< parent[i] = parent block, -1 at the root */
    std::vector<std::vector<int>> children; /**< children[i] = child blocks of block i */
    std::vector<int> depth;                 /**< depth[i] = depth of block i */
    std::vector<int> block_of;              /**< block_of[v] = block holding vertex v (size n+1) */
    bool success = false;                   /**< false if no block tree exists for this root */
};

/**
 * @brief An indifference tree-layout
 */
struct TreeLayoutResult {
    bool success = false;    /**< true if the graph admits an indifference tree-layout */
    /**
     * @brief parent[v] = the parent of v in the layout, 0 at a root (size n+1)
     *
     * Valid only when success == true.
     */
    std::vector<int> parent;
};

namespace detail_tree_layout {

/**
 * @brief Returns connected components of vertex_set excluding the excluded subset
 */
inline std::vector<std::vector<int>> find_components_in_subset(
    const Graph& g,
    const std::vector<bool>& in_subset,
    const std::vector<bool>& excluded) {
    int n = g.n;
    std::vector<bool> visited(n + 1, false);
    std::vector<std::vector<int>> components;

    for (int v = 1; v <= n; ++v) {
        if (!in_subset[v] || excluded[v] || visited[v]) continue;
        std::vector<int> comp;
        std::queue<int> q;
        q.push(v);
        visited[v] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            comp.push_back(u);
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (in_subset[w] && !excluded[w] && !visited[w]) {
                    visited[w] = true;
                    q.push(w);
                }
            }
        }
        components.push_back(comp);
    }
    return components;
}

/**
 * @brief Algorithm 1: Block tree computation
 *
 * Computes the block tree of the indifference tree-layout rooted at vertex root.
 * Returns success = false on failure.
 */
inline LayoutBlockTree compute_layout_block_tree(const Graph& g, int root) {
    int n = g.n;
    LayoutBlockTree bt;
    bt.block_of.assign(n + 1, -1);
    bt.success = false;

    // S: set of processed vertices
    std::vector<bool> in_S(n + 1, false);
    in_S[root] = true;
    int s_count = 1;

    // First block: {root}
    bt.blocks.push_back(std::vector<int>(1, root));
    bt.parent.push_back(-1);
    bt.children.push_back(std::vector<int>());
    bt.depth.push_back(0);
    bt.block_of[root] = 0;

    while (s_count < n) {
        // Find connected components of G - S
        std::vector<bool> not_S(n + 1, false);
        for (int v = 1; v <= n; ++v) {
            if (!in_S[v]) not_S[v] = true;
        }
        std::vector<bool> empty_excl(n + 1, false);
        std::vector<std::vector<int>> comps =
            find_components_in_subset(g, not_S, empty_excl);

        bool found_any = false;
        for (size_t ci = 0; ci < comps.size(); ++ci) {
            const std::vector<int>& comp = comps[ci];

            // NS_C: vertices in comp that are adjacent to S
            std::vector<int> ns_c;
            std::vector<bool> in_comp(n + 1, false);
            for (size_t i = 0; i < comp.size(); ++i) in_comp[comp[i]] = true;

            for (size_t i = 0; i < comp.size(); ++i) {
                int v = comp[i];
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    if (in_S[g.adj[v][j]]) {
                        ns_c.push_back(v);
                        break;
                    }
                }
            }

            // S-maximal vertex: N(v) ∩ S is maximal by inclusion
            // First find the largest N(v) ∩ S
            std::vector<bool> best_ns(n + 1, false);
            int best_ns_size = -1;

            for (size_t i = 0; i < comp.size(); ++i) {
                int v = comp[i];
                std::vector<bool> nv_s(n + 1, false);
                int nv_s_size = 0;
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_S[u]) { nv_s[u] = true; nv_s_size++; }
                }
                // Check if nv_s is a superset of best_ns
                if (nv_s_size > best_ns_size) {
                    // Verify nv_s is a superset of best_ns
                    bool is_super = true;
                    for (int u = 1; u <= n; ++u) {
                        if (best_ns[u] && !nv_s[u]) { is_super = false; break; }
                    }
                    if (is_super) {
                        best_ns = nv_s;
                        best_ns_size = nv_s_size;
                    }
                }
            }

            // Collect S-maximal vertices
            std::vector<int> s_maximal;
            for (size_t i = 0; i < comp.size(); ++i) {
                int v = comp[i];
                bool match = true;
                for (int u = 1; u <= n; ++u) {
                    if (best_ns[u] && !g.has_edge(v, u)) { match = false; break; }
                    if (!best_ns[u] && in_S[u] && g.has_edge(v, u)) { match = false; break; }
                }
                if (match) s_maximal.push_back(v);
            }

            // NS_C-universal: adjacent to all vertices in ns_c (except itself)
            std::vector<bool> in_ns_c(n + 1, false);
            for (size_t i = 0; i < ns_c.size(); ++i) in_ns_c[ns_c[i]] = true;

            std::vector<int> block;
            for (size_t i = 0; i < s_maximal.size(); ++i) {
                int v = s_maximal[i];
                bool universal = true;
                for (size_t j = 0; j < ns_c.size(); ++j) {
                    int u = ns_c[j];
                    if (u != v && !g.has_edge(v, u)) {
                        universal = false;
                        break;
                    }
                }
                if (universal) block.push_back(v);
            }

            if (block.empty()) continue;

            // Found S-block X = block
            // Update S
            for (size_t i = 0; i < block.size(); ++i) in_S[block[i]] = true;
            s_count += (int)block.size();

            // Compute N(X)
            std::vector<bool> nx(n + 1, false);
            for (size_t i = 0; i < block.size(); ++i) {
                for (size_t j = 0; j < g.adj[block[i]].size(); ++j) {
                    nx[g.adj[block[i]][j]] = true;
                }
            }

            // Find deepest block B with N(X) ∩ B non-empty
            int best_parent = -1;
            int best_depth = -1;
            for (size_t bi = 0; bi < bt.blocks.size(); ++bi) {
                bool intersects = false;
                for (size_t vi = 0; vi < bt.blocks[bi].size(); ++vi) {
                    if (nx[bt.blocks[bi][vi]]) { intersects = true; break; }
                }
                if (intersects && bt.depth[bi] > best_depth) {
                    best_depth = bt.depth[bi];
                    best_parent = (int)bi;
                }
            }

            if (best_parent < 0) {
                // Defensive: a partially built bt would mislead the caller
                // because bt.success stays false but bt.blocks is non-empty.
                bt.blocks.clear();
                bt.parent.clear();
                bt.children.clear();
                bt.depth.clear();
                bt.block_of.assign(n + 1, -1);
                return bt;
            }

            int new_idx = (int)bt.blocks.size();
            bt.blocks.push_back(block);
            bt.parent.push_back(best_parent);
            bt.children.push_back(std::vector<int>());
            bt.depth.push_back(best_depth + 1);
            bt.children[best_parent].push_back(new_idx);
            for (size_t i = 0; i < block.size(); ++i) bt.block_of[block[i]] = new_idx;

            found_any = true;
            break; // After processing one block, loop again
        }

        if (!found_any) {
            // Same reasoning as above: clear the partial state on failure.
            bt.blocks.clear();
            bt.parent.clear();
            bt.children.clear();
            bt.depth.clear();
            bt.block_of.assign(n + 1, -1);
            return bt;
        }
    }

    bt.success = true;
    return bt;
}

/**
 * @brief Determines whether a collection is nested (pairwise comparable by inclusion)
 */
inline bool is_nested_collection(const std::vector<std::vector<bool>>& sets, int n) {
    for (size_t i = 0; i < sets.size(); ++i) {
        for (size_t j = i + 1; j < sets.size(); ++j) {
            // i ⊆ j ?
            bool i_sub_j = true, j_sub_i = true;
            for (int v = 1; v <= n; ++v) {
                if (sets[i][v] && !sets[j][v]) i_sub_j = false;
                if (sets[j][v] && !sets[i][v]) j_sub_i = false;
            }
            if (!i_sub_j && !j_sub_i) return false;
        }
    }
    return true;
}

/**
 * @brief Determines whether elements of set s are consecutive in permutation perm
 */
inline bool is_consecutive_in_perm(const std::vector<bool>& s,
                                   const std::vector<int>& perm) {
    int first = -1, last = -1;
    int count = 0;
    for (size_t i = 0; i < perm.size(); ++i) {
        if (s[perm[i]]) {
            if (first < 0) first = (int)i;
            last = (int)i;
            count++;
        }
    }
    if (count == 0) return true;
    return (last - first + 1) == count;
}

/**
 * @brief Brute-force verification of Algorithm 2 nested-convex condition
 *
 * Among permutations of vertices of block B, checks if there exists one where
 * all N(y) ∩ B are consecutive and Y \\ Z precedes Z for nested sets Y > Z in the same component.
 */
inline bool check_nested_convex_brute(
    const std::vector<int>& block_vertices,
    const std::vector<std::vector<std::vector<bool>>>& component_sets,
    int n,
    std::vector<int>* witness_order = 0) {
    int bsize = (int)block_vertices.size();
    if (bsize <= 1) {
        if (witness_order != 0) *witness_order = block_vertices;
        return true;
    }

    // Collect all sets
    std::vector<std::vector<bool>> all_sets;
    std::vector<int> set_comp; // Which component it belongs to
    for (size_t ci = 0; ci < component_sets.size(); ++ci) {
        for (size_t si = 0; si < component_sets[ci].size(); ++si) {
            all_sets.push_back(component_sets[ci][si]);
            set_comp.push_back((int)ci);
        }
    }

    // Try all permutations of block_vertices
    std::vector<int> perm = block_vertices;
    std::sort(perm.begin(), perm.end());

    do {
        // Are all sets consecutive?
        bool all_consecutive = true;
        for (size_t i = 0; i < all_sets.size() && all_consecutive; ++i) {
            if (!is_consecutive_in_perm(all_sets[i], perm)) {
                all_consecutive = false;
            }
        }
        if (!all_consecutive) continue;

        // C-nested condition: for nested Y > Z in the same component, Y\Z comes before Z
        bool c_nested = true;
        for (size_t ci = 0; ci < component_sets.size() && c_nested; ++ci) {
            const std::vector<std::vector<bool>>& ni = component_sets[ci];
            for (size_t a = 0; a < ni.size() && c_nested; ++a) {
                for (size_t b = 0; b < ni.size() && c_nested; ++b) {
                    if (a == b) continue;
                    // Is ni[a] a strict subset of ni[b]?
                    bool a_sub_b = true, b_sub_a = true;
                    for (int v = 1; v <= n; ++v) {
                        if (ni[a][v] && !ni[b][v]) a_sub_b = false;
                        if (ni[b][v] && !ni[a][v]) b_sub_a = false;
                    }
                    if (!a_sub_b || b_sub_a) continue;
                    // ni[a] is a strict subset of ni[b]: Z=ni[a], Y=ni[b]
                    // All elements of Y\Z must precede all elements of Z
                    int last_diff = -1; // Last position of Y\Z
                    int first_z = (int)perm.size(); // First position of Z
                    for (int pi = 0; pi < (int)perm.size(); ++pi) {
                        int v = perm[pi];
                        if (ni[b][v] && !ni[a][v]) {
                            // Element of Y\Z
                            if (pi > last_diff) last_diff = pi;
                        }
                        if (ni[a][v]) {
                            // Element of Z
                            if (pi < first_z) first_z = pi;
                        }
                    }
                    if (last_diff >= 0 && first_z < (int)perm.size()) {
                        if (last_diff >= first_z) c_nested = false;
                    }
                }
            }
        }
        if (c_nested) {
            if (witness_order != 0) *witness_order = perm;
            return true;
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    return false;
}

/**
 * @brief Algorithm 2: Block tree verification
 *
 * For each block B:
 * 1. Compute C_B (the "cone" of vertices below B)
 * 2. Obtain connected components C_1, ..., C_k of G[C_B] - B
 * 3. Compute N_i = {N(y) ∩ B | y in C_i} for each C_i
 * 4. Verify each N_i is nested
 * 5. Verify nested-convex condition
 */
inline bool verify_block_tree(
    const Graph& g,
    const LayoutBlockTree& bt,
    std::vector<std::vector<int>>* witness_orders = 0) {
    int n = g.n;
    if (witness_orders != 0) {
        witness_orders->assign(bt.blocks.size(), std::vector<int>());
    }

    for (size_t bi = 0; bi < bt.blocks.size(); ++bi) {
        const std::vector<int>& B = bt.blocks[bi];

        // A_B: all vertices of ancestor blocks
        std::vector<bool> in_ancestors(n + 1, false);
        {
            int cur = bt.parent[bi];
            while (cur >= 0) {
                for (size_t i = 0; i < bt.blocks[cur].size(); ++i) {
                    in_ancestors[bt.blocks[cur][i]] = true;
                }
                cur = bt.parent[cur];
            }
        }

        // C_B: connected component containing B in G - A_B
        std::vector<bool> not_ancestor(n + 1, false);
        for (int v = 1; v <= n; ++v) {
            if (!in_ancestors[v]) not_ancestor[v] = true;
        }
        std::vector<bool> empty_excl(n + 1, false);
        std::vector<std::vector<int>> cone_comps =
            find_components_in_subset(g, not_ancestor, empty_excl);

        // Find component containing first vertex of B
        std::vector<bool> in_cone(n + 1, false);
        for (size_t ci = 0; ci < cone_comps.size(); ++ci) {
            bool contains_b = false;
            for (size_t i = 0; i < cone_comps[ci].size(); ++i) {
                if (cone_comps[ci][i] == B[0]) { contains_b = true; break; }
            }
            if (contains_b) {
                for (size_t i = 0; i < cone_comps[ci].size(); ++i) {
                    in_cone[cone_comps[ci][i]] = true;
                }
                break;
            }
        }

        // Connected components of G[C_B] - B
        std::vector<bool> in_B(n + 1, false);
        for (size_t i = 0; i < B.size(); ++i) in_B[B[i]] = true;

        std::vector<std::vector<int>> sub_comps =
            find_components_in_subset(g, in_cone, in_B);

        if (sub_comps.empty()) {
            // No children -- every order works; choose a deterministic one.
            if (witness_orders != 0) {
                (*witness_orders)[bi] = B;
                std::sort((*witness_orders)[bi].begin(),
                          (*witness_orders)[bi].end());
            }
            continue;
        }

        // Compute N_i for each component C_i
        std::vector<std::vector<std::vector<bool>>> component_sets;
        for (size_t ci = 0; ci < sub_comps.size(); ++ci) {
            std::vector<std::vector<bool>> ni_sets;
            // Deduplication
            for (size_t yi = 0; yi < sub_comps[ci].size(); ++yi) {
                int y = sub_comps[ci][yi];
                std::vector<bool> ny_cap_B(n + 1, false);
                bool non_empty = false;
                for (size_t j = 0; j < g.adj[y].size(); ++j) {
                    if (in_B[g.adj[y][j]]) {
                        ny_cap_B[g.adj[y][j]] = true;
                        non_empty = true;
                    }
                }
                if (!non_empty) continue; // Ignore empty set
                // Duplicate check
                bool dup = false;
                for (size_t si = 0; si < ni_sets.size(); ++si) {
                    bool same = true;
                    for (size_t vi = 0; vi < B.size(); ++vi) {
                        if (ni_sets[si][B[vi]] != ny_cap_B[B[vi]]) {
                            same = false; break;
                        }
                    }
                    if (same) { dup = true; break; }
                }
                if (!dup) ni_sets.push_back(ny_cap_B);
            }
            // Is N_i nested?
            if (!is_nested_collection(ni_sets, n)) return false;
            component_sets.push_back(ni_sets);
        }

        // Nested-convex condition
        std::vector<int>* order = witness_orders == 0
            ? static_cast<std::vector<int>*>(0)
            : &(*witness_orders)[bi];
        if (!check_nested_convex_brute(B, component_sets, n, order)) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Verifies the indifference conditions for an explicit rooted tree
 */
inline bool is_indifference_tree_layout(
    const Graph& g,
    const std::vector<int>& tree_parent) {
    const int n = g.n;
    if (tree_parent.size() != static_cast<std::size_t>(n + 1)) return false;

    std::vector<std::vector<bool>> ancestor(
        n + 1, std::vector<bool>(n + 1, false));
    for (int v = 1; v <= n; ++v) {
        int cur = tree_parent[v];
        int steps = 0;
        while (cur != 0) {
            if (cur < 1 || cur > n || ++steps > n) return false;
            ancestor[cur][v] = true;
            cur = tree_parent[cur];
        }
    }

    // Every graph edge must join comparable tree nodes.
    for (int u = 1; u <= n; ++u) {
        for (std::size_t i = 0; i < g.adj[u].size(); ++i) {
            const int v = g.adj[u][i];
            if (u < v && !ancestor[u][v] && !ancestor[v][u]) return false;
        }
    }

    // Theorem 6: on each ancestor chain x < y < z, xz implies xy and yz.
    for (int x = 1; x <= n; ++x) {
        for (int z = 1; z <= n; ++z) {
            if (!ancestor[x][z] || !g.has_edge(x, z)) continue;
            for (int y = 1; y <= n; ++y) {
                if (ancestor[x][y] && ancestor[y][z] &&
                    (!g.has_edge(x, y) || !g.has_edge(y, z))) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * @brief Constructs one deterministic indifference tree-layout of a
 *        connected graph, if one exists
 *
 * Each block is replaced by the lexicographically first nested-convex order.
 * A child block is attached to the last vertex of its parent block adjacent
 * to the child block, as in the block-tree extension used in Theorem 14.
 */
inline bool find_connected_indifference_tree_layout(
    const Graph& g,
    std::vector<int>* tree_parent) {
    const int n = g.n;
    tree_parent->clear();
    if (n == 0) {
        tree_parent->assign(1, 0);
        return true;
    }

    for (int root = 1; root <= n; ++root) {
        const LayoutBlockTree bt = compute_layout_block_tree(g, root);
        if (!bt.success) continue;

        std::vector<std::vector<int>> orders;
        if (!verify_block_tree(g, bt, &orders)) continue;

        std::vector<int> parent(n + 1, 0);
        bool construction_ok = true;
        for (std::size_t bi = 0; bi < orders.size(); ++bi) {
            const std::vector<int>& order = orders[bi];
            if (order.empty()) {
                construction_ok = false;
                break;
            }
            for (std::size_t i = 1; i < order.size(); ++i) {
                parent[order[i]] = order[i - 1];
            }
            if (bt.parent[bi] < 0) continue;

            const std::vector<int>& parent_order = orders[bt.parent[bi]];
            int attachment = 0;
            for (std::size_t i = 0; i < parent_order.size(); ++i) {
                for (std::size_t j = 0; j < order.size(); ++j) {
                    if (g.has_edge(parent_order[i], order[j])) {
                        attachment = parent_order[i];
                        break;
                    }
                }
            }
            if (attachment == 0) {
                construction_ok = false;
                break;
            }
            parent[order[0]] = attachment;
        }

        if (construction_ok && is_indifference_tree_layout(g, parent)) {
            *tree_parent = parent;
            return true;
        }
    }
    return false;
}

/**
 * @brief Determines whether a connected graph is proper chordal
 */
inline bool is_connected_proper_chordal(const Graph& g) {
    int n = g.n;
    if (n <= 2) return true;
    std::vector<int> tree_parent;
    return find_connected_indifference_tree_layout(g, &tree_parent);
}

/**
 * @brief Constructs a deterministic indifference tree-layout componentwise
 */
inline bool find_indifference_tree_layout(
    const Graph& g,
    std::vector<int>* tree_parent) {
    const int n = g.n;
    tree_parent->assign(n + 1, 0);
    if (n == 0) return true;

    std::vector<bool> visited(n + 1, false);
    int global_root = 0;
    for (int start = 1; start <= n; ++start) {
        if (visited[start]) continue;

        std::vector<int> component;
        std::queue<int> q;
        q.push(start);
        visited[start] = true;
        while (!q.empty()) {
            const int u = q.front();
            q.pop();
            component.push_back(u);
            for (std::size_t i = 0; i < g.adj[u].size(); ++i) {
                const int v = g.adj[u][i];
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }

        std::vector<int> remap(n + 1, 0);
        for (std::size_t i = 0; i < component.size(); ++i) {
            remap[component[i]] = static_cast<int>(i) + 1;
        }
        std::vector<std::pair<int, int>> edges;
        for (std::size_t i = 0; i < component.size(); ++i) {
            const int u = component[i];
            for (std::size_t j = 0; j < g.adj[u].size(); ++j) {
                const int v = g.adj[u][j];
                if (remap[v] > remap[u]) {
                    edges.push_back(std::make_pair(remap[u], remap[v]));
                }
            }
        }

        const Graph subgraph(static_cast<int>(component.size()), edges);
        std::vector<int> subparent(component.size() + 1, 0);
        if (component.size() == 2) {
            subparent[2] = 1;
        } else if (component.size() > 2 &&
                   !find_connected_indifference_tree_layout(
                       subgraph, &subparent)) {
            tree_parent->clear();
            return false;
        }

        int component_root = 0;
        for (std::size_t i = 1; i < subparent.size(); ++i) {
            const int original = component[i - 1];
            if (subparent[i] == 0) {
                component_root = original;
            } else {
                (*tree_parent)[original] = component[subparent[i] - 1];
            }
        }
        if (component_root == 0) {
            tree_parent->clear();
            return false;
        }
        if (global_root == 0) {
            global_root = component_root;
        } else {
            (*tree_parent)[component_root] = global_root;
        }
    }

    if (!is_indifference_tree_layout(g, *tree_parent)) {
        tree_parent->clear();
        return false;
    }
    return true;
}

} // namespace detail_tree_layout

/**
 * @brief Computes the candidate block tree of a layout rooted at a vertex
 * @param g Input graph; connected (see below)
 * @param root Vertex to root the layout at, in [1, g.n]
 * @return LayoutBlockTree; success is false when Algorithm 1 finds no
 *         candidate block tree at that root
 *
 * This is Algorithm 1 of Paul & Protopapas on its own. Building a candidate
 * is necessary but not sufficient for an indifference tree-layout rooted at
 * @p root: Algorithm 2 can still reject it, so read success as "Algorithm 1
 * succeeded", not as "a layout exists". verify_layout_block_tree() runs the
 * nested-convex verification on the result, and check_proper_chordal() decides
 * class membership over all roots.
 *
 * Algorithm 1 grows a single tree out of @p root, so it expects a connected
 * graph: on a disconnected one it fails as soon as it reaches a component with
 * no processed neighbour, even when the graph does have a layout (the empty
 * 2-vertex graph is the smallest example). find_indifference_tree_layout()
 * is the entry point that handles components separately.
 *
 * A @p root outside [1, g.n] is rejected before any work: success is false
 * and block_of is the all -1 vector of size n+1. In particular 0, the
 * sentinel a layout's parent array uses for "no parent", is not a vertex.
 */
inline LayoutBlockTree compute_layout_block_tree(const Graph& g, int root) {
    if (root < 1 || root > g.n) {
        LayoutBlockTree bt;
        bt.block_of.assign(g.n + 1, -1);
        return bt;
    }
    return detail_tree_layout::compute_layout_block_tree(g, root);
}

/**
 * @brief Verifies a candidate block tree with the nested-convex condition
 * @param g Input graph, the one compute_layout_block_tree() was given
 * @param bt Candidate block tree from compute_layout_block_tree()
 * @return true iff Algorithm 2 accepts @p bt, i.e. an indifference
 *         tree-layout with that block tree exists
 *
 * Algorithm 2 of Paul & Protopapas. The verification enumerates the vertex
 * orderings of each block, so it is factorial in the largest block size
 * rather than the O(n^4) of the paper. An unsuccessful @p bt is false.
 */
inline bool verify_layout_block_tree(const Graph& g, const LayoutBlockTree& bt) {
    if (!bt.success) return false;
    return detail_tree_layout::verify_block_tree(g, bt);
}

/**
 * @brief Constructs a deterministic indifference tree-layout
 * @param g Input graph
 * @return TreeLayoutResult; success is false exactly when g has no such layout
 *
 * Disconnected graphs are handled componentwise, with the component roots
 * hung under the first component's root.
 */
inline TreeLayoutResult find_indifference_tree_layout(const Graph& g) {
    TreeLayoutResult res;
    std::vector<int> parent;
    if (!detail_tree_layout::find_indifference_tree_layout(g, &parent)) return res;
    res.parent.swap(parent);
    res.success = true;
    return res;
}

} // namespace graph_recognition

#endif
