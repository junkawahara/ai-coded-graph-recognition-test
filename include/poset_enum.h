#ifndef GRAPH_RECOGNITION_POSET_ENUM_H
#define GRAPH_RECOGNITION_POSET_ENUM_H

/**
 * @file poset_enum.h
 * @brief Poset (半順序集合) のラベル付き全列挙
 *
 * 頂点集合 {1, ..., n} 上の全ラベル付き半順序 (poset) を構成的に全列挙する。
 * n(n-1)/2 個の非順序ペア {i, j} それぞれについて 3 通り
 * (i < j / j < i / 非comparable) を DFS で選択し、推移閉包を増分管理して
 * サイクル検出による枝刈りを行う。出力は各 poset の Hasse 図 (被覆関係)。
 *
 * 参考文献:
 *   - Brinkmann, McKay, "Posets on up to 16 Points," Order 19(2), 2002
 * OEIS:
 *   - A001035 (ラベル付き半順序数): 1, 1, 3, 19, 219, 4231, ...
 *   - A000112 (非ラベル付き半順序数)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 列挙された半順序 (Hasse 図表現)
 */
struct PosetEnumeratedGraph {
    int n;                                        /**< 要素数 */
    std::vector<std::pair<int, int> > arcs;       /**< Hasse 図アーク (u, v) = u < v (被覆関係), ソート済み */
};

/**
 * @brief Poset 列挙の結果
 */
struct PosetEnumerationResult {
    std::vector<PosetEnumeratedGraph> graphs;     /**< 列挙された半順序の配列 */
};

namespace detail_poset_enum {

/** @brief 構成的列挙の内部状態 */
struct PosetEnumState {
    int n;                                         /**< 要素数 */
    int num_pairs;                                 /**< C(n,2): 非順序ペア数 */
    std::vector<std::pair<int, int> > pairs;       /**< 全非順序ペア (i<j, 辞書順) */
    std::vector<std::vector<char> > rel;           /**< rel[i][j]=1 iff i <_P j (推移閉包) */
    std::vector<std::vector<char> > incomp;        /**< incomp[i][j]=1 iff i‖j が確定済み */
};

/**
 * @brief 状態の初期化: 全非順序ペアを構築
 */
inline PosetEnumState poset_build_state(int n) {
    PosetEnumState state;
    state.n = n;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            state.pairs.push_back(std::make_pair(i, j));
        }
    }
    state.num_pairs = static_cast<int>(state.pairs.size());
    state.rel.assign(static_cast<size_t>(n + 1),
                     std::vector<char>(static_cast<size_t>(n + 1), 0));
    state.incomp.assign(static_cast<size_t>(n + 1),
                        std::vector<char>(static_cast<size_t>(n + 1), 0));
    return state;
}

/**
 * @brief Hasse 図 (被覆関係) を rel 行列から抽出する
 *
 * arc (u, v) は u < v かつ u < k < v なる k が存在しないとき被覆関係。
 */
inline PosetEnumeratedGraph poset_extract_hasse(const PosetEnumState& state) {
    PosetEnumeratedGraph g;
    g.n = state.n;
    for (int u = 1; u <= state.n; ++u) {
        for (int v = 1; v <= state.n; ++v) {
            if (u == v) continue;
            if (!state.rel[static_cast<size_t>(u)][static_cast<size_t>(v)]) continue;
            bool is_cover = true;
            for (int k = 1; k <= state.n; ++k) {
                if (k == u || k == v) continue;
                if (state.rel[static_cast<size_t>(u)][static_cast<size_t>(k)] &&
                    state.rel[static_cast<size_t>(k)][static_cast<size_t>(v)]) {
                    is_cover = false;
                    break;
                }
            }
            if (is_cover) {
                g.arcs.push_back(std::make_pair(u, v));
            }
        }
    }
    std::sort(g.arcs.begin(), g.arcs.end());
    return g;
}

inline void poset_enum_dfs(PosetEnumState& state, int pair_idx,
                           std::vector<PosetEnumeratedGraph>* out);

/**
 * @brief 関係 lo < hi を追加し、推移閉包を更新して再帰する
 *
 * predecessors(lo) × successors(hi) の全ペアを設定する。
 * サイクル検出時は枝刈り。変更セルを記録してバックトラック時に復元する。
 */
