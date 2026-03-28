#ifndef GRAPH_RECOGNITION_LEXBFS_H
#define GRAPH_RECOGNITION_LEXBFS_H

/**
 * @file lexbfs.h
 * @brief Lexicographic Breadth-First Search (LexBFS)
 *
 * Rose, Tarjan, Lueker (1976) のアルゴリズム。
 * 弦グラフに対して完全除去順序 (PEO) を生成する。
 *
 * MCS と同様に、弦グラフの場合にのみ正しい PEO を返す。
 * 非弦グラフでは PEO 検証に失敗する順序を返す。
 *
 * アルゴリズム:
 *   - SIMPLE_LEXBFS: ラベルリスト比較による単純実装 O(n^2 + nm)
 *   - PARTITION_LEXBFS: 分割細分化による高速実装 O(n+m) (デフォルト)
 */

#include "graph.h"
#include "mcs.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief LexBFS アルゴリズムの選択
 */
enum class LexBFSAlgorithm {
    SIMPLE_LEXBFS,    /**< ラベルリスト比較 O(n^2 + nm) */
    PARTITION_LEXBFS  /**< 分割細分化 O(n+m) (デフォルト) */
};

namespace detail {

/**
 * @brief 単純な LexBFS 実装
 *
 * 各頂点にラベル (整数リスト) を持ち、辞書式最大のラベルを持つ
 * 未番号頂点を選択する。ラベル比較に O(n) かかるため全体 O(n^2 + nm)。
 *
 * @param g 入力グラフ
 * @return MCSResult (order と number)
 */
inline MCSResult lexbfs_simple(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    // label[v]: 頂点 v のラベル (辞書式比較用)
    std::vector<std::vector<int> > label(n + 1);
    std::vector<unsigned char> used(n + 1, 0);

    for (int i = n; i >= 1; --i) {
        // 未番号頂点のうち辞書式最大のラベルを持つものを選択
        int best = -1;
        for (int v = 1; v <= n; ++v) {
            if (used[v]) continue;
            if (best == -1 || label[v] > label[best]) {
                best = v;
            }
        }

        used[best] = 1;
        res.order[i] = best;
        res.number[best] = i;

        // 未番号隣接頂点のラベルに i を追加
        for (size_t j = 0; j < g.adj[best].size(); ++j) {
            int u = g.adj[best][j];
            if (!used[u]) {
                label[u].push_back(i);
            }
        }
    }

    return res;
}

/**
 * @brief 分割細分化による LexBFS O(n+m)
 *
 * Habib, McConnell, Paul, Viennot (2000) の手法に基づく。
 * 頂点クラスの順序リストを管理し、各ステップで隣接頂点を
 * 所属クラスの前方に分離する。
 *
 * @param g 入力グラフ
 * @return MCSResult (order と number)
 */
inline MCSResult lexbfs_partition(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    int m = 0;
    for (int v = 1; v <= n; ++v) m += (int)g.adj[v].size();
    m /= 2;

    // クラスの双方向リスト
    // sentinel = 0, 初期クラス = 1
    int max_classes = n + 2 * m + 10;
    std::vector<int> class_next(max_classes, 0);
    std::vector<int> class_prev(max_classes, 0);
    std::vector<int> class_head(max_classes, 0);

    // 頂点のクラス内双方向リスト
    std::vector<int> vertex_next(n + 1, 0);
    std::vector<int> vertex_prev(n + 1, 0);
    std::vector<int> vertex_class(n + 1, 0);
    std::vector<unsigned char> used(n + 1, 0);

    int next_class_id = 2;

    // 初期化: 全頂点をクラス 1 に配置
    class_next[0] = 1;
    class_prev[1] = 0;
    class_next[1] = 0;
    class_prev[0] = 1;

    class_head[1] = 1;
    for (int v = 1; v <= n; ++v) {
        vertex_class[v] = 1;
        vertex_next[v] = (v < n) ? v + 1 : 0;
        vertex_prev[v] = (v > 1) ? v - 1 : 0;
    }

    // new_class[c]: ステップ内でクラス c を分割した新クラスの ID (0 = 未分割)
    std::vector<int> new_class(max_classes, 0);
    std::vector<int> touched_classes;

    for (int i = n; i >= 1; --i) {
        // 先頭の非空クラスから頂点を取り出す
        int first_class = class_next[0];
        while (first_class != 0 && class_head[first_class] == 0) {
            int nc = class_next[first_class];
            class_next[0] = nc;
            if (nc != 0) class_prev[nc] = 0;
            first_class = nc;
        }

        int v = class_head[first_class];

        // v をクラスから除去
        class_head[first_class] = vertex_next[v];
        if (vertex_next[v] != 0) vertex_prev[vertex_next[v]] = 0;

        // クラスが空になった場合、クラスリストから除去
        if (class_head[first_class] == 0) {
            int nc = class_next[first_class];
            int pc = class_prev[first_class];
            class_next[pc] = nc;
            if (nc != 0) class_prev[nc] = pc;
        }

        used[v] = 1;
        res.order[i] = v;
        res.number[v] = i;

        // 分割細分化: v の未番号隣接頂点を所属クラスの前方に分離
        touched_classes.clear();

        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (used[u]) continue;

            int c = vertex_class[u];

            // クラス c がまだ分割されていなければ、新クラスを c の前に作成
            if (new_class[c] == 0) {
                int nc = next_class_id++;

                // c の前に nc を挿入
                int pc = class_prev[c];
                class_next[pc] = nc;
                class_prev[nc] = pc;
                class_next[nc] = c;
                class_prev[c] = nc;

                class_head[nc] = 0;
                new_class[c] = nc;
                touched_classes.push_back(c);
            }

            int nc = new_class[c];

            // u をクラス c の頂点リストから除去
            if (vertex_prev[u] == 0) {
                class_head[c] = vertex_next[u];
                if (vertex_next[u] != 0) vertex_prev[vertex_next[u]] = 0;
            } else {
                vertex_next[vertex_prev[u]] = vertex_next[u];
                if (vertex_next[u] != 0) vertex_prev[vertex_next[u]] = vertex_prev[u];
            }

            // u を nc の先頭に挿入
            vertex_next[u] = class_head[nc];
            vertex_prev[u] = 0;
            if (class_head[nc] != 0) vertex_prev[class_head[nc]] = u;
            class_head[nc] = u;
            vertex_class[u] = nc;
        }

        // 空になった元クラスを除去し、new_class をリセット
        for (size_t j = 0; j < touched_classes.size(); ++j) {
            int c = touched_classes[j];
            if (class_head[c] == 0) {
                int nc = class_next[c];
                int pc = class_prev[c];
                class_next[pc] = nc;
                if (nc != 0) class_prev[nc] = pc;
            }
            new_class[c] = 0;
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief LexBFS 順序を計算する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: PARTITION_LEXBFS)
 * @return MCSResult (order と number)
 *
 * 弦グラフに対しては、結果の order[1..n] が完全除去順序 (PEO) となる。
 * MCS と同じインターフェースで結果を返す。
 */
inline MCSResult lexbfs(const Graph& g,
    LexBFSAlgorithm algo = LexBFSAlgorithm::PARTITION_LEXBFS) {
    switch (algo) {
        case LexBFSAlgorithm::SIMPLE_LEXBFS:
            return detail::lexbfs_simple(g);
        case LexBFSAlgorithm::PARTITION_LEXBFS:
            return detail::lexbfs_partition(g);
        default:
            break;
    }
    return MCSResult();
}

} // namespace graph_recognition

#endif
