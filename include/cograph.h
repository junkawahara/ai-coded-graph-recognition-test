#ifndef GRAPH_RECOGNITION_COGRAPH_H
#define GRAPH_RECOGNITION_COGRAPH_H

/**
 * @file cograph.h
 * @brief 余グラフ (cograph) 認識
 *
 * 連結成分 / 補グラフ連結成分の再帰的分解によりコグラフを認識する。
 *
 * COTREE: 元の余木分解アルゴリズム。補連結成分探索で unvisited 全体をスキャン。
 * PARTITION_REFINEMENT: 双方向連結リストを用いた高速補連結成分探索。
 *   各 BFS ステップで隣接頂点を一時除去し、残りを一括移動する。
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief コグラフ認識アルゴリズムの選択
 */
enum class CographAlgorithm {
    COTREE,              /**< 余木分解 */
    PARTITION_REFINEMENT /**< 分割細分化による高速余木分解 (デフォルト) */
};

/**
 * @brief コグラフ認識の結果
 */
struct CographResult {
    bool is_cograph; /**< コグラフであれば true */
};

namespace detail {

/** @brief コグラフの再帰的チェッカー (元のアルゴリズム) */
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

/** @brief コグラフ認識 (元のアルゴリズム) */
inline CographResult check_cograph_cotree(const Graph& g) {
    CographResult res;
    CographChecker checker(g);
    res.is_cograph = checker.run();
    return res;
}

/** @brief コグラフの再帰的チェッカー (高速版: 分割細分化) */
class CographCheckerFast {
public:
    explicit CographCheckerFast(const Graph& graph)
        : g(graph),
          in_subset(graph.n + 1, 0),
          seen(graph.n + 1, 0),
          ll_nxt(graph.n + 1, 0),
          ll_prv(graph.n + 1, 0),
          in_remaining(graph.n + 1, 0),
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
    std::vector<int> ll_nxt;
    std::vector<int> ll_prv;
    std::vector<unsigned char> in_remaining;
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

    /** @brief 双方向連結リストから頂点を除去する */
    void ll_remove(int v) {
        ll_nxt[ll_prv[v]] = ll_nxt[v];
        ll_prv[ll_nxt[v]] = ll_prv[v];
        in_remaining[v] = 0;
    }

    /** @brief 双方向連結リストの sentinel 直後に頂点を挿入する */
    void ll_insert_front(int v) {
        ll_nxt[v] = ll_nxt[0];
        ll_prv[v] = 0;
        ll_prv[ll_nxt[0]] = v;
        ll_nxt[0] = v;
        in_remaining[v] = 1;
    }

    /**
     * @brief 補グラフの連結成分を高速に求める
     *
     * 双方向連結リストで remaining 集合を管理する。
     * 各 BFS ステップで:
     *   1. dequeue した v の隣接頂点を remaining から一時除去
     *   2. remaining に残った全頂点 (= 補グラフでの隣接) をコンポーネントに移動
     *   3. 一時除去した頂点を remaining に復元
     */
    void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();
        int k = (int)verts.size();
        if (k == 0) return;

        // 双方向連結リスト構築 (sentinel = 0)
        int sentinel = 0;
        ll_nxt[sentinel] = verts[0];
        ll_prv[verts[0]] = sentinel;
        for (int i = 0; i < k - 1; ++i) {
            ll_nxt[verts[i]] = verts[i + 1];
            ll_prv[verts[i + 1]] = verts[i];
        }
        ll_nxt[verts[k - 1]] = sentinel;
        ll_prv[sentinel] = verts[k - 1];
        for (int i = 0; i < k; ++i) in_remaining[verts[i]] = 1;

        std::queue<int> q;
        std::vector<int> temp_removed;

        while (ll_nxt[sentinel] != sentinel) {
            int s = ll_nxt[sentinel];
            ll_remove(s);

            std::vector<int> comp;
            comp.push_back(s);
            q.push(s);

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                // Step 1: v の G 上の隣接頂点を remaining から一時除去
                temp_removed.clear();
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_remaining[u]) {
                        ll_remove(u);
                        temp_removed.push_back(u);
                    }
                }

                // Step 2: remaining に残った全頂点を comp に移動
                // (これらは v の補グラフ上の隣接頂点)
                while (ll_nxt[sentinel] != sentinel) {
                    int u = ll_nxt[sentinel];
                    ll_remove(u);
                    comp.push_back(u);
                    q.push(u);
                }

                // Step 3: 一時除去した隣接頂点を remaining に復元
                for (size_t j = 0; j < temp_removed.size(); ++j) {
                    ll_insert_front(temp_removed[j]);
                }
            }

            comps.push_back(comp);
        }
    }
};

/** @brief コグラフ認識 (高速版: 分割細分化) */
inline CographResult check_cograph_partition(const Graph& g) {
    CographResult res;
    CographCheckerFast checker(g);
    res.is_cograph = checker.run();
    return res;
}

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
    CographAlgorithm algo = CographAlgorithm::PARTITION_REFINEMENT) {
    switch (algo) {
        case CographAlgorithm::COTREE:
            return detail::check_cograph_cotree(g);
        case CographAlgorithm::PARTITION_REFINEMENT:
            return detail::check_cograph_partition(g);
    }
    return CographResult();
}

} // namespace graph_recognition

#endif
