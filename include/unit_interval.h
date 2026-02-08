#ifndef GRAPH_RECOGNITION_UNIT_INTERVAL_H
#define GRAPH_RECOGNITION_UNIT_INTERVAL_H

/**
 * @file unit_interval.h
 * @brief ユニットインターバルグラフ (unit interval graph) 認識
 *
 * 単純グラフでは unit interval graph と proper interval graph は同値。
 */

#include "graph.h"
#include "proper_interval.h"

namespace graph_recognition {

/**
 * @brief ユニットインターバルグラフ認識の結果
 */
struct UnitIntervalResult {
    bool is_unit_interval; /**< ユニットインターバルグラフであれば true */
};

/**
 * @brief グラフがユニットインターバルグラフか判定する
 * @param g 入力グラフ
 * @return UnitIntervalResult
 */
inline UnitIntervalResult check_unit_interval(const Graph& g) {
    UnitIntervalResult res;
    res.is_unit_interval = false;

    ProperIntervalResult pres = check_proper_interval(g);
    if (!pres.is_proper_interval) return res;

    res.is_unit_interval = true;
    return res;
}

} // namespace graph_recognition

#endif
