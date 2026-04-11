#ifndef GRAPH_RECOGNITION_HALIN_ENUM_H
#define GRAPH_RECOGNITION_HALIN_ENUM_H

/**
 * @file halin_enum.h
 * @brief 非同型 Halin グラフの列挙
 *
 * 構成的列挙により頂点数 n の全非同型 Halin グラフを列挙する。
 *
 * Halin グラフは次の手順で構成される:
 *   1. 次数 2 の頂点を持たない木 T (HI-tree) を取る
 *   2. T を平面に埋め込む
 *   3. T の葉を埋め込み順でサイクルで接続する
 *
 * アルゴリズム:
 *   tree_enum.h で非同型木を生成し、HI-tree をフィルタ。
 *   各 HI-tree の全平面埋め込み (各頂点の隣接巡回順序) を列挙し、
 *   DFS で葉順序を決定して Halin グラフを構築。
 *   平面木のブラケットコード (全根・全回転・両反転の最小値) で
 *   同型重複を除去する。
 *
 * 非同型数: OEIS A346779
 *   0, 0, 0, 1, 1, 2, 2, 4, 6, 13, 22, 50, 106, 252, ...
 *
 * 参考文献:
 *   Halin, "Studies on minimally n-connected graphs,"
 *   Combinatorial Mathematics and its Applications, 1971
 */

#include "tree_enum.h"

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 列挙された Halin グラフ
 */
struct HalinEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > edges;      /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief Halin グラフ列挙の結果
 */
struct HalinEnumerationResult {
    std::vector<HalinEnumeratedGraph> graphs;
};

namespace detail {

/**
 * @brief HI-tree 判定 (次数 2 の頂点がないか)
 */
inline bool is_hi_tree(int n,
                       const std::vector<std::pair<int, int> >& edges) {
    if (n < 4) return false;
    std::vector<int> deg(n + 1, 0);
    for (std::size_t i = 0; i < edges.size(); ++i) {
        ++deg[edges[i].first];
        ++deg[edges[i].second];
    }
    int leaf_count = 0;
    for (int v = 1; v <= n; ++v) {
        if (deg[v] == 2) return false;
        if (deg[v] == 1) ++leaf_count;
    }
    return leaf_count >= 3;
}

/**
 * @brief 隣接リストを構築
 */
inline std::vector<std::vector<int> > build_adj(
    int n, const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > adj(n + 1);
    for (std::size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first].push_back(edges[i].second);
        adj[edges[i].second].push_back(edges[i].first);
    }
    for (int v = 1; v <= n; ++v) {
        std::sort(adj[v].begin(), adj[v].end());
    }
    return adj;
}

/**
 * @brief 平面木のブラケットコードを再帰的に計算
 *
 * @param v 現在の頂点
 * @param parent 親頂点 (0 なら根)
 * @param reflected 反転フラグ (true なら巡回順序を逆転)
 * @param tree_adj 木の隣接リスト
 * @param embedding 各頂点の隣接巡回順序
 * @return ブラケットコード文字列
 */
inline std::string compute_plane_tree_code(
    int v, int parent, bool reflected,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::vector<int> >& embedding) {

    // 葉
    if ((int)tree_adj[v].size() == 1 && parent != 0) {
        return "L";
    }

    const std::vector<int>& cyc = embedding[v];
    int k = (int)cyc.size();

    // 子の順序を決定
    std::vector<int> children;
    if (parent == 0) {
        // 根: 巡回順序をそのまま (または反転)
        if (!reflected) {
            for (int i = 0; i < k; ++i) {
                children.push_back(cyc[i]);
            }
        } else {
            children.push_back(cyc[0]);
            for (int i = k - 1; i >= 1; --i) {
                children.push_back(cyc[i]);
            }
        }
    } else {
        // 非根: parent の次の位置から巡回
        int parent_pos = -1;
        for (int i = 0; i < k; ++i) {
            if (cyc[i] == parent) { parent_pos = i; break; }
        }
        if (!reflected) {
            for (int i = 1; i < k; ++i) {
                children.push_back(cyc[(parent_pos + i) % k]);
            }
        } else {
            for (int i = 1; i < k; ++i) {
                children.push_back(cyc[((parent_pos - i) % k + k) % k]);
            }
        }
    }

    std::string code = "(";
    for (int i = 0; i < (int)children.size(); ++i) {
        code += compute_plane_tree_code(
            children[i], v, reflected, tree_adj, embedding);
    }
    code += ")";
    return code;
}

/**
 * @brief 平面木のカノニカルコードを計算
 *
 * 全根・全回転・両反転における最小ブラケットコードを返す。
 * 同一の Halin グラフを与える平面木は同一のカノニカルコードを持つ。
 */
inline std::string canonical_plane_tree_code(
    int n,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::vector<int> >& embedding) {

    std::string min_code;
    bool first = true;

    for (int root = 1; root <= n; ++root) {
        int deg = (int)tree_adj[root].size();
        if (deg == 0) continue;

        // 根の巡回順序の各回転を試行
        for (int start = 0; start < deg; ++start) {
            // 回転された埋め込みを作成 (根のみ回転)
            std::vector<std::vector<int> > emb_rot = embedding;
            emb_rot[root].clear();
            for (int i = 0; i < deg; ++i) {
                emb_rot[root].push_back(
                    embedding[root][(start + i) % deg]);
            }

            // 正方向
            std::string code = compute_plane_tree_code(
                root, 0, false, tree_adj, emb_rot);
            if (first || code < min_code) {
                min_code = code;
                first = false;
            }

            // 反転方向
            code = compute_plane_tree_code(
                root, 0, true, tree_adj, emb_rot);
            if (code < min_code) {
                min_code = code;
            }
        }
    }

    return min_code;
}

