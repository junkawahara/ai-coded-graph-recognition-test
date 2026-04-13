#ifndef GRAPH_RECOGNITION_SIMPLE_QUADRANGULATION_ENUM_H
#define GRAPH_RECOGNITION_SIMPLE_QUADRANGULATION_ENUM_H

/**
 * @file simple_quadrangulation_enum.h
 * @brief 単純四角形分割 (simple quadrangulation) の非同型列挙
 *
 * 頂点数 n の全非同型単純四角形分割を列挙する。
 *
 * アルゴリズム (補グラフ双対アプローチ):
 *   1. v = n-2 頂点の 3-正則 (cubic) グラフを列挙 (cubic_enum.h)
 *   2. 各 cubic グラフの補グラフを取得 (= (v-4)-正則 → v=8 なら 4-正則)
 *      一般には: 補グラフは (v-1-3) = (v-4)-正則
 *      v-4 = 4 のとき v = 8, つまり n = 10 の場合のみ有効
 *      一般的な v に対しては直接 4-正則を列挙
 *   [修正: 4-正則グラフの双対が四角形分割]
 *   3. 4-正則平面 3-連結グラフの面を回転系バックトラッキングで抽出
 *   4. 双対グラフ (= 四角形分割) を構築
 *   5. 全置換正準形で非同型重複を除去
 *
 * 非同型数: OEIS A078666
 *   1, 2, 6, 16, 51, 199, 819, ... (n=8, 10, 12, ...)
 *
 * 参考文献:
 *   Brinkmann, McKay, Discrete Math. 305, 2005
 */

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "kregular_enum.h"
#include "planar.h"
#include "triconnected.h"

