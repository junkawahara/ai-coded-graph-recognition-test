# graph-recognition

[Graph Recognition Library](https://github.com/junkawahara/ai-coded-graph-recognition-test) の Python バインディング — 81 のグラフクラスの認識アルゴリズムを提供する C++11 ヘッダオンリーライブラリ。

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
| アポロニアンネットワーク (Apollonian network) | `is_apollonian`, `recognize_apollonian` |
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
| 極大外平面 (Maximal outerplanar) | `is_maximal_outer_planar`, `recognize_maximal_outer_planar` |
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

C++ ライブラリの 81 認識器のうち 78 個がバインドされています
(partial_ktree、degenerate、cage のみ、追加パラメータがラッパーの統一
シグネチャに合わないため除外)。`is_digraph`,
`is_poset`, `is_tournament` は辺リストを**有向**の弧として解釈します
(`is_poset` の弧は被覆関係で、`(u, v)` は `v` が `u` を被覆することを
意味します)。

### 列挙関数

`enumerate_<type>_labeled_graphs(n)` は頂点数 `n` の当該クラスのラベル付き
グラフを全列挙します。`n` の上限は 6 (`ENUM_MAX_N`) で、それを超えると
`ValueError` を送出します (結果全体をメモリ上に構築するため。C++ の CLI は
ストリーミング出力なのでより大きい `n` に対応します):

`enumerate_at_free_labeled_graphs`,
`enumerate_biconvex_bipartite_labeled_graphs`,
`enumerate_bipartite_labeled_graphs`,
`enumerate_bipartite_permutation_labeled_graphs`,
`enumerate_block_labeled_graphs`, `enumerate_cactus_labeled_graphs`,
`enumerate_chordal_bipartite_labeled_graphs`,
`enumerate_chordal_labeled_graphs`, `enumerate_circular_arc_labeled_graphs`,
`enumerate_claw_free_labeled_graphs`, `enumerate_co_chordal_labeled_graphs`,
`enumerate_co_comparability_labeled_graphs`,
`enumerate_co_interval_labeled_graphs`, `enumerate_cograph_labeled_graphs`,
`enumerate_comparability_labeled_graphs`,
`enumerate_convex_bipartite_labeled_graphs`,
`enumerate_diamond_free_labeled_graphs`,
`enumerate_distance_hereditary_labeled_graphs`,
`enumerate_interval_labeled_graphs`, `enumerate_line_graph_labeled_graphs`,
`enumerate_outer_planar_labeled_graphs`, `enumerate_perfect_labeled_graphs`,
`enumerate_permutation_labeled_graphs`, `enumerate_planar_labeled_graphs`,
`enumerate_proper_interval_labeled_graphs`,
`enumerate_ptolemaic_labeled_graphs`,
`enumerate_series_parallel_labeled_graphs`, `enumerate_split_labeled_graphs`,
`enumerate_strongly_chordal_labeled_graphs`,
`enumerate_three_leaf_power_labeled_graphs`,
`enumerate_trapezoid_labeled_graphs`,
`enumerate_trivially_perfect_labeled_graphs`,
`enumerate_weakly_chordal_labeled_graphs`

`enumerate_<type>_unlabeled_graphs(n)` はラベル付けを全列挙する代わりに、
同型類ごとに代表元を 1 つだけ出力します。`apollonian`, `biconnected`, `chain`, `cochain`,
`maximal_outer_planar`, `maximal_planar`, `polyhedral`, `self_complementary`, `threshold` と補グラフ経由の
列挙器 (`co_chordal`, `co_comparability`, `co_interval`) 以外は
`connected_only=False` も受け取り、
連結な代表元のみに限定できます:

`enumerate_apollonian_unlabeled_graphs`,
`enumerate_biconnected_unlabeled_graphs`,
`enumerate_bipartite_permutation_unlabeled_graphs`,
`enumerate_bipartite_unlabeled_graphs`,
`enumerate_cactus_unlabeled_graphs`, `enumerate_chain_unlabeled_graphs`,
`enumerate_chordal_unlabeled_graphs`, `enumerate_circle_unlabeled_graphs`,
`enumerate_cluster_unlabeled_graphs`,
`enumerate_co_chordal_unlabeled_graphs`,
`enumerate_co_comparability_unlabeled_graphs`,
`enumerate_co_interval_unlabeled_graphs`,
`enumerate_cochain_unlabeled_graphs`,
`enumerate_cograph_unlabeled_graphs`,
`enumerate_comparability_unlabeled_graphs`,
`enumerate_cubic_planar_unlabeled_graphs`,
`enumerate_cubic_unlabeled_graphs`,
`enumerate_distance_hereditary_unlabeled_graphs`,
`enumerate_eulerian_unlabeled_graphs`,
`enumerate_interval_unlabeled_graphs`,
`enumerate_maximal_outer_planar_unlabeled_graphs`,
`enumerate_maximal_planar_unlabeled_graphs`,
`enumerate_outer_planar_unlabeled_graphs`,
`enumerate_permutation_unlabeled_graphs`, `enumerate_planar_unlabeled_graphs`,
`enumerate_polyhedral_unlabeled_graphs`,
`enumerate_proper_interval_unlabeled_graphs`,
`enumerate_ptolemaic_unlabeled_graphs`,
`enumerate_self_complementary_unlabeled_graphs`,
`enumerate_series_parallel_unlabeled_graphs`,
`enumerate_split_unlabeled_graphs`,
`enumerate_three_leaf_power_unlabeled_graphs`,
`enumerate_threshold_unlabeled_graphs`,
`enumerate_triangle_free_unlabeled_graphs`,
`enumerate_trivially_perfect_unlabeled_graphs`

### 部分グラフ列挙関数

`enumerate_<type>_subgraphs(n_or_graph, edges=None)` は頂点数ではなく
**ホストグラフ**を受け取り、そのグラフに含まれる当該クラスの部分グラフを
全列挙します。入力形式は認識関数と同じ `(n, edges)` または
`networkx.Graph` です:

`enumerate_chordal_subgraphs`

ここでの部分グラフは全域部分グラフ `(V, E')` (`E'` はホストの辺集合の
部分集合) です。頂点集合は固定され孤立頂点も保持されるため、結果はクラスに
属する辺部分集合と 1 対 1 に対応し、空の辺集合も常に含まれます。

上限は頂点数ではなく辺数にかかります。森の部分グラフはすべて弦グラフなので、
辺数 `m` のホストは最大 `2^m` 個の弦部分グラフを持ちうるためです。相異なる辺が
16 本 (`ENUM_MAX_M`) を超えるホストは `ValueError` を送出します。

```python
from graph_recognition import enumerate_chordal_subgraphs

# C4: 自身以外のすべての辺部分集合が弦グラフ
len(enumerate_chordal_subgraphs(4, [(1, 2), (2, 3), (3, 4), (4, 1)]))
# 15
```

### 誘導部分グラフ列挙関数

`enumerate_<type>_induced_subgraphs(n_or_graph, edges=None)` も
**ホストグラフ**を受け取りますが、解は `G[X]` が当該クラスに属する頂点部分集合
`X` なので、戻り値は辺リストではなく頂点リストのリストです:

`enumerate_chordal_bipartite_induced_subgraphs`

各頂点集合は昇順にソートされており、空集合が常に先頭に来ます。

上限は辺数ではなく頂点数にかかります。これらのクラスは遺伝的なので、ホスト自身が
クラスに属していれば、どれだけ疎であっても `2^n` 個の頂点部分集合がすべて解に
なるためです。頂点数が 16 (`INDUCED_ENUM_MAX_N`) を超えるホストは `ValueError`
を送出します。

```python
from graph_recognition import enumerate_chordal_bipartite_induced_subgraphs

# C6: 全体以外のすべての頂点部分集合が弦二部
len(enumerate_chordal_bipartite_induced_subgraphs(
    6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)]))
# 63
```

## API

### `is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool`

グラフが指定されたクラスに属するかを判定します。

- `n_or_graph`: 頂点数 (int) または `networkx.Graph`
- `edges`: `(u, v)` タプルのリスト (1-indexed)。`n_or_graph` が int の場合に必要。
- `algorithm`: アルゴリズム名 (str、オプション)。利用可能な選択肢は各関数の docstring を参照。

### `recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)`

`is_<type>` と同じですが、タプルを返します。2 番目の要素は将来の証拠 (certificate) 返却のために予約されています。

### 分解 (Decompositions)

`is_<type>` が「そのクラスに属するか」を答えるのに対し、これらはその答えの
背後にある構造そのものを返す。結果はすべて素の Python データ (int の dict /
list / tuple) であり、頂点添字のリストは 1-indexed 規約に従うため
`n + 1` 要素を持ち添字 0 は未使用。

```python
from graph_recognition import modular_decomposition, split_decomposition, spqr_tree

# P4 は非自明なモジュールを持たないので、分解木は prime ノード 1 つ
t = modular_decomposition(4, [(1, 2), (2, 3), (3, 4)])
t["nodes"][t["root"]]["kind"]                      # 'prime'

# 距離遺伝的グラフ ⟺ prime バッグを持たない
split_decomposition(4, [(1, 2), (2, 3), (3, 4)])["totally_decomposable"]   # True

# サイクルは多角形 1 つ
[node["kind"] for node in spqr_tree(4, [(1, 2), (2, 3), (3, 4), (4, 1)])["nodes"]]  # ['S']
```

| 関数 | 返すもの |
| --- | --- |
| `connected_components` / `co_components` | 各成分の頂点リスト |
| `twin_quotient(n, edges, kind='both')` | twin クラスと商グラフ |
| `block_cut_tree` | 二重連結成分・カット頂点・橋 |
| `modular_decomposition` | modular decomposition 木 |
| `cotree` | cograph の cotree (prime ノードを持たない木) |
| `transitive_orientation` | comparability グラフの推移的向き付け |
| `permutation_realizer` | 順列図 |
| `clique_tree` | 弦グラフの極大クリークとクリーク木 |
| `tree_decomposition` | 弦グラフの bag / 木 / treewidth |
| `split_decomposition` | Cunningham の正準 split decomposition |
| `spqr_tree` | 二重連結グラフの Tutte 3-連結成分 |
| `planar_embedding` | 回転系とその面 |
| `strong_elimination_ordering` | strong elimination ordering |
| `indifference_tree_layout` | indifference tree-layout |
| `consecutive_ones(num_columns, rows)` | 0/1 行列の連続 1 性判定 |

## ビルド

C++ 拡張はライブラリのヘッダを参照してコンパイルされます。リポジトリの
チェックアウトからは `../include` を使い、`python -m build --sdist` で
作成した sdist にはヘッダのコピーが同梱されるため、単体の sdist からも
ビルドできます。

## ライセンス

MIT
