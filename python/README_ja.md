# graph-recognition

[Graph Recognition Library](https://github.com/junkawahara/ai-coded-graph-recognition-test) の Python バインディング — 76 のグラフクラスの認識アルゴリズムを提供する C++11 ヘッダオンリーライブラリ。

[English version (README.md)](README.md)

## インストール

```bash
pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
```

C++11 対応のコンパイラ (g++ または clang++) が必要です。

### オプション: NetworkX サポート

```bash
pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
```

## クイックスタート

```python
from graph_recognition import is_interval, is_chordal, is_bipartite

# (n, edges) 形式 (1-indexed)
print(is_interval(4, [(1, 2), (2, 3), (3, 4)]))    # True
print(is_interval(4, [(1, 2), (2, 3), (3, 4), (4, 1)]))  # False (C4)

# アルゴリズム選択
print(is_chordal(4, [(1, 2), (2, 3), (3, 4)], algorithm="mcs_peo"))

# recognize (証拠付きタプルを返す。証拠は将来対応予定)
from graph_recognition import recognize_interval
result, cert = recognize_interval(4, [(1, 2), (2, 3), (3, 4)])
print(result)  # True
```

### NetworkX 連携

```python
import networkx as nx
from graph_recognition import is_interval, is_planar

G = nx.path_graph(5)
print(is_interval(G))  # True

# 任意のノード型をサポート
G = nx.Graph()
G.add_edges_from([("a", "b"), ("b", "c")])
print(is_interval(G))  # True
```

## 対応グラフクラス

| グラフクラス | 関数 |
|---|---|
| 頂点削除平面 (Apex) | `is_apex`, `recognize_apex` |
| AT-free | `is_at_free`, `recognize_at_free` |
| 2-連結 (Biconnected) | `is_biconnected`, `recognize_biconnected` |
| 双凸二部グラフ (Biconvex bipartite) | `is_biconvex_bipartite`, `recognize_biconvex_bipartite` |
| 二部グラフ (Bipartite) | `is_bipartite`, `recognize_bipartite` |
| 二部順列グラフ (Bipartite permutation) | `is_bipartite_permutation`, `recognize_bipartite_permutation` |
| ブロックグラフ (Block) | `is_block`, `recognize_block` |
| Bull-free | `is_bull_free`, `recognize_bull_free` |
| カクタスグラフ (Cactus) | `is_cactus`, `recognize_cactus` |
| 毛虫 (Caterpillar) | `is_caterpillar`, `recognize_caterpillar` |
| チェーングラフ (Chain) | `is_chain`, `recognize_chain` |
| 弦グラフ (Chordal) | `is_chordal`, `recognize_chordal` |
| 弦二部グラフ (Chordal bipartite) | `is_chordal_bipartite`, `recognize_chordal_bipartite` |
| サークルグラフ (Circle) | `is_circle`, `recognize_circle` |
| 円弧グラフ (Circular-arc) | `is_circular_arc`, `recognize_circular_arc` |
| クローフリーグラフ (Claw-free) | `is_claw_free`, `recognize_claw_free` |
| クラスタ (Cluster) | `is_cluster`, `recognize_cluster` |
| 余弦グラフ (Co-chordal) | `is_co_chordal`, `recognize_co_chordal` |
| 余比較可能性グラフ (Co-comparability) | `is_co_comparability`, `recognize_co_comparability` |
| 余インターバルグラフ (Co-interval) | `is_co_interval`, `recognize_co_interval` |
| 余チェーングラフ (Co-chain) | `is_cochain`, `recognize_cochain` |
| コグラフ (Cograph) | `is_cograph`, `recognize_cograph` |
| 比較可能性グラフ (Comparability) | `is_comparability`, `recognize_comparability` |
| 凸二部グラフ (Convex bipartite) | `is_convex_bipartite`, `recognize_convex_bipartite` |
| 3-正則 (Cubic) | `is_cubic`, `recognize_cubic` |
| 3-正則平面 (Cubic planar) | `is_cubic_planar`, `recognize_cubic_planar` |
| ダイヤモンドフリーグラフ (Diamond-free) | `is_diamond_free`, `recognize_diamond_free` |
| 単純有向グラフ (Digraph, 有向入力) | `is_digraph`, `recognize_digraph` |
| 距離遺伝グラフ (Distance-hereditary) | `is_distance_hereditary`, `recognize_distance_hereditary` |
| オイラー (Eulerian) | `is_eulerian`, `recognize_eulerian` |
| Even-hole-free | `is_even_hole_free`, `recognize_even_hole_free` |
| 5-leaf power | `is_five_leaf_power`, `recognize_five_leaf_power` |
| 森 (Forest) | `is_forest`, `recognize_forest` |
| 4-leaf power | `is_four_leaf_power`, `recognize_four_leaf_power` |
| フラーレン (Fullerene) | `is_fullerene`, `recognize_fullerene` |
| Gem-free | `is_gem_free`, `recognize_gem_free` |
| ハリン (Halin) | `is_halin`, `recognize_halin` |
| インターバルグラフ (Interval) | `is_interval`, `recognize_interval` |
| k-正則 (k-regular) | `is_kregular`, `recognize_kregular` |
| k-木 (k-tree) | `is_ktree`, `recognize_ktree` |
| ラマン (Laman) | `is_laman`, `recognize_laman` |
| ライングラフ (Line graph) | `is_line_graph`, `recognize_line_graph` |
| 極大平面 (Maximal planar) | `is_maximal_planar`, `recognize_maximal_planar` |
| メイニエル (Meyniel) | `is_meyniel`, `recognize_meyniel` |
| Odd-hole-free | `is_odd_hole_free`, `recognize_odd_hole_free` |
| 外平面グラフ (Outerplanar) | `is_outer_planar`, `recognize_outer_planar` |
| P5-free | `is_p5_free`, `recognize_p5_free` |
| パリティ (Parity) | `is_parity`, `recognize_parity` |
| パーフェクトグラフ (Perfect) | `is_perfect`, `recognize_perfect` |
| 順列グラフ (Permutation) | `is_permutation`, `recognize_permutation` |
| 平面グラフ (Planar) | `is_planar`, `recognize_planar` |
| 多面体 (Polyhedral) | `is_polyhedral`, `recognize_polyhedral` |
| 半順序ハッセ図 (Poset, 有向入力) | `is_poset`, `recognize_poset` |
| Proper chordal | `is_proper_chordal`, `recognize_proper_chordal` |
| 真円弧 (Proper circular-arc) | `is_proper_circular_arc`, `recognize_proper_circular_arc` |
| 固有インターバルグラフ (Proper interval) | `is_proper_interval`, `recognize_proper_interval` |
| プトレマイオスグラフ (Ptolemaic) | `is_ptolemaic`, `recognize_ptolemaic` |
| 準閾値グラフ (Quasi-threshold) | `is_quasi_threshold`, `recognize_quasi_threshold` |
| 自己補対 (Self-complementary) | `is_self_complementary`, `recognize_self_complementary` |
| 直並列グラフ (Series-parallel) | `is_series_parallel`, `recognize_series_parallel` |
| 単純四角形分割 (Simple quadrangulation) | `is_simple_quadrangulation`, `recognize_simple_quadrangulation` |
| スナーク (Snark) | `is_snark`, `recognize_snark` |
| スプリットグラフ (Split) | `is_split`, `recognize_split` |
| 強弦グラフ (Strongly chordal) | `is_strongly_chordal`, `recognize_strongly_chordal` |
| 強正則 (Strongly regular) | `is_strongly_regular`, `recognize_strongly_regular` |
| 3-リーフパワーグラフ (Three-leaf power) | `is_three_leaf_power`, `recognize_three_leaf_power` |
| 閾値グラフ (Threshold) | `is_threshold`, `recognize_threshold` |
| トーナメント (Tournament, 有向入力) | `is_tournament`, `recognize_tournament` |
| 台形グラフ (Trapezoid) | `is_trapezoid`, `recognize_trapezoid` |
| 木 (Tree) | `is_tree`, `recognize_tree` |
| 三角形なし (Triangle-free) | `is_triangle_free`, `recognize_triangle_free` |
| 3-連結 (Triconnected) | `is_triconnected`, `recognize_triconnected` |
| 自明完全グラフ (Trivially perfect) | `is_trivially_perfect`, `recognize_trivially_perfect` |
| 単閉路 (Unicyclic) | `is_unicyclic`, `recognize_unicyclic` |
| 単位インターバルグラフ (Unit interval) | `is_unit_interval`, `recognize_unit_interval` |
| 弱弦グラフ (Weakly chordal) | `is_weakly_chordal`, `recognize_weakly_chordal` |

C++ ライブラリの全 76 認識器がバインドされています。`is_digraph`,
`is_poset`, `is_tournament` は辺リストを**有向**の弧として解釈します
(`is_poset` の弧は被覆関係で、`(u, v)` は `v` が `u` を被覆することを
意味します)。

### 列挙関数

`enumerate_<type>_graphs(n)` は頂点数 `n` の当該クラスのラベル付きグラフを
全列挙します (`chain`, `cochain`, `threshold` は非同型列挙):

`enumerate_at_free_graphs`, `enumerate_biconvex_bipartite_graphs`,
`enumerate_bipartite_graphs`,
`enumerate_bipartite_permutation_graphs`, `enumerate_block_graphs`,
`enumerate_cactus_graphs`, `enumerate_chain_graphs`, `enumerate_chordal_graphs`,
`enumerate_chordal_bipartite_graphs`, `enumerate_circular_arc_graphs`,
`enumerate_claw_free_graphs`, `enumerate_co_chordal_graphs`,
`enumerate_co_comparability_graphs`, `enumerate_co_interval_graphs`,
`enumerate_cochain_graphs`,
`enumerate_cograph_graphs`, `enumerate_comparability_graphs`,
`enumerate_convex_bipartite_graphs`, `enumerate_diamond_free_graphs`,
`enumerate_distance_hereditary_graphs`, `enumerate_interval_graphs`,
`enumerate_line_graph_graphs`, `enumerate_outer_planar_graphs`,
`enumerate_perfect_graphs`, `enumerate_permutation_graphs`,
`enumerate_planar_graphs`,
`enumerate_proper_interval_graphs`, `enumerate_ptolemaic_graphs`,
`enumerate_series_parallel_graphs`, `enumerate_split_graphs`,
`enumerate_strongly_chordal_graphs`, `enumerate_three_leaf_power_graphs`,
`enumerate_threshold_graphs`, `enumerate_trapezoid_graphs`,
`enumerate_trivially_perfect_graphs`, `enumerate_weakly_chordal_graphs`

## API

### `is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool`

グラフが指定されたクラスに属するかを判定します。

- `n_or_graph`: 頂点数 (int) または `networkx.Graph`
- `edges`: `(u, v)` タプルのリスト (1-indexed)。`n_or_graph` が int の場合に必要。
- `algorithm`: アルゴリズム名 (str、オプション)。利用可能な選択肢は各関数の docstring を参照。

### `recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)`

`is_<type>` と同じですが、タプルを返します。2 番目の要素は将来の証拠 (certificate) 返却のために予約されています。

## ビルド

C++ 拡張はライブラリのヘッダを参照してコンパイルされます。リポジトリの
チェックアウトからは `../include` を使い、`python -m build --sdist` で
作成した sdist にはヘッダのコピーが同梱されるため、単体の sdist からも
ビルドできます。

## ライセンス

MIT