namespace graph_recognition {

struct SimpleQuadEnumeratedGraph {
    int n;
    std::vector<std::pair<int, int> > edges;
};

struct SimpleQuadrangulationEnumerationResult {
    std::vector<SimpleQuadEnumeratedGraph> graphs;
};

enum class SimpleQuadrangulationEnumAlgorithm {
    REVERSE_SEARCH
};

namespace detail {

// ---- 面抽出 ----

inline std::vector<std::vector<int> > sq_trace_all_faces(
    int v,
    const std::vector<std::vector<int> >& adj_list,
    const std::vector<std::map<int, int> >& next_cw) {

    std::set<std::pair<int, int> > visited_darts;
    std::vector<std::vector<int> > faces;

    for (int s = 1; s <= v; ++s) {
        for (std::size_t ni = 0; ni < adj_list[s].size(); ++ni) {
            int u = adj_list[s][ni];
            if (visited_darts.count(std::make_pair(s, u))) continue;

            std::vector<int> face;
            int cu = s, cv = u;
            int max_steps = 4 * v + 4;
            bool ok = true;
            while (max_steps-- > 0) {
                if (visited_darts.count(std::make_pair(cu, cv))) {
                    if (cu != s || cv != u) ok = false;
                    break;
                }
                visited_darts.insert(std::make_pair(cu, cv));
                face.push_back(cu);
                std::map<int, int>::const_iterator it = next_cw[cv].find(cu);
                if (it == next_cw[cv].end()) { ok = false; break; }
                int w = it->second;
                cu = cv;
                cv = w;
            }
            if (max_steps < 0) ok = false;
            if (!ok) return std::vector<std::vector<int> >();
            faces.push_back(face);
        }
    }

    int E = 0;
    for (int u = 1; u <= v; ++u) E += (int)adj_list[u].size();
    E /= 2;
    if (v - E + (int)faces.size() != 2)
        return std::vector<std::vector<int> >();
    return faces;
}

inline bool sq_try_rotations(
    int v,
    const std::vector<std::vector<int> >& adj_list,
    std::vector<std::vector<int> >& rotations,
    int cur_vertex,
    std::vector<std::vector<int> >& out_faces) {

    if (cur_vertex > v) {
        std::vector<std::map<int, int> > next_cw(v + 1);
        for (int u = 1; u <= v; ++u) {
            int d = (int)rotations[u].size();
            for (int i = 0; i < d; ++i)
                next_cw[u][rotations[u][i]] = rotations[u][(i + 1) % d];
        }
        std::vector<std::vector<int> > faces =
            sq_trace_all_faces(v, adj_list, next_cw);
        if (!faces.empty()) {
            out_faces = faces;
            return true;
        }
        return false;
    }

    std::sort(rotations[cur_vertex].begin() + 1,
              rotations[cur_vertex].end());
    do {
        // 増分整合性チェック
        bool consistent = true;
        int d = (int)rotations[cur_vertex].size();

        for (int ni = 0; ni < d && consistent; ++ni) {
            int w = rotations[cur_vertex][ni];
            if (w > cur_vertex) continue;

            int cu = cur_vertex, cv = w;
            int max_steps = 4 * v + 4;
            std::set<std::pair<int, int> > seen;
            while (max_steps-- > 0 && consistent) {
                if (seen.count(std::make_pair(cu, cv))) break;
                seen.insert(std::make_pair(cu, cv));
                if (cv > cur_vertex) break;
                int ri = -1;
                for (int j = 0; j < (int)rotations[cv].size(); ++j) {
                    if (rotations[cv][j] == cu) { ri = j; break; }
                }
                if (ri == -1) { consistent = false; break; }
                int nxt = rotations[cv][(ri + 1) % (int)rotations[cv].size()];
                cu = cv;
                cv = nxt;
            }
        }

        if (consistent) {
            if (sq_try_rotations(v, adj_list, rotations,
                                 cur_vertex + 1, out_faces))
                return true;
        }
    } while (std::next_permutation(rotations[cur_vertex].begin() + 1,
                                    rotations[cur_vertex].end()));
    return false;
}

inline std::vector<std::vector<int> > sq_extract_faces(
    int v,
    const std::vector<std::vector<char> >& adj) {

    std::vector<std::vector<int> > adj_list(v + 1);
    for (int u = 1; u <= v; ++u)
        for (int w = u + 1; w <= v; ++w)
            if (adj[u][w]) {
                adj_list[u].push_back(w);
                adj_list[w].push_back(u);
            }
    for (int u = 1; u <= v; ++u)
        std::sort(adj_list[u].begin(), adj_list[u].end());

    for (int u = 1; u <= v; ++u)
        if ((int)adj_list[u].size() < 3)
            return std::vector<std::vector<int> >();

    std::vector<std::vector<int> > rotations(v + 1);
    for (int u = 1; u <= v; ++u)
        rotations[u] = adj_list[u];

    std::vector<std::vector<int> > faces;
    if (sq_try_rotations(v, adj_list, rotations, 1, faces))
        return faces;
    return std::vector<std::vector<int> >();
}

// ---- 双対構築 ----

inline bool sq_build_dual(
    const std::vector<std::vector<int> >& faces,
    int quad_n,
    std::vector<std::pair<int, int> >& quad_edges) {

    int f = (int)faces.size();
    if (f != quad_n) return false;

    std::map<std::pair<int, int>, std::vector<int> > edge_to_faces;
    for (int fi = 0; fi < f; ++fi) {
        int fs = (int)faces[fi].size();
        for (int j = 0; j < fs; ++j) {
            int a = faces[fi][j], b = faces[fi][(j + 1) % fs];
            int lo = (a < b) ? a : b;
            int hi = (a < b) ? b : a;
            edge_to_faces[std::make_pair(lo, hi)].push_back(fi);
        }
    }

    std::set<std::pair<int, int> > dual_edges;
    for (std::map<std::pair<int, int>, std::vector<int> >::const_iterator
             it = edge_to_faces.begin(); it != edge_to_faces.end(); ++it) {
        if ((int)it->second.size() != 2) return false;
        int a = it->second[0] + 1;
        int b = it->second[1] + 1;
        if (a > b) { int tmp = a; a = b; b = tmp; }
        dual_edges.insert(std::make_pair(a, b));
    }

    quad_edges.assign(dual_edges.begin(), dual_edges.end());
    std::sort(quad_edges.begin(), quad_edges.end());
    return true;
}

// ---- 正準形 (全置換最小コード) ----

inline std::string sq_canonical_form(
    int n,
    const std::vector<std::vector<int> >& al) {
    std::vector<std::set<int> > adj_set(n + 1);
    for (int u = 1; u <= n; ++u)
        for (std::size_t i = 0; i < al[u].size(); ++i)
            adj_set[u].insert(al[u][i]);

    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i + 1;

    std::string min_code;
    bool first = true;

    do {
        std::string code;
        code.reserve(n * (n - 1) / 2);
        bool skip = false;
        for (int i = 0; i < n && !skip; ++i) {
            for (int j = i + 1; j < n && !skip; ++j) {
                char c = adj_set[perm[i]].count(perm[j]) ? '1' : '0';
                code += c;
                if (!first && code > min_code) skip = true;
            }
        }
        if (!skip && (first || code < min_code)) {
            min_code = code;
            first = false;
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    return min_code;
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型単純四角形分割を列挙する
 */
inline SimpleQuadrangulationEnumerationResult
enumerate_simple_quadrangulation_graphs(int n,
    SimpleQuadrangulationEnumAlgorithm algo =
        SimpleQuadrangulationEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    SimpleQuadrangulationEnumerationResult result;

    if (n < 8) return result;
    if (n % 2 != 0) return result;

    int v = n - 2;  // 双対の頂点数

    // 4-正則グラフを直接列挙
    KRegularEnumerationResult kreg_res =
        enumerate_kregular_graphs_reverse_search(v, 4);
    std::set<std::string> seen;

    for (std::size_t gi = 0; gi < kreg_res.graphs.size(); ++gi) {
        const EnumeratedGraph& rg = kreg_res.graphs[gi];

        // 隣接行列を構築
        std::vector<std::vector<char> > adj(v + 1,
            std::vector<char>(v + 1, 0));
        for (std::size_t ei = 0; ei < rg.edges.size(); ++ei) {
            adj[rg.edges[ei].first][rg.edges[ei].second] = 1;
            adj[rg.edges[ei].second][rg.edges[ei].first] = 1;
        }

        // 平面性チェック
        {
            std::vector<std::pair<int, int> > edges;
            for (int u = 1; u <= v; ++u)
                for (int w = u + 1; w <= v; ++w)
                    if (adj[u][w])
                        edges.push_back(std::make_pair(u, w));
            Graph g(v, edges);
            PlanarResult pr = check_planar(g);
            if (!pr.is_planar) continue;

            TriconnectedResult tr = check_triconnected(g);
            if (!tr.is_triconnected) continue;
        }

        // 面を抽出
        std::vector<std::vector<int> > faces =
            detail::sq_extract_faces(v, adj);
        if (faces.empty()) continue;

        // 双対を構築
        std::vector<std::pair<int, int> > quad_edges;
        if (!detail::sq_build_dual(faces, n, quad_edges))
            continue;

        // 隣接リスト
        std::vector<std::vector<int> > al(n + 1);
        for (std::size_t i = 0; i < quad_edges.size(); ++i) {
            al[quad_edges[i].first].push_back(quad_edges[i].second);
            al[quad_edges[i].second].push_back(quad_edges[i].first);
        }
        for (int u = 1; u <= n; ++u)
            std::sort(al[u].begin(), al[u].end());

        // 正準形で重複チェック
        std::string canon = detail::sq_canonical_form(n, al);
        if (seen.count(canon)) continue;
        seen.insert(canon);

        SimpleQuadEnumeratedGraph graph;
        graph.n = n;
        graph.edges = quad_edges;
        result.graphs.push_back(graph);
    }

    return result;
}

}  // namespace graph_recognition

#endif
