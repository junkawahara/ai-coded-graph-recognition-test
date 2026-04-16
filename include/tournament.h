#ifndef GRAPH_RECOGNITION_TOURNAMENT_H
#define GRAPH_RECOGNITION_TOURNAMENT_H

/**
 * @file tournament.h
 * @brief トーナメント (tournament) 認識
 *
 * トーナメントは完全有向グラフ: 任意の頂点対 {u, v} に対し
 * u→v または v→u の弧がちょうど 1 本存在する。
 *
 * 入力形式: n m (頂点数, 弧数) 続いて m 行の弧 u v (u→v)
 */

#include <iostream>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief トーナメント認識アルゴリズムの選択
 */
enum class TournamentAlgorithm {
    ARC_CHECK /**< 全頂点対の弧チェック */
};

/**
 * @brief トーナメント認識の結果
 */
struct TournamentResult {
    bool is_tournament = false; /**< トーナメントであれば true */
};

/**
 * @brief 有向グラフの弧リストがトーナメントか判定する
 * @param n 頂点数
 * @param arcs 弧リスト (u, v) = u→v
 * @param algo 使用アルゴリズム
 * @return TournamentResult
 *
 * トーナメント ⟺ m = n(n-1)/2 かつ各頂点対に弧がちょうど 1 本。
 */
inline TournamentResult check_tournament(int n,
    const std::vector<std::pair<int, int>>& arcs,
    TournamentAlgorithm algo = TournamentAlgorithm::ARC_CHECK) {
    (void)algo;
    TournamentResult res;

    if (n == 0) {
        res.is_tournament = true;
        return res;
    }

    /* 弧数チェック: m = n(n-1)/2 */
    long long expected = (long long)n * (n - 1) / 2;
    if ((long long)arcs.size() != expected) return res;

    /* 隣接行列で弧を記録 */
    std::vector<std::vector<char>> has_arc(n + 1, std::vector<char>(n + 1, 0));
    for (size_t i = 0; i < arcs.size(); ++i) {
        int u = arcs[i].first, v = arcs[i].second;
        if (u < 1 || u > n || v < 1 || v > n) return res;
        if (u == v) return res; /* 自己ループ不可 */
        if (has_arc[u][v]) return res; /* 重複弧不可 */
        has_arc[u][v] = 1;
    }

    /* 全頂点対の検証: ちょうど一方向の弧が存在 */
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            int cnt = has_arc[u][v] + has_arc[v][u];
            if (cnt != 1) return res;
        }
    }

    res.is_tournament = true;
    return res;
}

/**
 * @brief 標準入力からトーナメントの入力を読み込む
 * @param in 入力ストリーム
 * @param[out] n 頂点数
 * @param[out] arcs 弧リスト
 */
inline void read_directed(std::istream& in, int& n,
    std::vector<std::pair<int, int>>& arcs) {
    int m;
    n = 0;
    arcs.clear();
    if (!(in >> n >> m)) return;
    if (n < 0 || m < 0) { n = 0; return; }
    arcs.reserve(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        if (!(in >> u >> v)) break;
        arcs.push_back(std::make_pair(u, v));
    }
}

} // namespace graph_recognition

#endif
