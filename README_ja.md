# ai-coded-graph-recognition-test

**70 超のグラフクラスに対する認識・列挙アルゴリズム**を提供する C++11 ヘッダオンリーライブラリです。弦グラフ、インターバルグラフ、平面グラフ、順列グラフなど多数のクラスに対応しています。認識器はグラフが特定のクラスに属するかを判定し (証明書付き)、列挙器は n 頂点上のラベル付きグラフを全列挙します。

本ライブラリの**コードはすべて AI** (Claude 4.6 Opus および Codex 5.2) が生成しました。AI コーディングの限界を探る実験として開発されており、人間の役割はタスク指示とコードレビューのみで、コードは一切手書きしていません。

[English version (README.md)](README.md)

## 動機

このプロジェクトは、AI が非自明なアルゴリズムをゼロから実装できる限界を調査するものです。グラフクラス認識を題材に選んだ理由は、以下の能力が要求されるためです:

- 構造的グラフ理論の形式的定義・定理の理解
- 既知アルゴリズム (MCS, PEO, BFS, DFS, バックトラッキング等) の正確な実装
- 微妙なエッジケース (空グラフ、非連結グラフ、補グラフの閉包性) の処理
- 正しさを検証するためのテストインフラの設計・実行

実験は進行中です。知見がまとまり次第ドキュメント化します。

## ドキュメント

API ドキュメント: **https://junkawahara.github.io/ai-coded-graph-recognition-test/**

## 特徴

- **ヘッダオンリー**: `#include` するだけで使用可能。リンク不要
- **C++11 互換**: 標準的なコンパイラで動作
- **70 超のグラフクラス** に対して認識・列挙またはその両方を提供
- **56 種の認識器**: 複数のアルゴリズムバリアント (YES/NO + 証明書)
- **73 種の列挙器**: 指定された頂点数 n のラベル付きグラフを全列挙
- **CLI ツール**: 全認識器・列挙器にコマンドラインインターフェースを提供
- **テストインフラ**: 静的テストケース、Python 全探索チェッカー、ファズテスト、アルゴリズム間の差分テスト
- **Python バインディング**: pybind11 による NetworkX 連携対応

## 対応グラフクラス

### 弦グラフ系

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 弦グラフ (Chordal) | `chordal.h` | Yes | 長さ 4 以上の誘導閉路を持たない |
| 強弦グラフ (Strongly chordal) | `strongly_chordal.h` | Yes | 弦グラフ + 長さ 6 以上の偶閉路が奇弦を持つ |
| 固有弦グラフ (Proper chordal) | `proper_chordal.h` | Yes | 弦グラフ + indifference tree-layout を許容 |
| スプリットグラフ (Split) | `split.h` | Yes | 頂点集合をクリークと独立集合に分割可能 |
| 閾値グラフ (Threshold) | `threshold.h` | Yes | 孤立頂点または全域頂点の反復除去で空にできる |
| 弱弦グラフ (Weakly chordal) | `weakly_chordal.h` | Yes | G と補グラフのいずれにも長さ 5 以上の誘導閉路がない |
| ブロックグラフ (Block) | `block.h` | Yes | 全ての二重連結成分がクリーク |
| プトレマイオスグラフ (Ptolemaic) | `ptolemaic.h` | Yes | 弦グラフ + 距離遺伝グラフ |
| 自明完全グラフ (Trivially perfect) | `trivially_perfect.h` | Yes | 弦グラフ + コグラフ (= 準閾値グラフ) |