inline void poset_try_add_relation(PosetEnumState& state, int lo, int hi,
                                   int pair_idx,
                                   std::vector<PosetEnumeratedGraph>* out) {
    // サイクル検出: hi < lo が既に成立
    if (state.rel[static_cast<size_t>(hi)][static_cast<size_t>(lo)]) return;

    // predecessors of lo (including lo): {a : rel[a][lo] || a == lo}
    // successors of hi (including hi):  {b : rel[hi][b] || b == hi}
    std::vector<int> preds, succs;
    preds.push_back(lo);
    for (int a = 1; a <= state.n; ++a) {
        if (a != lo && state.rel[static_cast<size_t>(a)][static_cast<size_t>(lo)]) {
            preds.push_back(a);
        }
    }
    succs.push_back(hi);
    for (int b = 1; b <= state.n; ++b) {
        if (b != hi && state.rel[static_cast<size_t>(hi)][static_cast<size_t>(b)]) {
            succs.push_back(b);
        }
    }

    // サイクル検出: preds ∩ succs ≠ ∅ ならサイクル
    for (size_t si = 0; si < succs.size(); ++si) {
        int b = succs[si];
        for (size_t pi = 0; pi < preds.size(); ++pi) {
            if (preds[pi] == b) return;
        }
    }

    // 変更セルを記録して推移閉包を設定
    std::vector<std::pair<int, int> > changed;
    bool violates_incomp = false;
    for (size_t pi = 0; pi < preds.size() && !violates_incomp; ++pi) {
        int a = preds[pi];
        for (size_t si = 0; si < succs.size() && !violates_incomp; ++si) {
            int b = succs[si];
            if (!state.rel[static_cast<size_t>(a)][static_cast<size_t>(b)]) {
                // incomparable 制約違反チェック
                if (state.incomp[static_cast<size_t>(a)][static_cast<size_t>(b)]) {
                    violates_incomp = true;
                    break;
                }
                state.rel[static_cast<size_t>(a)][static_cast<size_t>(b)] = 1;
                changed.push_back(std::make_pair(a, b));
            }
        }
    }

    if (!violates_incomp) {
        poset_enum_dfs(state, pair_idx + 1, out);
    }

    // 復元
    for (size_t ci = 0; ci < changed.size(); ++ci) {
        state.rel[static_cast<size_t>(changed[ci].first)]
                 [static_cast<size_t>(changed[ci].second)] = 0;
    }
}

/**
 * @brief 構成的列挙の DFS
 *
 * pair_idx 番目の非順序ペア (i, j) について:
 * - 推移閉包で既決定なら分岐なしで再帰
 * - 未決定なら 3 分岐で探索:
 *   分岐 0: 非comparable (i ‖ j)
 *   分岐 1: i < j
 *   分岐 2: j < i
 * 全ペアの選択が決定したら Hasse 図を抽出して出力する。
 */
inline void poset_enum_dfs(PosetEnumState& state, int pair_idx,
                           std::vector<PosetEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        out->push_back(poset_extract_hasse(state));
        return;
    }

    int i = state.pairs[static_cast<size_t>(pair_idx)].first;
    int j = state.pairs[static_cast<size_t>(pair_idx)].second;

    // 推移閉包で既に関係が決定済みの場合: 分岐なしで再帰
    if (state.rel[static_cast<size_t>(i)][static_cast<size_t>(j)] ||
        state.rel[static_cast<size_t>(j)][static_cast<size_t>(i)]) {
        poset_enum_dfs(state, pair_idx + 1, out);
        return;
    }

    // 分岐 0: 非comparable (i ‖ j)
    state.incomp[static_cast<size_t>(i)][static_cast<size_t>(j)] = 1;
    state.incomp[static_cast<size_t>(j)][static_cast<size_t>(i)] = 1;
    poset_enum_dfs(state, pair_idx + 1, out);
    state.incomp[static_cast<size_t>(i)][static_cast<size_t>(j)] = 0;
    state.incomp[static_cast<size_t>(j)][static_cast<size_t>(i)] = 0;

    // 分岐 1: i < j
    poset_try_add_relation(state, i, j, pair_idx, out);

    // 分岐 2: j < i
    poset_try_add_relation(state, j, i, pair_idx, out);
}

} // namespace detail_poset_enum

/**
 * @brief 要素集合 {1, ..., n} 上のラベル付き半順序を全列挙する
 * @param n 要素数
 * @return PosetEnumerationResult
 *
 * n(n-1)/2 個の非順序ペアに対し、各ペアの関係を 3 通りから選択する
 * DFS で全ラベル付き半順序を構成する。推移閉包の増分管理により
 * サイクルを早期に検出して枝刈りを行う。出力は Hasse 図 (被覆関係)。
 */
inline PosetEnumerationResult enumerate_posets(int n) {
    PosetEnumerationResult result;
    if (n <= 0) return result;
    if (n == 1) {
        PosetEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_poset_enum::PosetEnumState state =
        detail_poset_enum::poset_build_state(n);
    detail_poset_enum::poset_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
