#ifndef GRAPH_RECOGNITION_COGRAPH_H
#define GRAPH_RECOGNITION_COGRAPH_H

/**
 * @file cograph.h
 * @brief 余グラフ (cograph) 認識
 *
 * 連結成分 / 補グラフ連結成分の再帰的分解によりコグラフを認識する。
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief コグラフ認識アルゴリズムの選択
 */
enum class CographAlgorithm {
    COTREE /**< 余木分解 */
};

/**
 * @brief コグラフ認識の結果
 */
struct CographResult {
    bool is_cograph; /**< コグラフであれば true */
};

namespace detail {

/** @brief コグラフの再帰的チェッカー (内部クラス) */
class CographChecker {
public:
    explicit CographChecker(const Graph& graph)
        : g(graph),
          in_subset(graph.n + 1, 0),
          seen(graph.n + 1, 0),
          subset_token(0),
          seen_token(0) {}

    bool run() {
        std::vector<int> verts;
        verts.reserve(g.n);
        for (int v = 1; v <= g.n; ++v) verts.push_back(v);
        return solve(verts);
    }

private:
    const Graph& g;
    std::vector<int> in_subset;
    std::vector<int> seen;
    int subset_token;
    int seen_token;

    bool solve(const std::vector<int>& verts) {
        if ((int)verts.size() <= 1) return true;

        std::vector<std::vector<int>> comps;
        graph_components(verts, comps);
        if ((int)comps.size() > 1) {
            for (size_t i = 0; i < comps.size(); ++i) {
                if (!solve(comps[i])) return false;
            }
            return true;
        }

        std::vector<std::vector<int>> cocomps;
        complement_components(verts, cocomps);
        if ((int)cocomps.size() > 1) {
            for (size_t i = 0; i < cocomps.size(); ++i) {
                if (!solve(cocomps[i])) return false;
            }
            return true;
        }

        return false;
    }

    void graph_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();
        subset_token++;
        seen_token++;
        for (size_t i = 0; i < verts.size(); ++i) {
            in_subset[verts[i]] = subset_token;
        }

        std::queue<int> q;
        for (size_t i = 0; i < verts.size(); ++i) {
            int s = verts[i];
            if (seen[s] == seen_token) continue;
            std::vector<int> comp;
            seen[s] = seen_token;
            q.push(s);
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                comp.push_back(v);
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_subset[u] != subset_token) continue;
                    if (seen[u] == seen_token) continue;
                    seen[u] = seen_token;
                    q.push(u);
                }
            }
            comps.push_back(comp);
        }
    }

    void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();

        std::vector<int> unvisited = verts;
        std::vector<unsigned char> alive(g.n + 1, 0);
        for (size_t i = 0; i < verts.size(); ++i) alive[verts[i]] = 1;

        std::queue<int> q;
        while (!unvisited.empty()) {
            int s = unvisited.back();
            unvisited.pop_back();
            if (!alive[s]) continue;
            alive[s] = 0;

            std::vector<int> comp;
            comp.push_back(s);
            q.push(s);

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                std::vector<int> next_unvisited;
                next_unvisited.reserve(unvisited.size());
                for (size_t i = 0; i < unvisited.size(); ++i) {
                    int u = unvisited[i];
                    if (!alive[u]) continue;
                    if (!g.has_edge(v, u)) {
                        alive[u] = 0;
                        comp.push_back(u);
                        q.push(u);
                    } else {
                        next_unvisited.push_back(u);
                    }
                }
                unvisited.swap(next_unvisited);
            }

            comps.push_back(comp);
        }
    }
};

} // namespace detail

/**
 * @brief グラフがコグラフか判定する
 * @param g 入力グラフ
 * @return CographResult
 *
 * 2 頂点以上の任意の誘導部分グラフが非連結または補グラフが非連結であれば
 * コグラフ。再帰的に連結成分 / 補連結成分に分解して判定する。
 */
inline CographResult check_cograph(const Graph& g,
    CographAlgorithm algo = CographAlgorithm::COTREE) {
    (void)algo;
    CographResult res;
    detail::CographChecker checker(g);
    res.is_cograph = checker.run();
    return res;
}

} // namespace graph_recognition

#endif