### インターバルグラフ / 円弧グラフ系

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| インターバルグラフ (Interval) | `interval.h` | Yes | 実数直線上の区間の交差グラフ |
| 固有インターバルグラフ (Proper interval) | `proper_interval.h` | Yes | 区間間に包含関係がないインターバルグラフ |
| 単位インターバルグラフ (Unit interval) | `unit_interval.h` | — | 等長区間のインターバルグラフ (= 固有インターバルグラフ) |
| 余インターバルグラフ (Co-interval) | `co_interval.h` | Yes | 補グラフがインターバルグラフ |
| 円弧グラフ (Circular-arc) | `circular_arc.h` | Yes | 円周上の弧の交差グラフ |
| 固有円弧グラフ (Proper circular-arc) | `proper_circular_arc.h` | Yes | 弧間に包含関係がない円弧グラフ |
| 台形グラフ (Trapezoid) | `trapezoid.h` | Yes | 二平行線間の台形の交差グラフ |

### 順列グラフ / 比較可能性グラフ系

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 順列グラフ (Permutation) | `permutation.h` | Yes | G と補グラフの両方が比較可能性グラフ |
| 比較可能性グラフ (Comparability) | `comparability.h` | Yes | 辺に推移的向き付けが可能 |
| 余比較可能性グラフ (Co-comparability) | `co_comparability.h` | Yes | 補グラフが比較可能性グラフ |
| 二部順列グラフ (Bipartite permutation) | `bipartite_permutation.h` | Yes | 二部グラフ + 順列グラフ |

### 二部グラフ系

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 二部グラフ (Bipartite) | `bipartite.h` | Yes | 2 彩色可能 (奇閉路なし) |
| 弦二部グラフ (Chordal bipartite) | `chordal_bipartite.h` | Yes | 二部グラフ + 長さ 6 以上の誘導閉路なし |
| チェーングラフ (Chain) | `chain.h` | Yes | 二部グラフ + 近傍が包含順序で全順序 |
| 余チェーングラフ (Co-chain) | `cochain.h` | Yes | 補グラフがチェーングラフ |
| 凸二部グラフ (Convex bipartite) | `convex_bipartite.h` | Yes | 二部グラフ + 片側が連続近傍性質を持つ |
| 双凸二部グラフ (Biconvex bipartite) | `biconvex_bipartite.h` | Yes | 二部グラフ + 両側が連続近傍性質を持つ |

### 平面グラフ系

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 平面グラフ (Planar) | `planar.h` | Yes | K5 および K3,3 マイナーを持たない |
| 外平面グラフ (Outerplanar) | `outer_planar.h` | Yes | K4 および K2,3 マイナーを持たない |
| カクタスグラフ (Cactus) | `cactus.h` | Yes | 各二重連結成分が辺 1 本または単純閉路 |
| 直並列グラフ (Series-parallel) | `series_parallel.h` | Yes | K4 マイナーを持たない (2-退化) |
| 頂点グラフ (Apex) | `apex.h` | Yes | 1 頂点の除去で平面グラフになる |

### 完全グラフ / 構造クラス

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 完全グラフ (Perfect) | `perfect.h` | Yes | 奇穴も奇反穴も持たない (SPGT) |
| コグラフ (Cograph) | `cograph.h` | Yes | 誘導部分グラフとして P4 を含まない |
| 距離遺伝グラフ (Distance-hereditary) | `distance_hereditary.h` | Yes | 全連結誘導部分グラフで頂点間距離が保存される |
| AT-free グラフ | `at_free.h` | Yes | 小惑星三つ組 (asteroidal triple) を持たない |
| 余弦グラフ (Co-chordal) | `co_chordal.h` | Yes | 補グラフが弦グラフ |
| 線グラフ (Line graph) | `line_graph.h` | Yes | 別のグラフの辺交差グラフ |
| 円グラフ (Circle) | `circle.h` | Yes | 円の弦の交差グラフ |
| Meyniel グラフ | `meyniel.h` | Yes | 長さ 5 以上の奇閉路が少なくとも 2 本の弦を持つ |
| パリティグラフ (Parity) | `parity.h` | Yes | 同じ端点間の誘導パスが全て同じパリティ |
| 偶穴フリー (Even-hole-free) | `even_hole_free.h` | Yes | 長さ 4 以上の偶数誘導閉路を持たない |
| 奇穴フリー (Odd-hole-free) | `odd_hole_free.h` | Yes | 長さ 5 以上の奇数誘導閉路を持たない |
| クラスターグラフ (Cluster) | `cluster.h` | Yes | 完全グラフの非連結和 |

