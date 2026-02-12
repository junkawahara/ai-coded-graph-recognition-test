#ifndef GRAPH_RECOGNITION_INTERVAL_H
#define GRAPH_RECOGNITION_INTERVAL_H

/**
 * @file interval.h
 * @brief インターバルグラフ (interval graph) 認識
 *
 * 弦性チェック + 極大クリークの consecutive-1s 順序探索により
 * インターバルグラフを認識し、区間モデルを構築する。
 *
 * アルゴリズム:
 *   - BACKTRACKING: バックトラッキングによるクリークパス探索
 *   - AT_FREE: 弦性 + AT-free 判定 (多項式時間)
 */

#include "at_free.h"
#include "chordal.h"
#include "clique.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief インターバルグラフ認識アルゴリズムの選択
 */
enum class IntervalAlgorithm {
    BACKTRACKING, /**< バックトラッキングによるクリークパス探索 */
    AT_FREE       /**< 弦性 + AT-free 判定 (デフォルト) */
};

/**
 * @brief インターバルグラフ認識の結果
 */
struct IntervalResult {
    bool is_interval;   /**< インターバルグラフであれば true */
    /**
     * @brief intervals[v] = (L, R): 頂点 v の区間 (1-indexed)
     *
     * is_interval == true の場合のみ有効。
     */
    std::vector<std::pair<int, int>> intervals;
};

namespace detail {

/**
 * @brief 極大クリークの consecutive-1s 順序をバックトラッキングで探索する
 */
inline bool find_clique_path(
    int k, int n,
    std::vector<int>& clique_order,
    std::vector<bool>& placed,
    std::vector<bool>& finished,
    std::vector<int>& unplaced_count,
    const MaximalCliques& mc,
    const std::vector<std::unordered_set<int>>& cset)
{
    if ((int)clique_order.size() == k) return true;

    int cur = clique_order.back();

    std::vector<int> active;
    for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
        int v = mc.cliques[cur][j];
        if (unplaced_count[v] > 0) active.push_back(v);
    }

    if (active.empty()) {
        for (int c = 0; c < k; ++c) {
            if (placed[c]) continue;
            bool ok = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
                if (finished[mc.cliques[c][j]]) { ok = false; break; }
            }
            if (!ok) continue;

            clique_order.push_back(c);
            placed[c] = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]--;

            std::vector<int> newly_finished;
            for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
                int v = mc.cliques[cur][j];
                if (!finished[v]) {
                    finished[v] = true;
                    newly_finished.push_back(v);
                }
            }

            if (find_clique_path(k, n, clique_order, placed, finished,
                                 unplaced_count, mc, cset))
                return true;

            for (size_t j = 0; j < newly_finished.size(); ++j)
                finished[newly_finished[j]] = false;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]++;
            placed[c] = false;
            clique_order.pop_back();
        }
        return false;
    }

    for (int c = 0; c < k; ++c) {
        if (placed[c]) continue;

        bool ok = true;
        for (size_t j = 0; j < active.size(); ++j) {
            if (cset[c].count(active[j]) == 0) { ok = false; break; }
        }
        if (!ok) continue;

        for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
            if (finished[mc.cliques[c][j]]) { ok = false; break; }
        }
        if (!ok) continue;

        clique_order.push_back(c);
        placed[c] = true;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]--;

        std::vector<int> newly_finished;
        for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
            int v = mc.cliques[cur][j];
            if (cset[c].count(v) == 0 && !finished[v]) {
                finished[v] = true;
                newly_finished.push_back(v);
            }
        }

        if (find_clique_path(k, n, clique_order, placed, finished,
                             unplaced_count, mc, cset))
            return true;

        for (size_t j = 0; j < newly_finished.size(); ++j)
            finished[newly_finished[j]] = false;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]++;
        placed[c] = false;
        clique_order.pop_back();
    }
    return false;
}

/**
 * @brief バックトラッキングによるインターバルグラフ認識
 */
