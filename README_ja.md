# ai-coded-graph-recognition-test

AI コーディングの限界を探る実験: C++11 ヘッダオンリーのグラフクラス認識ライブラリ。**コードはすべて AI** (Claude 4.6 Opus および Codex 5.2) が生成しました。人間の役割はタスク指示とコードレビューのみで、コードは一切手書きしていません。

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
- **30 以上のグラフクラス** を複数のアルゴリズムバリアントで認識
- **CLI ツール**: 各認識器にコマンドラインインターフェースを提供 (グラフを読み込み YES/NO を出力)
- **テストインフラ**: 静的テストケース、Python 全探索チェッカー、ファズテスト、アルゴリズム間の差分テスト

## 対応グラフクラス

### 弦グラフ系

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| 弦グラフ (Chordal) | `chordal.h` | 長さ 4 以上の誘導閉路を持たない |
| 強弦グラフ (Strongly chordal) | `strongly_chordal.h` | 弦グラフ + 長さ 6 以上の偶閉路が奇弦を持つ |
| スプリットグラフ (Split) | `split.h` | 頂点集合をクリークと独立集合に分割可能 |
| 閾値グラフ (Threshold) | `threshold.h` | 孤立頂点または全域頂点の反復除去で空にできる |
| 弱弦グラフ (Weakly chordal) | `weakly_chordal.h` | G と補グラフのいずれにも長さ 5 以上の誘導閉路がない |
| ブロックグラフ (Block) | `block.h` | 全ての二重連結成分がクリーク |
| プトレマイオスグラフ (Ptolemaic) | `ptolemaic.h` | 弦グラフ + 距離遺伝グラフ |
| 自明完全グラフ (Trivially perfect) | `trivially_perfect.h` | 弦グラフ + コグラフ (= 準閾値グラフ) |
| 準閾値グラフ (Quasi-threshold) | `quasi_threshold.h` | 自明完全グラフの別名 |

### インターバルグラフ系

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| インターバルグラフ (Interval) | `interval.h` | 実数直線上の区間の交差グラフ |
| 固有インターバルグラフ (Proper interval) | `proper_interval.h` | 区間間に包含関係がないインターバルグラフ |
| 単位インターバルグラフ (Unit interval) | `unit_interval.h` | 等長区間のインターバルグラフ (= 固有インターバルグラフ) |
| 余インターバルグラフ (Co-interval) | `co_interval.h` | 補グラフがインターバルグラフ |
| 円弧グラフ (Circular-arc) | `circular_arc.h` | 円周上の弧の交差グラフ |

### 順列グラフ系

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| 順列グラフ (Permutation) | `permutation.h` | G と補グラフの両方が比較可能性グラフ |
| 比較可能性グラフ (Comparability) | `comparability.h` | 辺に推移的向き付けが可能 |
| 余比較可能性グラフ (Co-comparability) | `co_comparability.h` | 補グラフが比較可能性グラフ |
| 二部順列グラフ (Bipartite permutation) | `bipartite_permutation.h` | 二部グラフ + 順列グラフ |

### 二部グラフ系

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| 二部グラフ (Bipartite) | `bipartite.h` | 2 彩色可能 (奇閉路なし) |
| 弦二部グラフ (Chordal bipartite) | `chordal_bipartite.h` | 二部グラフ + 長さ 6 以上の誘導閉路なし |
| チェーングラフ (Chain) | `chain.h` | 二部グラフ + 近傍が包含順序で全順序 |
| 余チェーングラフ (Co-chain) | `cochain.h` | 補グラフがチェーングラフ |

### 平面グラフ系

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| 平面グラフ (Planar) | `planar.h` | K₅ および K₃,₃ マイナーを持たない |
| 外平面グラフ (Outerplanar) | `outer_planar.h` | K₄ および K₂,₃ マイナーを持たない |
| カクタスグラフ (Cactus) | `cactus.h` | 各二重連結成分が辺 1 本または単純閉路 |
| 直並列グラフ (Series-parallel) | `series_parallel.h` | K₄ マイナーを持たない (2-退化) |

### その他

| グラフクラス | ヘッダ | 説明 |
|---|---|---|
| コグラフ (Cograph) | `cograph.h` | 誘導部分グラフとして P₄ を含まない |
| 距離遺伝グラフ (Distance-hereditary) | `distance_hereditary.h` | 全連結誘導部分グラフで頂点間距離が保存される |
| AT-free グラフ | `at_free.h` | 小惑星三つ組 (asteroidal triple) を持たない |
| 余弦グラフ (Co-chordal) | `co_chordal.h` | 補グラフが弦グラフ |

## ビルド

```bash
make          # 全認識器をビルド
make clean    # バイナリを削除
```

C++11 対応の g++ が必要です。`CXX`, `CXXFLAGS` でカスタマイズ可能。

## 使い方

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

## テスト

```bash
# 特定のグラフクラスの静的テストを実行
bash tests/run.sh interval

# 差分テスト: 2 つのバイナリをランダムグラフで比較
python3 tests/compare.py ./interval ./interval_v2 1000

# ファズテスト
bash tests/fuzz.sh ./binary1 ./binary2 500
```

Python 全探索チェッカー (`tests/check_*.py`) は小さいグラフ (n ≤ 9) に対して網羅的に結果を検証します。

## プロジェクト構成

```
include/          ヘッダオンリーライブラリ (全アルゴリズム)
  graph.h           グラフ表現 (1-indexed, 隣接リスト + 隣接セット)
  chordal.h         弦グラフ認識
  interval.h        インターバルグラフ認識
  ...               (30 以上のヘッダ)
src/              CLI エントリポイント
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
