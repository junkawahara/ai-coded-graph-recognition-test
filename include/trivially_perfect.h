#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_H

/**
 * @file trivially_perfect.h
 * @brief 自明完全グラフ (trivially perfect graph) 認識
 *
 * 弦グラフかつコグラフであれば自明完全グラフと判定する。
 */

#include "chordal.h"
#include "cograph.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief 自明完全グラフ認識アルゴリズムの選択
 */
enum class TriviallyPerfectAlgorithm {
    DFS /**< DFS による判定 */
};

/**
 * @brief 自明完全グラフ認識の結果
 */
struct TriviallyPerfectResult {
    bool is_trivially_perfect; /**< 自明完全グラフであれば true */
};

/**
 * @brief グラフが自明完全グラフか判定する
 * @param g 入力グラフ
 * @return TriviallyPerfectResult
 *
 * G が自明完全グラフ ⟺ G が弦グラフかつコグラフ。
 */
inline TriviallyPerfectResult check_trivially_perfect(const Graph& g,
    TriviallyPerfectAlgorithm algo = TriviallyPerfectAlgorithm::DFS) {
    (void)algo;
    TriviallyPerfectResult res;
    res.is_trivially_perfect = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    CographResult cograph = check_cograph(g);
    if (!cograph.is_cograph) return res;

    res.is_trivially_perfect = true;
    return res;
}

} // namespace graph_recognition

#endif