### 禁止誘導部分グラフクラス

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| クローフリー (Claw-free) | `claw_free.h` | Yes | 誘導 K1,3 を含まない |
| ダイヤモンドフリー (Diamond-free) | `diamond_free.h` | Yes | 誘導 K4 - e を含まない |
| 三角形フリー (Triangle-free) | `triangle_free.h` | Yes | K3 を含まない |
| ブルフリー (Bull-free) | `bull_free.h` | Yes | 誘導ブルグラフを含まない |
| P5-free | `p5_free.h` | Yes | 誘導 P5 (長さ 5 の道) を含まない |
| ジェムフリー (Gem-free) | `gem_free.h` | Yes | 誘導ジェム (fan) グラフを含まない |

### 葉べき乗グラフ (Leaf Power)

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 3-leaf power | `three_leaf_power.h` | Yes | 距離閾値 3 の葉べき乗グラフ |
| 4-leaf power | `four_leaf_power.h` | Yes | 距離閾値 4 の葉べき乗グラフ |
| 5-leaf power | `five_leaf_power.h` | Yes | 距離閾値 5 の葉べき乗グラフ |

### 連結性 / 正則性

| グラフクラス | ヘッダ | 列挙 | 説明 |
|---|---|---|---|
| 二重連結 (Biconnected) | `biconnected.h` | Yes | 2-連結 (カット頂点なし) |
| 三重連結 (Triconnected) | `triconnected.h` | — | 3-連結 |
| オイラーグラフ (Eulerian) | `eulerian.h` | Yes | 全頂点の次数が偶数 |

### 列挙専用クラス

以下のクラスは列挙器のみ提供しており、単体の認識器はありません。

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| 木 (Tree) | `tree_enum.h` | 非同型な自由木 |
| 森 (Forest) | `forest_enum.h` | 閉路を持たないグラフ |
| 毛虫グラフ (Caterpillar) | `caterpillar_enum.h` | 全頂点がパスから距離 1 以内の木 |
| 一閉路グラフ (Unicyclic) | `unicyclic_enum.h` | ちょうど 1 つの閉路を持つ連結グラフ |
| k-木 (k-tree) | `ktree_enum.h` | 木幅がちょうど k の弦グラフ |
| k-正則 (k-regular) | `kregular_enum.h` | 全頂点の次数が k |
| 三正則 (Cubic) | `cubic_enum.h` | 3-正則グラフ |
| 三正則平面 (Cubic planar) | `cubic_planar_enum.h` | 3-正則平面グラフ |
| 極大平面 (Maximal planar) | `maximal_planar_enum.h` | 全面が三角形の平面グラフ |
| 多面体 (Polyhedral) | `polyhedral_enum.h` | 3-連結平面グラフ |
| ハリングラフ (Halin) | `halin_enum.h` | 木 + 外周閉路からなる平面グラフ |
| フラーレン (Fullerene) | `fullerene_enum.h` | 五角形と六角形の面を持つ 3-正則平面グラフ |
| 単純四角分割 (Simple quadrangulation) | `simple_quadrangulation_enum.h` | 全面が四角形の 3-連結平面グラフ |
| スナーク (Snark) | `snark_enum.h` | 巡回 4-辺連結な三正則グラフで彩色指数 4 |
| 自己補グラフ (Self-complementary) | `self_complementary_enum.h` | 補グラフと同型 |
| 強正則 (Strongly regular) | `strongly_regular_enum.h` | 正則で隣接数が一様 |
| トーナメント (Tournament) | `tournament_enum.h` | 完全有向グラフ (Kn の向き付け) |
| 有向グラフ (Directed graph) | `digraph_enum.h` | 全有向グラフ |
| Laman グラフ | `laman_enum.h` | 2D で最小剛性を持つグラフ |
| 半順序集合 (Poset) | `poset_enum.h` | ハッセ図 (半順序集合) |

