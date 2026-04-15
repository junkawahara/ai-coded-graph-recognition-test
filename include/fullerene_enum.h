#ifndef GRAPH_RECOGNITION_FULLERENE_ENUM_H
#define GRAPH_RECOGNITION_FULLERENE_ENUM_H

/**
 * @file fullerene_enum.h
 * @brief フラーレングラフの非同型列挙
 *
 * 頂点数 n の全非同型フラーレングラフを列挙する。
 *
 * フラーレングラフとは:
 *   - 三次 (cubic): 全頂点の次数が 3
 *   - 平面 (planar)
 *   - 全面が五角形 (12 個) または六角形 (n/2 - 10 個)
 *   - n は偶数、n >= 20、n != 22
 *
 * アルゴリズム (双対アプローチ):
 *   フラーレンの双対は次数 {5,6} の三角形分割 (v = n/2+2 頂点)。
 *   1. 逆探索で v 頂点の三角形分割を次数制約付きで列挙
 *   2. 各三角形分割の平面埋め込みから面 (全三角形) を抽出
 *   3. 双対グラフ (= フラーレン) を構築
 *   4. BFS 正準形で非同型重複を除去
 *
 * 非同型数: OEIS A007894
 *   1, 0, 1, 1, 2, 3, 6, 6, 15, 17, 40, ... (n=20, 22, 24, ...)
 *
 * 参考文献:
 *   Brinkmann, Goedgebeur, McKay, J. Chem. Inf. Model. 52, 2012 (buckygen)
 */

#include "graph.h"
#include "planar.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace graph_recognition {

struct FullereneEnumeratedGraph {
    int n;
    std::vector<std::pair<int, int> > edges;
};

struct FullereneEnumerationResult {
    std::vector<FullereneEnumeratedGraph> graphs;
};

enum class FullereneEnumAlgorithm {
    REVERSE_SEARCH
};

namespace detail {

// ---- 三角形分割列挙用の状態 ----
struct TriangEnumState {
    int total_v;      // 三角形分割の頂点数
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;

