#ifndef GRAPH_RECOGNITION_PTOLEMAIC_H
#define GRAPH_RECOGNITION_PTOLEMAIC_H

/**
 * @file ptolemaic.h
 * @brief プトレマイオスグラフ (Ptolemaic graph) 認識
 *
 * 弦性と距離遺伝性の両方を満たすグラフを認識する。
 *
 * アルゴリズム:
 *   - DH_HASHMAP: 弦性 + ハッシュマップ DH 判定
 *   - DH_SORTED: 弦性 + ソート済み DH 判定 (デフォルト)
 */

#include "chordal.h"
#include "distance_hereditary.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief プトレマイオスグラフ認識アルゴリズムの選択
 */
enum class PtolemaicAlgorithm {
    DH_HASHMAP, /**< 弦性 + ハッシュマップ DH 判定 */
    DH_SORTED   /**< 弦性 + ソート済み DH 判定 (デフォルト) */
};

/**
 * @brief プトレマイオスグラフ認識の結果
 */
struct PtolemaicResult {
    bool is_ptolemaic; /**< プトレマイオスグラフであれば true */
};

/**
 * @brief グラフがプトレマイオスグラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DH_SORTED)
 * @return PtolemaicResult
 *
 * G がプトレマイオスグラフ ⟺ G が弦グラフかつ距離遺伝グラフ。
 */
inline PtolemaicResult check_ptolemaic(const Graph& g,
    PtolemaicAlgorithm algo = PtolemaicAlgorithm::DH_SORTED) {
    PtolemaicResult res;
    res.is_ptolemaic = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    DistanceHereditaryAlgorithm dh_algo;
    switch (algo) {
        case PtolemaicAlgorithm::DH_HASHMAP:
            dh_algo = DistanceHereditaryAlgorithm::HASHMAP_TWINS;
            break;
        case PtolemaicAlgorithm::DH_SORTED:
            dh_algo = DistanceHereditaryAlgorithm::SORTED_TWINS;
            break;
        default:
            dh_algo = DistanceHereditaryAlgorithm::SORTED_TWINS;
            break;
    }

    DistanceHereditaryResult dh = check_distance_hereditary(g, dh_algo);
    if (!dh.is_distance_hereditary) return res;

    res.is_ptolemaic = true;
    return res;
}

} // namespace graph_recognition

#endif