inline IntervalResult check_interval_backtracking(const Graph& g) {
    IntervalResult res;
    res.is_interval = false;
    int n = g.n;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    int k = (int)mc.cliques.size();
    if (k == 0) return res;

    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    std::vector<int> unplaced_count(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        unplaced_count[v] = (int)mc.member[v].size();
    }

    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    std::vector<int> clique_order;
    clique_order.reserve(k);

    std::vector<int> starts;
    for (int i = 0; i < k; ++i) {
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            if ((int)mc.member[mc.cliques[i][j]].size() == 1) {
                starts.push_back(i);
                break;
            }
        }
    }
    if (starts.empty()) {
        starts.push_back(0);
    }

    bool found = false;
    for (size_t si = 0; si < starts.size() && !found; ++si) {
        int s = starts[si];
        clique_order.clear();
        std::fill(placed.begin(), placed.end(), false);
        std::fill(finished.begin(), finished.end(), false);
        for (int v = 1; v <= n; ++v) unplaced_count[v] = (int)mc.member[v].size();

        clique_order.push_back(s);
        placed[s] = true;
        for (size_t j = 0; j < mc.cliques[s].size(); ++j) {
            unplaced_count[mc.cliques[s][j]]--;
        }

        if (find_clique_path(k, n, clique_order, placed, finished,
                                     unplaced_count, mc, cset)) {
            found = true;
        }
    }

    if (!found) return res;

    std::vector<int> pos(k);
    for (int p = 0; p < k; ++p) pos[clique_order[p]] = p;

    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) continue;
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        if (hi - lo + 1 != (int)cl.size()) return res;
    }

    res.intervals.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) {
            res.intervals[v] = std::make_pair(k + v, k + v);
            continue;
        }
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        res.intervals[v] = std::make_pair(lo + 1, hi + 1);
    }

    res.is_interval = true;
    return res;
}

/**
 * @brief AT-free 判定によるインターバルグラフ認識
 */
inline IntervalResult check_interval_at_free(const Graph& g) {
    IntervalResult res;
    res.is_interval = false;
    int n = g.n;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    if (has_asteroidal_triple(g)) return res;

    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    int k = (int)mc.cliques.size();
    if (k == 0) {
        res.is_interval = true;
        res.intervals.resize(n + 1);
        for (int v = 1; v <= n; ++v) {
            res.intervals[v] = std::make_pair(v, v);
        }
        return res;
    }

    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    std::vector<int> unplaced_count(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        unplaced_count[v] = (int)mc.member[v].size();
    }

    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    std::vector<int> clique_order;
    clique_order.reserve(k);

    std::vector<int> starts;
    for (int i = 0; i < k; ++i) {
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            if ((int)mc.member[mc.cliques[i][j]].size() == 1) {
                starts.push_back(i);
                break;
            }
        }
    }
    if (starts.empty()) {
        starts.push_back(0);
    }

    bool found = false;
    for (size_t si = 0; si < starts.size() && !found; ++si) {
        int s = starts[si];
        clique_order.clear();
        std::fill(placed.begin(), placed.end(), false);
        std::fill(finished.begin(), finished.end(), false);
        for (int v = 1; v <= n; ++v) unplaced_count[v] = (int)mc.member[v].size();

        clique_order.push_back(s);
        placed[s] = true;
        for (size_t j = 0; j < mc.cliques[s].size(); ++j) {
            unplaced_count[mc.cliques[s][j]]--;
        }

        if (find_clique_path(k, n, clique_order, placed, finished,
                                     unplaced_count, mc, cset)) {
            found = true;
        }
    }

    if (!found) return res;

    std::vector<int> pos(k);
    for (int p = 0; p < k; ++p) pos[clique_order[p]] = p;

    res.intervals.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) {
            res.intervals[v] = std::make_pair(k + v, k + v);
            continue;
        }
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        res.intervals[v] = std::make_pair(lo + 1, hi + 1);
    }

    res.is_interval = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフがインターバルグラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: AT_FREE)
 * @return IntervalResult
 */
inline IntervalResult check_interval(const Graph& g,
    IntervalAlgorithm algo = IntervalAlgorithm::AT_FREE) {
    switch (algo) {
        case IntervalAlgorithm::BACKTRACKING:
            return detail::check_interval_backtracking(g);
        case IntervalAlgorithm::AT_FREE:
            return detail::check_interval_at_free(g);
    }
    return IntervalResult();
}

} // namespace graph_recognition

#endif
