#ifndef GRAPH_RECOGNITION_PROPER_CHORDAL_H
#define GRAPH_RECOGNITION_PROPER_CHORDAL_H

/**
 * @file proper_chordal.h
 * @brief Proper chordal graph recognition
 *
 * Proper chordal graphs are chordal graphs that admit an indifference tree-layout.
 * proper interval is a subset of proper chordal which is a subset of chordal (incomparable with interval).
 *
 * Algorithm (Paul & Protopapas, STACS 2024):
 *   1. Compute block tree with each vertex x as root (Algorithm 1)
 *   2. Verify nested-convex condition for each block (Algorithm 2)
 *   Proper chordal if successful for any root.
 *
 * Complexity: O(n^4) (as analyzed in the paper). Uses brute-force verification for small n.
 */

#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Result of proper chordal recognition
 */
struct ProperChordalResult {
    bool is_proper_chordal = false;
};

namespace detail_proper_chordal {

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
 * @brief Block tree structure
 */
struct BlockTree {
    std::vector<std::vector<int>> blocks;  // blocks[i] = vertex set of block i
    std::vector<int> parent;               // parent[i] = parent block (-1 = root)
    std::vector<std::vector<int>> children; // children[i] = child blocks
    std::vector<int> depth;                // depth[i] = depth of block
    std::vector<int> block_of;             // block_of[v] = block number of vertex v
    bool success;
};

/**
 * @brief Algorithm 1: Block tree computation
 *
 * Computes the block tree of the indifference tree-layout rooted at vertex root.
 * Returns success = false on failure.
 */
inline BlockTree compute_block_tree(const Graph& g, int root) {
    int n = g.n;
    BlockTree bt;
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
    int n) {
    int bsize = (int)block_vertices.size();
    if (bsize <= 1) return true;

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
        if (c_nested) return true;
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
inline bool verify_block_tree(const Graph& g, const BlockTree& bt) {
    int n = g.n;

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

        if (sub_comps.empty()) continue; // No children -- condition is automatically satisfied

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
        if (!check_nested_convex_brute(B, component_sets, n)) return false;
    }

    return true;
}

/**
 * @brief Determines whether a connected graph is proper chordal
 */
inline bool is_connected_proper_chordal(const Graph& g) {
    int n = g.n;
    if (n <= 2) return true;

    // Try all vertices as root
    for (int x = 1; x <= n; ++x) {
        BlockTree bt = compute_block_tree(g, x);
        if (!bt.success) continue;
        if (verify_block_tree(g, bt)) return true;
    }
    return false;
}

} // namespace detail_proper_chordal

/**
 * @brief Determines whether the graph is proper chordal
 * @param g Input graph
 * @return ProperChordalResult
 *
 * proper chordal = chordal and admitting an indifference tree-layout.
 * proper interval is a subset of proper chordal which is a subset of chordal.
 * Hereditary class (closed under induced subgraphs).
 */
inline ProperChordalResult check_proper_chordal(const Graph& g) {
    ProperChordalResult res;

    int n = g.n;
    if (n <= 2) { res.is_proper_chordal = true; return res; }

    // Chordal check
    ChordalResult cr = check_chordal(g);
    if (!cr.is_chordal) return res;

    // Determine per connected component
    std::vector<bool> visited(n + 1, false);
    for (int v = 1; v <= n; ++v) {
        if (visited[v]) continue;
        // Get connected component via BFS
        std::vector<int> comp;
        std::queue<int> q;
        q.push(v); visited[v] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            comp.push_back(u);
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (!visited[w]) { visited[w] = true; q.push(w); }
            }
        }

        if (comp.size() <= 2) continue; // Components of 0, 1, 2 vertices are trivially proper chordal

        // Build induced subgraph of connected component
        std::vector<int> remap(n + 1, 0);
        for (size_t i = 0; i < comp.size(); ++i) remap[comp[i]] = (int)i + 1;

        std::vector<std::pair<int, int>> edges;
        for (size_t i = 0; i < comp.size(); ++i) {
            int u = comp[i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (remap[w] > remap[u]) {
                    edges.push_back(std::make_pair(remap[u], remap[w]));
                }
            }
        }
        Graph sub((int)comp.size(), edges);

        if (!detail_proper_chordal::is_connected_proper_chordal(sub)) return res;
    }

    res.is_proper_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