## ビルド

```bash
make          # 全認識器・列挙器をビルド
make clean    # バイナリを削除
```

C++11 対応の g++ が必要です。`CXX`, `CXXFLAGS` でカスタマイズ可能。

## 使い方

### 認識

各認識器は標準入力からグラフを読み込み、結果を出力します:

```bash
echo "4 4
1 2
2 3
3 4
4 1" | ./chordal
# 出力: NO
```

**入力形式:**
```
n m
u1 v1
u2 v2
...
```
`n` = 頂点数, `m` = 辺数。頂点は 1-indexed。

**出力形式:** `YES` または `NO`。グラフクラス固有の付加情報が続く場合があります (例: インターバルモデル)。

### 列挙

各列挙器は指定された頂点数 n のラベル付きグラフを全列挙します:

```bash
echo "4" | ./chordal_enum
# 出力: 1行目にグラフ数、続いて辺リスト
```

### ライブラリとしての使用

```cpp
#include "interval.h"

graph_recognition::Graph g(4, {{1,2}, {2,3}, {3,4}, {4,1}});
auto result = graph_recognition::check_interval(g);
if (result.is_interval) {
    // result.intervals[v] = {L, R}: 頂点 v の区間
}
```

多くの認識器はアルゴリズム選択をサポートしています:

```cpp
// デフォルトの AT-free ではなくバックトラッキングを使用
auto result = graph_recognition::check_interval(g,
    graph_recognition::IntervalAlgorithm::BACKTRACKING);
```

### Python ラッパー

pybind11 バインディングによる Python パッケージを提供しています:

```bash
pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

# NetworkX サポート付き
pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
```

```python
from graph_recognition import is_interval, is_chordal

# 辺リスト (1-indexed)
is_interval(4, [(1, 2), (2, 3), (3, 4)])  # True
is_chordal(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

# アルゴリズム選択
is_interval(4, [(1, 2), (2, 3), (3, 4)], algorithm="backtracking")

# NetworkX 連携 (任意のノード型をサポート)
import networkx as nx
is_interval(nx.path_graph(5))  # True
```

38 グラフクラスが `is_<type>()` および `recognize_<type>()` 関数として利用可能です。詳細は [python/README.md](python/README.md) を参照してください。

## テスト

```bash
# 特定のグラフクラスの静的テストを実行
bash tests/run.sh interval

# 差分テスト: 2 つのバイナリをランダムグラフで比較
python3 tests/compare.py ./interval ./interval_v2 1000

# ファズテスト
bash tests/fuzz.sh ./binary1 ./binary2 500
```

Python チェッカー (`tests/check_*.py`) は認識器の出力を期待結果と照合して検証します。一部のチェッカー (例: `check_interval.py`) は証明書 (インターバルモデル等) も検証します。

## プロジェクト構成

```
include/          ヘッダオンリーライブラリ (全アルゴリズム)
  graph.h           グラフ表現 (1-indexed, 隣接リスト + 隣接セット)
  chordal.h         弦グラフ認識
  interval.h        インターバルグラフ認識
  ...               (130 以上のヘッダ)
src/              CLI エントリポイント
python/           Python ラッパー (pybind11)
  src/              パッケージソース (graph_recognition)
  tests/            pytest テストスイート
tests/            テストインフラ
  <type>/           静的テストケース (.in / .exp)
  check_<type>.py   全探索チェッカー
  run.sh            テストランナー
  compare.py        差分テスト
  fuzz.sh           ファズテスト
docs/             Sphinx + Doxygen ドキュメント
```

## 使用した AI モデル

- **Claude 4.6 Opus** (Anthropic) — Claude Code CLI 経由
- **Codex 5.2** (OpenAI)

全コードはこれらのモデルが生成しました。人間はタスクの指示、出力のレビュー、バグ修正の指示を行いましたが、コードは一行も書いていません。

## ライセンス

[MIT](LICENSE)