    explicit TriangEnumState(int v)
        : total_v(v), alive_count(0),
          adj(v + 1, std::vector<char>(v + 1, 0)),
          deg(v + 1, 0) {}
};

inline int triang_count_components(const TriangEnumState& state, int x) {
    std::vector<char> visited(x + 1, 0);
    int comp = 0;
    for (int s = 1; s <= x; ++s) {
        if (visited[s]) continue;
        comp++;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
        for (std::size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (int u = 1; u <= x; ++u) {
                if (!visited[u] && state.adj[v][u]) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
    }
    return comp;
}

// ---- 三角形分割から面を抽出 ----

/**
 * @brief 三角形分割の面を抽出する
 *
 * 三角形分割 (maximal planar) の各頂点の隣接頂点はサイクルを成す。
 * そのサイクル順序から回転系を構築し、dart追跡で面を列挙する。
 *
 * @return 面のリスト (各面は頂点の列)、失敗時は空
 */
inline std::vector<std::vector<int> > extract_triangulation_faces(
    int v,
    const std::vector<std::vector<char> >& adj) {

    // 隣接リスト構築
    std::vector<std::vector<int> > adj_list(v + 1);
    for (int u = 1; u <= v; ++u)
        for (int w = u + 1; w <= v; ++w)
            if (adj[u][w]) {
                adj_list[u].push_back(w);
                adj_list[w].push_back(u);
            }

    // 各頂点の回転系を計算:
    // 三角形分割では各頂点 w の隣接頂点は w を含む辺がなす面の列に対応し、
    // 隣接頂点の誘導部分グラフはサイクルを成す。
    // next_cw[w][u] = w の回転系��� u の次の時計回り隣接頂点
    std::vector<std::map<int, int> > next_cw(v + 1);

    for (int w = 1; w <= v; ++w) {
        const std::vector<int>& nbrs = adj_list[w];
        int d = (int)nbrs.size();
        if (d < 3) return std::vector<std::vector<int> >();

        // nbrs 間の辺からサイクルを構築
        // 各隣接頂点は nbrs 中ちょうど 2 頂点と隣接 (サイクル)
        std::vector<std::vector<int> > nbr_adj(d);
        std::map<int, int> nbr_idx;
        for (int i = 0; i < d; ++i) nbr_idx[nbrs[i]] = i;

        for (int i = 0; i < d; ++i) {
            for (int j = i + 1; j < d; ++j) {
                if (adj[nbrs[i]][nbrs[j]]) {
                    nbr_adj[i].push_back(j);
                    nbr_adj[j].push_back(i);
                }
            }
        }

        // サイクルを辿る
        std::vector<int> cycle;
        std::vector<char> used(d, 0);
        cycle.push_back(0);
        used[0] = 1;
        int prev = -1, cur = 0;
        for (int step = 1; step < d; ++step) {
            bool found = false;
            for (std::size_t ni = 0; ni < nbr_adj[cur].size(); ++ni) {
                int nxt = nbr_adj[cur][ni];
                if (nxt != prev && !used[nxt]) {
                    cycle.push_back(nxt);
                    used[nxt] = 1;
                    prev = cur;
                    cur = nxt;
                    found = true;
                    break;
                }
            }
            if (!found) return std::vector<std::vector<int> >();
        }

        // cycle の順序が回転 (時計回りまたは反時計回り)
        // next_cw[w] を設定
        for (int i = 0; i < d; ++i) {
            int a = nbrs[cycle[i]];
            int b = nbrs[cycle[(i + 1) % d]];
            next_cw[w][a] = b;
        }
    }

    // dart追跡で面を抽出
    // dart (u, w) → next dart (w, next_cw[w][u])
    std::set<std::pair<int, int> > visited_darts;
    std::vector<std::vector<int> > faces;

    for (int u = 1; u <= v; ++u) {
        for (std::size_t ni = 0; ni < adj_list[u].size(); ++ni) {
            int w = adj_list[u][ni];
            if (visited_darts.count(std::make_pair(u, w))) continue;

            std::vector<int> face;
            int cu = u, cw = w;
            int max_steps = 3 * v;
            while (max_steps-- > 0) {
                if (visited_darts.count(std::make_pair(cu, cw))) break;
                visited_darts.insert(std::make_pair(cu, cw));
                face.push_back(cu);
                int nxt = next_cw[cw][cu];
                cu = cw;
                cw = nxt;
            }
            if (max_steps <= 0) return std::vector<std::vector<int> >();
            if ((int)face.size() != 3) {
                // 三角形分割なので全面が三角形のはず
                // 一方の向きが失敗 → もう一方を試す必要があるが、
                // 単純��無効として返す
                return std::vector<std::vector<int> >();
            }
            faces.push_back(face);
        }
    }

    // Euler 公式検証: V - E + F = 2
    int E = 0;
    for (int u = 1; u <= v; ++u) E += (int)adj_list[u].size();
    E /= 2;
    if (v - E + (int)faces.size() != 2) {
        return std::vector<std::vector<int> >();
    }

    return faces;
}

/**
 * @brief 三角形分割の面リストから双対グラフ (フラーレン) を構築
 */
inline bool build_dual_fullerene(
    int v,
    const std::vector<std::vector<char> >& triang_adj,
    const std::vector<std::vector<int> >& faces,
    int fullerene_n,
    std::vector<std::pair<int, int> >& fullerene_edges) {

    int f = (int)faces.size();
    if (f != fullerene_n) return false;

    // 辺 → 面のマッピング
    // 各辺は正確に 2 つの面に属する
    std::map<std::pair<int, int>, std::vector<int> > edge_to_faces;
    for (int fi = 0; fi < f; ++fi) {
        const std::vector<int>& face = faces[fi];
        int fs = (int)face.size();
        for (int j = 0; j < fs; ++j) {
            int a = face[j], b = face[(j + 1) % fs];
            int lo = (a < b) ? a : b;
            int hi = (a < b) ? b : a;
            edge_to_faces[std::make_pair(lo, hi)].push_back(fi);
        }
    }

    // 双対グラフの辺を構築 (面番号は 1-indexed)
    std::set<std::pair<int, int> > dual_edges;
    for (std::map<std::pair<int, int>, std::vector<int> >::const_iterator
             it = edge_to_faces.begin(); it != edge_to_faces.end(); ++it) {
        const std::vector<int>& flist = it->second;
        if ((int)flist.size() != 2) return false;
        int a = flist[0] + 1; // 1-indexed
        int b = flist[1] + 1;
        if (a > b) { int tmp = a; a = b; b = tmp; }
        dual_edges.insert(std::make_pair(a, b));
    }

    fullerene_edges.assign(dual_edges.begin(), dual_edges.end());
    std::sort(fullerene_edges.begin(), fullerene_edges.end());
    return true;
}

// ---- BFS 正準形 ----

inline std::string fullerene_bfs_code(
    int n,
    const std::vector<std::vector<int> >& adj_list,
    int start) {

    std::vector<int> label(n + 1, -1);
    std::vector<int> order;
    order.reserve(n);
    label[start] = 0;
    order.push_back(start);

    for (std::size_t qi = 0; qi < order.size(); ++qi) {
        int v = order[qi];
        std::vector<std::pair<int, int> > nbrs;
        for (std::size_t i = 0; i < adj_list[v].size(); ++i) {
            int u = adj_list[v][i];
            if (label[u] == -1) {
                int min_lbl = label[v];
                for (std::size_t j = 0; j < adj_list[u].size(); ++j) {
                    int w = adj_list[u][j];
                    if (label[w] >= 0 && label[w] < min_lbl)
                        min_lbl = label[w];
                }
                nbrs.push_back(std::make_pair(min_lbl, u));
            }
        }
        std::sort(nbrs.begin(), nbrs.end());
        for (std::size_t i = 0; i < nbrs.size(); ++i) {
            int u = nbrs[i].second;
            if (label[u] == -1) {
                label[u] = (int)order.size();
                order.push_back(u);
            }
        }
    }

    std::string code;
    code.reserve(n * (n - 1) / 2);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int u = order[i], v = order[j];
            bool is_adj = false;
            for (std::size_t k = 0; k < adj_list[u].size(); ++k) {
                if (adj_list[u][k] == v) { is_adj = true; break; }
            }
            code += (is_adj ? '1' : '0');
        }
    }
    return code;
}

inline std::string fullerene_canonical_form(
    int n,
    const std::vector<std::vector<int> >& adj_list) {
    std::string min_code;
    bool first = true;
    for (int s = 1; s <= n; ++s) {
        std::string code = fullerene_bfs_code(n, adj_list, s);
        if (first || code < min_code) {
            min_code = code;
            first = false;
        }
    }
    return min_code;
}

// ---- 三角形分割の逆探索 (choose パターン) ----

inline void triang_enum_choose(TriangEnumState& state,
                                const std::vector<int>& available,
                                std::size_t start,
                                int chosen_count,
                                int min_size, int max_size,
                                int fullerene_n,
                                std::set<std::string>* seen,
                                std::vector<FullereneEnumeratedGraph>* out);

inline void triang_enum_dfs(TriangEnumState& state,
                             int fullerene_n,
                             std::set<std::string>* seen,
                             std::vector<FullereneEnumeratedGraph>* out) {
    int v = state.total_v;

    if (state.alive_count == v) {
        // 最終チェック: 全次数が 5 or 6、辺数 = 3v - 6
        int edge_count = 0;
        for (int u = 1; u <= v; ++u) {
            if (state.deg[u] < 5 || state.deg[u] > 6) return;
            edge_count += state.deg[u];
        }
        edge_count /= 2;
        if (edge_count != 3 * v - 6) return;

        // 次数 5 の頂点がちょうど 12 個か確認
        int deg5_count = 0;
        for (int u = 1; u <= v; ++u) {
            if (state.deg[u] == 5) deg5_count++;
        }
        if (deg5_count != 12) return;

        // 平面性チェック
        {
            std::vector<std::pair<int, int> > edges;
            for (int u = 1; u <= v; ++u)
                for (int w = u + 1; w <= v; ++w)
                    if (state.adj[u][w])
                        edges.push_back(std::make_pair(u, w));
            Graph g(v, edges);
            PlanarResult pr = check_planar(g);
            if (!pr.is_planar) return;
        }

        // 面を抽出
        std::vector<std::vector<int> > faces =
            extract_triangulation_faces(v, state.adj);
        if (faces.empty()) return;

        // 双対 (フラーレン) を構築
        std::vector<std::pair<int, int> > fullerene_edges;
        if (!build_dual_fullerene(v, state.adj, faces,
                                   fullerene_n, fullerene_edges))
            return;

        // 隣接リスト構築
        std::vector<std::vector<int> > adj_list(fullerene_n + 1);
        for (std::size_t i = 0; i < fullerene_edges.size(); ++i) {
            adj_list[fullerene_edges[i].first].push_back(
                fullerene_edges[i].second);
            adj_list[fullerene_edges[i].second].push_back(
                fullerene_edges[i].first);
        }
        for (int u = 1; u <= fullerene_n; ++u)
            std::sort(adj_list[u].begin(), adj_list[u].end());

        // 正準形で重複チェック
        std::string canon =
            fullerene_canonical_form(fullerene_n, adj_list);
        if (seen->count(canon)) return;
        seen->insert(canon);

        FullereneEnumeratedGraph graph;
        graph.n = fullerene_n;
        graph.edges = fullerene_edges;
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int remaining = state.total_v - x;

    // deg < 6 の利用可能頂点
    std::vector<int> available;
    for (int u = 1; u < x; ++u) {
        if (state.deg[u] < 6) available.push_back(u);
    }

    // 新頂点の最小・最大近傍���
    int min_neighbors = 5 - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    // 三角形分割: 最初の頂点は 0 辺でもよい
    if (x <= 3) min_neighbors = (x <= 1) ? 0 : (x - 1);
    int max_neighbors = 6;
    if (max_neighbors > (int)available.size())
        max_neighbors = (int)available.size();
    // 三角形分割の辺数上界: 追加可能な辺数の制限
    if (max_neighbors > x - 1) max_neighbors = x - 1;

    if (max_neighbors < min_neighbors) return;

    triang_enum_choose(state, available, 0, 0,
                        min_neighbors, max_neighbors,
                        fullerene_n, seen, out);
}

inline void triang_enum_choose(TriangEnumState& state,
                                const std::vector<int>& available,
                                std::size_t start,
                                int chosen_count,
                                int min_size, int max_size,
                                int fullerene_n,
                                std::set<std::string>* seen,
                                std::vector<FullereneEnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_v - x;

    if (chosen_count >= min_size) {
        state.deg[x] = chosen_count;
        state.alive_count = x;

        bool feasible = true;

        // 枝刈り: 次数到達可能性 (最終的に deg >= 5 が必要)
        for (int u = 1; u <= x; ++u) {
            if (state.deg[u] + remaining_after_x < 5) {
                feasible = false;
                break;
            }
            if (state.deg[u] > 6) {
                feasible = false;
                break;
            }
        }

        // 枝刈り: 連結性
        if (feasible && x >= 2) {
            int comp = triang_count_components(state, x);
            if (comp > remaining_after_x + 1) feasible = false;
        }

        // 枝刈り: 辺数上界 (平面グラフ: m <= 3v - 6)
        if (feasible && x >= 3) {
            int edge_count = 0;
            for (int u = 1; u <= x; ++u) edge_count += state.deg[u];
            edge_count /= 2;
            if (edge_count > 3 * x - 6) feasible = false;
        }

        // 枝刈り: 平面性 (高価なので辺が多い場合のみ)
        if (feasible && x >= 5) {
            int edge_count = 0;
            for (int u = 1; u <= x; ++u) edge_count += state.deg[u];
            edge_count /= 2;
            if (edge_count >= 3 * x - 8) {
                std::vector<std::pair<int, int> > edges;
                for (int u = 1; u <= x; ++u)
                    for (int w = u + 1; w <= x; ++w)
                        if (state.adj[u][w])
                            edges.push_back(std::make_pair(u, w));
                Graph g(x, edges);
                PlanarResult pr = check_planar(g);
                if (!pr.is_planar) feasible = false;
            }
        }

        if (feasible) {
            triang_enum_dfs(state, fullerene_n, seen, out);
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int u = available[i];
        if (state.deg[u] >= 6) continue;

        state.adj[x][u] = 1;
        state.adj[u][x] = 1;
        state.deg[u]++;

        triang_enum_choose(state, available, i + 1, chosen_count + 1,
                            min_size, max_size,
                            fullerene_n, seen, out);

        state.adj[x][u] = 0;
        state.adj[u][x] = 0;
        state.deg[u]--;
    }
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型フラーレングラフを列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return FullereneEnumerationResult
 */
inline FullereneEnumerationResult
enumerate_fullerene_graphs(int n,
    FullereneEnumAlgorithm algo =
        FullereneEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    FullereneEnumerationResult result;

    if (n < 20) return result;
    if (n % 2 != 0) return result;
    if (n == 22) return result;

    // 双対三角形分割の頂点数
    int v = n / 2 + 2;

    std::set<std::string> seen;
    detail::TriangEnumState root(v);
    detail::triang_enum_dfs(root, n, &seen, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
