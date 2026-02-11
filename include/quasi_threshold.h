#ifndef GRAPH_RECOGNITION_QUASI_THRESHOLD_H
#define GRAPH_RECOGNITION_QUASI_THRESHOLD_H

/**
 * @file quasi_threshold.h
 * @brief 準しきい値グラフ (quasi-threshold graph) 認識
 *
 * quasi-threshold graph は trivially perfect graph の別名。
 */

#include "graph.h"
#include "trivially_perfect.h"

namespace graph_recognition {

/**
 * @brief 準しきい値グラフ認識アルゴリズムの選択
 */
enum class QuasiThresholdAlgorithm {
    DFS /**< DFS による判定 */
};

/**
 * @brief 準しきい値グラフ認識の結果
 */
struct QuasiThresholdResult {
    bool is_quasi_threshold; /**< 準しきい値グラフであれば true */
};

/**
 * @brief グラフが準しきい値グラフか判定する
 * @param g 入力グラフ
 * @return QuasiThresholdResult
 */
inline QuasiThresholdResult check_quasi_threshold(const Graph& g,
    QuasiThresholdAlgorithm algo = QuasiThresholdAlgorithm::DFS) {
    (void)algo;
    QuasiThresholdResult res;
    res.is_quasi_threshold = false;

    TriviallyPerfectResult tres = check_trivially_perfect(g);
    if (!tres.is_trivially_perfect) return res;

    res.is_quasi_threshold = true;
    return res;
}

} // namespace graph_recognition

#endif
