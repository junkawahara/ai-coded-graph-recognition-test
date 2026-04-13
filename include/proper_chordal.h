#ifndef GRAPH_RECOGNITION_PROPER_CHORDAL_H
#define GRAPH_RECOGNITION_PROPER_CHORDAL_H

/**
 * @file proper_chordal.h
 * @brief Proper chordal グラフ (固有弦グラフ) 認識
 *
 * Proper chordal グラフは indifference tree-layout を許容する弦グラフ。
 * proper interval ⊂ proper chordal ⊂ chordal (interval とは非比較)。
 *
 * アルゴリズム (Paul & Protopapas, STACS 2024):
 *   1. 各頂点 x を根として block tree を計算 (Algorithm 1)
 *   2. 各ブロックの nested-convex 条件を検証 (Algorithm 2)
 *   いずれかの根で成功すれば proper chordal。
 *
 * 計算量: O(n^4) (論文の解析)。小さい n 向けにブルートフォース検証を使用。
 */

#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Proper chordal 認識の結果
 */
struct ProperChordalResult {
    bool is_proper_chordal = false;
};

namespace detail_proper_chordal {

/**
 * @brief 頂点集合 vertex_set のうち excluded を除いた部分の連結成分を返す
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
 * @brief Block tree の構造
 */
struct BlockTree {
    std::vector<std::vector<int>> blocks;  // blocks[i] = block i の頂点集合
    std::vector<int> parent;               // parent[i] = 親ブロック (-1 = root)
    std::vector<std::vector<int>> children; // children[i] = 子ブロック
    std::vector<int> depth;                // depth[i] = ブロックの深さ
    std::vector<int> block_of;             // block_of[v] = 頂点 v のブロック番号
    bool success;
};

/**
 * @brief Algorithm 1: Block tree computation
 *
 * 頂点 root を根とする indifference tree-layout の block tree を計算する。
 * 失敗した場合 success = false を返す。
 */
inline BlockTree compute_block_tree(const Graph& g, int root) {
    int n = g.n;
    BlockTree bt;
    bt.block_of.assign(n + 1, -1);
    bt.success = false;

    // S: 処理済み頂点集合
    std::vector<bool> in_S(n + 1, false);
    in_S[root] = true;
    int s_count = 1;

    // 最初のブロック: {root}
    bt.blocks.push_back(std::vector<int>(1, root));
    bt.parent.push_back(-1);
    bt.children.push_back(std::vector<int>());
    bt.depth.push_back(0);
    bt.block_of[root] = 0;

    while (s_count < n) {
        // G - S の連結成分を探す
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

            // NS_C: comp 内で S に隣接する頂点
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

            // S-maximal 頂点: N(v) ∩ S が包含で最大
            // まず最大の N(v) ∩ S を見つける
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
                // best_ns に nv_s が含まれるかチェック
                if (nv_s_size > best_ns_size) {
                    // nv_s が best_ns のスーパーセットか確認
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

            // S-maximal 頂点を収集
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

            // NS_C-universal: ns_c の全頂点に隣接 (自分自身を除く)
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

            // S-block X = block を発見
            // S を更新
            for (size_t i = 0; i < block.size(); ++i) in_S[block[i]] = true;
            s_count += (int)block.size();

            // N(X) を計算
            std::vector<bool> nx(n + 1, false);
            for (size_t i = 0; i < block.size(); ++i) {
                for (size_t j = 0; j < g.adj[block[i]].size(); ++j) {
                    nx[g.adj[block[i]][j]] = true;
                }
            }

            // N(X) ∩ B ≠ ∅ かつ最深のブロック B を探す
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

            if (best_parent < 0) return bt; // should not happen

            int new_idx = (int)bt.blocks.size();
            bt.blocks.push_back(block);
            bt.parent.push_back(best_parent);
            bt.children.push_back(std::vector<int>());
            bt.depth.push_back(best_depth + 1);
            bt.children[best_parent].push_back(new_idx);
            for (size_t i = 0; i < block.size(); ++i) bt.block_of[block[i]] = new_idx;

            found_any = true;
            break; // 1 つのブロックを処理したら再度ループ
        }

        if (!found_any) return bt; // 失敗
    }

    bt.success = true;
    return bt;
}

/**
 * @brief 集合がネスト (pairwise comparable by inclusion) か判定
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
 * @brief 集合 s の要素が順列 perm 内で連続しているか判定
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
 * @brief Algorithm 2 の nested-convex 条件をブルートフォースで検証
 *
 * ブロック B の頂点の順列のうち、全ての N(y)∩B が連続で、
 * 同一成分内のネストした集合 Y ⊃ Z について Y\Z が Z より先に来るものが存在するか。
 */
inline bool check_nested_convex_brute(
    const std::vector<int>& block_vertices,
    const std::vector<std::vector<std::vector<bool>>>& component_sets,
    int n) {
    int bsize = (int)block_vertices.size();
    if (bsize <= 1) return true;

    // 全集合を収集
    std::vector<std::vector<bool>> all_sets;
    std::vector<int> set_comp; // どの成分に属するか
    for (size_t ci = 0; ci < component_sets.size(); ++ci) {
        for (size_t si = 0; si < component_sets[ci].size(); ++si) {
            all_sets.push_back(component_sets[ci][si]);
            set_comp.push_back((int)ci);
        }
    }

    // block_vertices の全順列を試す
    std::vector<int> perm = block_vertices;
    std::sort(perm.begin(), perm.end());

    do {
        // 全集合が連続か
        bool all_consecutive = true;
        for (size_t i = 0; i < all_sets.size() && all_consecutive; ++i) {
            if (!is_consecutive_in_perm(all_sets[i], perm)) {
                all_consecutive = false;
            }
        }
        if (!all_consecutive) continue;

        // C-nested 条件: 同一成分内のネスト Y ⊃ Z について Y\Z が Z より前
        bool c_nested = true;
        for (size_t ci = 0; ci < component_sets.size() && c_nested; ++ci) {
            const std::vector<std::vector<bool>>& ni = component_sets[ci];
            for (size_t a = 0; a < ni.size() && c_nested; ++a) {
                for (size_t b = 0; b < ni.size() && c_nested; ++b) {
                    if (a == b) continue;
                    // ni[a] ⊂ ni[b] (a は b の真部分集合) か?
                    bool a_sub_b = true, b_sub_a = true;
                    for (int v = 1; v <= n; ++v) {
                        if (ni[a][v] && !ni[b][v]) a_sub_b = false;
                        if (ni[b][v] && !ni[a][v]) b_sub_a = false;
                    }
                    if (!a_sub_b || b_sub_a) continue;
                    // ni[a] ⊂ ni[b] (strict): Z=ni[a], Y=ni[b]
                    // Y\Z の全要素が Z の全要素より前に来る必要がある
                    int last_diff = -1; // Y\Z の最後の位置
                    int first_z = (int)perm.size(); // Z の最初の位置
                    for (int pi = 0; pi < (int)perm.size(); ++pi) {
                        int v = perm[pi];
                        if (ni[b][v] && !ni[a][v]) {
                            // Y\Z の要素
                            if (pi > last_diff) last_diff = pi;
                        }
                        if (ni[a][v]) {
                            // Z の要素
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
 * @brief Algorithm 2: Block tree の検証
 *
 * 各ブロック B について:
 * 1. C_B (B の下の頂点の「コーン」) を計算
 * 2. G[C_B] - B の連結成分 C_1, ..., C_k を取得
 * 3. 各 C_i について N_i = {N(y) ∩ B | y ∈ C_i} を計算
 * 4. 各 N_i がネストか確認
 * 5. Nested-Convex 条件を検証
 */
inline bool verify_block_tree(const Graph& g, const BlockTree& bt) {
    int n = g.n;

    for (size_t bi = 0; bi < bt.blocks.size(); ++bi) {
        const std::vector<int>& B = bt.blocks[bi];

        // A_B: 祖先ブロックの全頂点
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

        // C_B: G - A_B で B を含む連結成分
        std::vector<bool> not_ancestor(n + 1, false);
        for (int v = 1; v <= n; ++v) {
            if (!in_ancestors[v]) not_ancestor[v] = true;
        }
        std::vector<bool> empty_excl(n + 1, false);
        std::vector<std::vector<int>> cone_comps =
            find_components_in_subset(g, not_ancestor, empty_excl);

        // B の最初の頂点を含む成分を見つける
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

        // G[C_B] - B の連結成分
        std::vector<bool> in_B(n + 1, false);
        for (size_t i = 0; i < B.size(); ++i) in_B[B[i]] = true;

        std::vector<std::vector<int>> sub_comps =
            find_components_in_subset(g, in_cone, in_B);

        if (sub_comps.empty()) continue; // 子がない — 条件は自動的に成立

        // 各成分 C_i について N_i を計算
        std::vector<std::vector<std::vector<bool>>> component_sets;
        for (size_t ci = 0; ci < sub_comps.size(); ++ci) {
            std::vector<std::vector<bool>> ni_sets;
            // 重複排除
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
                if (!non_empty) continue; // 空集合は無視
                // 重複チェック
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
            // N_i がネストか
            if (!is_nested_collection(ni_sets, n)) return false;
            component_sets.push_back(ni_sets);
        }

        // Nested-Convex 条件
        if (!check_nested_convex_brute(B, component_sets, n)) return false;
    }

    return true;
}

/**
 * @brief 連結グラフが proper chordal か判定
 */
inline bool is_connected_proper_chordal(const Graph& g) {
    int n = g.n;
    if (n <= 2) return true;

    // 全頂点を根として試す
    for (int x = 1; x <= n; ++x) {
        BlockTree bt = compute_block_tree(g, x);
        if (!bt.success) continue;
        if (verify_block_tree(g, bt)) return true;
    }
    return false;
}

} // namespace detail_proper_chordal

/**
 * @brief グラフが proper chordal か判定する
 * @param g 入力グラフ
 * @return ProperChordalResult
 *
 * proper chordal = chordal ∩ indifference tree-layout を許容。
 * proper interval ⊂ proper chordal ⊂ chordal。
 * 遺伝的クラス (誘導部分グラフに閉)。
 */
inline ProperChordalResult check_proper_chordal(const Graph& g) {
    ProperChordalResult res;

    int n = g.n;
    if (n <= 2) { res.is_proper_chordal = true; return res; }

    // chordal チェック
    ChordalResult cr = check_chordal(g);
    if (!cr.is_chordal) return res;

    // 連結成分ごとに判定
    std::vector<bool> visited(n + 1, false);
    for (int v = 1; v <= n; ++v) {
        if (visited[v]) continue;
        // BFS で連結成分を取得
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

        if (comp.size() <= 2) continue; // 0, 1, 2 頂点の成分は自明に proper chordal

        // 連結成分の誘導部分グラフを構築
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