/**
 * @brief 根付き木の DFS で葉を埋め込み順に収集
 */
inline std::vector<int> collect_leaf_order(
    int root,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::vector<int> >& embedding) {

    std::vector<int> leaves;
    // DFS スタック: (vertex, parent)
    std::vector<std::pair<int, int> > stack;
    stack.push_back(std::make_pair(root, 0));

    while (!stack.empty()) {
        int v = stack.back().first;
        int parent = stack.back().second;
        stack.pop_back();

        if ((int)tree_adj[v].size() == 1 && parent != 0) {
            leaves.push_back(v);
            continue;
        }

        const std::vector<int>& cyc = embedding[v];
        int k = (int)cyc.size();

        int parent_pos = -1;
        if (parent != 0) {
            for (int i = 0; i < k; ++i) {
                if (cyc[i] == parent) { parent_pos = i; break; }
            }
        }

        std::vector<int> children;
        if (parent_pos == -1) {
            for (int i = 0; i < k; ++i) {
                children.push_back(cyc[i]);
            }
        } else {
            for (int i = 1; i < k; ++i) {
                children.push_back(cyc[(parent_pos + i) % k]);
            }
        }

        for (int i = (int)children.size() - 1; i >= 0; --i) {
            stack.push_back(std::make_pair(children[i], v));
        }
    }

    return leaves;
}

/**
 * @brief 平面埋め込みの列挙を再帰的に行い、Halin グラフを構築
 */
inline void enumerate_embeddings_dfs(
    int vertex, int n,
    const std::vector<std::vector<int> >& tree_adj,
    const std::vector<std::pair<int, int> >& tree_edges,
    std::vector<std::vector<int> >& embedding,
    int root,
    std::set<std::string>& seen,
    std::vector<HalinEnumeratedGraph>& results) {

    if (vertex > n) {
        // 全頂点の埋め込みが確定 → カノニカルコードで重複チェック
        std::string code = canonical_plane_tree_code(n, tree_adj, embedding);

        if (seen.find(code) != seen.end()) return;
        seen.insert(code);

        // Halin グラフを構築
        std::vector<int> leaves = collect_leaf_order(root, tree_adj, embedding);
        if ((int)leaves.size() < 3) return;

        HalinEnumeratedGraph graph;
        graph.n = n;

        // 木の辺
        for (std::size_t i = 0; i < tree_edges.size(); ++i) {
            graph.edges.push_back(tree_edges[i]);
        }

        // 葉サイクルの辺
        int L = (int)leaves.size();
        for (int i = 0; i < L; ++i) {
            int u = leaves[i];
            int v = leaves[(i + 1) % L];
            if (u > v) { int tmp = u; u = v; v = tmp; }
            graph.edges.push_back(std::make_pair(u, v));
        }
        std::sort(graph.edges.begin(), graph.edges.end());

        results.push_back(graph);
        return;
    }

    int d = (int)tree_adj[vertex].size();
    if (d <= 1) {
        embedding[vertex] = tree_adj[vertex];
        enumerate_embeddings_dfs(vertex + 1, n, tree_adj, tree_edges,
                                 embedding, root, seen, results);
        return;
    }

    // 次数 d >= 2: 最初の隣接頂点を固定し、残りの順列を列挙
    std::vector<int> perm = tree_adj[vertex];
    std::sort(perm.begin(), perm.end());
    std::vector<int> rest(perm.begin() + 1, perm.end());
    std::sort(rest.begin(), rest.end());

    do {
        embedding[vertex].clear();
        embedding[vertex].push_back(perm[0]);
        for (std::size_t i = 0; i < rest.size(); ++i) {
            embedding[vertex].push_back(rest[i]);
        }
        enumerate_embeddings_dfs(vertex + 1, n, tree_adj, tree_edges,
                                 embedding, root, seen, results);
    } while (std::next_permutation(rest.begin(), rest.end()));
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型 Halin グラフを列挙する
 * @param n 頂点数
 * @return HalinEnumerationResult
 *
 * tree_enum.h で非同型木を生成し、HI-tree をフィルタ。
 * 各 HI-tree の全平面埋め込みを列挙して Halin グラフを構築し、
 * 平面木のカノニカルコードで重複を除去する。
 */
inline HalinEnumerationResult enumerate_halin_graphs(int n) {
    HalinEnumerationResult result;
    if (n < 4) return result;

    TreeEnumerationResult trees = enumerate_tree_graphs(n);
    std::set<std::string> seen;

    for (std::size_t t = 0; t < trees.graphs.size(); ++t) {
        const TreeEnumeratedGraph& tree = trees.graphs[t];

        if (!detail::is_hi_tree(n, tree.edges)) continue;

        std::vector<std::vector<int> > tree_adj =
            detail::build_adj(n, tree.edges);

        int root = -1;
        for (int v = 1; v <= n; ++v) {
            if ((int)tree_adj[v].size() >= 3) {
                root = v;
                break;
            }
        }
        if (root == -1) continue;

        std::vector<std::vector<int> > embedding(n + 1);
        detail::enumerate_embeddings_dfs(
            1, n, tree_adj, tree.edges, embedding,
            root, seen, result.graphs);
    }

    return result;
}

}  // namespace graph_recognition

#endif
