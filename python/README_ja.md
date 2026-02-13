# graph-recognition

[Graph Recognition Library](https://github.com/junkawahara/ai-coded-graph-recognition-test) の Python バインディング — 30 以上のグラフクラスの認識アルゴリズムを提供する C++11 ヘッダオンリーライブラリ。

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
| AT-free | `is_at_free`, `recognize_at_free` |
| 二部グラフ (Bipartite) | `is_bipartite`, `recognize_bipartite` |
| 二部順列グラフ (Bipartite permutation) | `is_bipartite_permutation`, `recognize_bipartite_permutation` |
| ブロックグラフ (Block) | `is_block`, `recognize_block` |
| カクタスグラフ (Cactus) | `is_cactus`, `recognize_cactus` |
| チェーングラフ (Chain) | `is_chain`, `recognize_chain` |
| 弦グラフ (Chordal) | `is_chordal`, `recognize_chordal` |
| 弦二部グラフ (Chordal bipartite) | `is_chordal_bipartite`, `recognize_chordal_bipartite` |
| 円弧グラフ (Circular-arc) | `is_circular_arc`, `recognize_circular_arc` |
| 余弦グラフ (Co-chordal) | `is_co_chordal`, `recognize_co_chordal` |
| 余比較可能性グラフ (Co-comparability) | `is_co_comparability`, `recognize_co_comparability` |
| 余インターバルグラフ (Co-interval) | `is_co_interval`, `recognize_co_interval` |
| 余チェーングラフ (Co-chain) | `is_cochain`, `recognize_cochain` |
| コグラフ (Cograph) | `is_cograph`, `recognize_cograph` |
| 比較可能性グラフ (Comparability) | `is_comparability`, `recognize_comparability` |
| 距離遺伝グラフ (Distance-hereditary) | `is_distance_hereditary`, `recognize_distance_hereditary` |
| インターバルグラフ (Interval) | `is_interval`, `recognize_interval` |
| 外平面グラフ (Outerplanar) | `is_outer_planar`, `recognize_outer_planar` |
| 順列グラフ (Permutation) | `is_permutation`, `recognize_permutation` |
| 平面グラフ (Planar) | `is_planar`, `recognize_planar` |
| 固有インターバルグラフ (Proper interval) | `is_proper_interval`, `recognize_proper_interval` |
| プトレマイオスグラフ (Ptolemaic) | `is_ptolemaic`, `recognize_ptolemaic` |
| 準閾値グラフ (Quasi-threshold) | `is_quasi_threshold`, `recognize_quasi_threshold` |
| 直並列グラフ (Series-parallel) | `is_series_parallel`, `recognize_series_parallel` |
| スプリットグラフ (Split) | `is_split`, `recognize_split` |
| 強弦グラフ (Strongly chordal) | `is_strongly_chordal`, `recognize_strongly_chordal` |
| 閾値グラフ (Threshold) | `is_threshold`, `recognize_threshold` |
| 自明完全グラフ (Trivially perfect) | `is_trivially_perfect`, `recognize_trivially_perfect` |
| 単位インターバルグラフ (Unit interval) | `is_unit_interval`, `recognize_unit_interval` |
| 弱弦グラフ (Weakly chordal) | `is_weakly_chordal`, `recognize_weakly_chordal` |

その他: `enumerate_chordal_graphs(n)` — 頂点数 n のラベル付き弦グラフの全列挙。

## API

### `is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool`

グラフが指定されたクラスに属するかを判定します。

- `n_or_graph`: 頂点数 (int) または `networkx.Graph`
- `edges`: `(u, v)` タプルのリスト (1-indexed)。`n_or_graph` が int の場合に必要。
- `algorithm`: アルゴリズム名 (str、オプション)。利用可能な選択肢は各関数の docstring を参照。

### `recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)`

`is_<type>` と同じですが、タプルを返します。2 番目の要素は将来の証拠 (certificate) 返却のために予約されています。

## ライセンス

MIT
