# 日本語版ドキュメント作成依頼

## 概要

英語版ドキュメント (`docs/en/`) が完成済みです。
同じ構成で日本語版 (`docs/ja/`) を作成してください。

## 前提

- **Sphinx + Breathe + Doxygen** によるドキュメントシステム
- C++ ヘッダコメントは英語に統一済み → `.. doxygenfile::` で生成される API 部分は英語
- RST の prose 部分（見出し、説明文、アルゴリズム選択テーブル）を日本語で作成する
- 共通設定は `docs/conf_common.py` に集約済み

## 作業手順

### 1. ディレクトリ作成

```bash
mkdir -p docs/ja/families docs/ja/api
```

### 2. conf.py 作成

`docs/ja/conf.py` を以下の内容で作成：

```python
import sys, os
sys.path.insert(0, os.path.abspath('..'))
from conf_common import *  # noqa: F401,F403
language = 'ja'
```

### 3. RST ファイル作成

英語版 (`docs/en/`) の各ファイルを日本語に翻訳してください。
ファイル構成は英語版と完全に同じです：

| ファイル | 英語版の対応 | 内容 |
|---|---|---|
| `ja/index.rst` | `en/index.rst` | トップページ |
| `ja/getting_started.rst` | `en/getting_started.rst` | クイックスタート |
| `ja/usage.rst` | `en/usage.rst` | ビルド・I/O・テスト |
| `ja/python.rst` | `en/python.rst` | Python API |
| `ja/families/index.rst` | `en/families/index.rst` | ファミリー一覧 |
| `ja/families/chordal.rst` | `en/families/chordal.rst` | 弦グラフ系（11クラス） |
| `ja/families/interval.rst` | `en/families/interval.rst` | インターバル/円弧系（6クラス） |
| `ja/families/permutation.rst` | `en/families/permutation.rst` | 順列/比較可能性系（5クラス） |
| `ja/families/bipartite.rst` | `en/families/bipartite.rst` | 二部グラフ系（6クラス） |
| `ja/families/planar.rst` | `en/families/planar.rst` | 平面グラフ系（11クラス） |
| `ja/families/perfect.rst` | `en/families/perfect.rst` | 完全/構造系（13クラス） |
| `ja/families/forbidden_subgraph.rst` | `en/families/forbidden_subgraph.rst` | 禁止部分グラフ系（6クラス） |
| `ja/families/leaf_power.rst` | `en/families/leaf_power.rst` | Leaf Power 系（3クラス） |
| `ja/families/tree_forest.rst` | `en/families/tree_forest.rst` | 木/森系（4クラス） |
| `ja/families/connectivity.rst` | `en/families/connectivity.rst` | 連結性/正則性（8クラス） |
| `ja/families/directed.rst` | `en/families/directed.rst` | 有向グラフ系（3クラス） |
| `ja/api/index.rst` | `en/api/index.rst` | 基盤 API 一覧 |
| `ja/api/graph.rst` | `en/api/graph.rst` | グラフ表現 |
| `ja/api/utilities.rst` | `en/api/utilities.rst` | ユーティリティ |

### 4. 翻訳のポイント

#### 翻訳する部分
- RST の見出し（セクションタイトル）
- 説明文（各グラフクラスの定義・特徴の説明）
- アルゴリズム選択テーブルの「Description」列
- toctree の caption
- コード例の周辺テキスト

#### 翻訳しない部分
- `.. doxygenfile::` ディレクティブ（そのまま維持）
- コードブロック内のコード
- `enum class` の値名（`MCS_PEO`, `BUCKET_MCS_PEO` 等）
- 関数名・型名

#### 日本語の見出し対応表

| 英語 | 日本語 |
|---|---|
| Chordal Family | 弦グラフ系 |
| Interval / Arc Family | インターバル/円弧系 |
| Permutation / Comparability Family | 順列/比較可能性系 |
| Bipartite Family | 二部グラフ系 |
| Planar Family | 平面グラフ系 |
| Perfect / Structural Family | 完全/構造系 |
| Forbidden Subgraph Family | 禁止部分グラフ系 |
| Leaf Power Family | Leaf Power 系 |
| Tree / Forest Family | 木/森系 |
| Connectivity / Regularity Family | 連結性/正則性 |
| Directed Graph Family | 有向グラフ系 |
| Getting Started | クイックスタート |
| Usage Reference | 使い方 |
| Graph Families API Reference | グラフファミリー API リファレンス |
| Core API Reference | 基盤 API リファレンス |
| Chordal Graph | 弦グラフ |
| Strongly Chordal Graph | 強弦グラフ |
| Split Graph | スプリットグラフ |
| Threshold Graph | 閾値グラフ |
| Interval Graph | インターバルグラフ |
| Proper Interval Graph | 固有インターバルグラフ |
| Unit Interval Graph | 単位インターバルグラフ |
| Circular-Arc Graph | 円弧グラフ |
| Permutation Graph | 順列グラフ |
| Comparability Graph | 比較可能性グラフ |
| Bipartite Graph | 二部グラフ |
| Convex Bipartite Graph | 凸二部グラフ |
| Chain Graph | チェーングラフ |
| Planar Graph | 平面グラフ |
| Outerplanar Graph | 外平面グラフ |
| Cactus Graph | カクタスグラフ |
| Series-Parallel Graph | 直並列グラフ |
| Perfect Graph | 完全グラフ |
| Cograph | コグラフ |
| Distance-Hereditary Graph | 距離遺伝グラフ |
| Block Graph | ブロックグラフ |
| Ptolemaic Graph | プトレマイオスグラフ |
| Trivially Perfect Graph | 自明完全グラフ |
| Enumeration | 列挙 |

### 5. 既存の日本語ドキュメントの再利用

以下のファイルに既存の日本語説明文があります。これらの内容を再利用できます：

- `docs/api/recognition.rst` — 約50クラスの日本語説明文とアルゴリズムテーブル
- `docs/api/enumeration.rst` — 列挙アルゴリズムの日本語セクション見出し
- `docs/usage.rst` — ビルド・I/O・テストの日本語説明
- `docs/python.rst` — Python API の日本語説明
- `docs/index.rst` — 概要の日本語テキスト

**注意**: これらのファイルは旧構成のもので、新しく追加された 37+ クラスの説明は含まれていません。
未掲載クラスについては英語版を参考に新規作成してください。

### 6. ビルド確認

```bash
cd docs
doxygen Doxyfile
sphinx-build -b html ja _build/html/ja
```

エラーと WARNING（Duplicate 系以外）がゼロであることを確認してください。

## GitHub Actions の更新

日本語版が完成したら、`.github/workflows/docs.yml` を更新して
英語版・日本語版の両方をビルド・デプロイするようにしてください。

```yaml
- name: Build documentation
  working-directory: docs
  run: |
    doxygen Doxyfile
    sphinx-build -b html en _build/html/en
    sphinx-build -b html ja _build/html/ja
```

## 成果物の配置

```
_build/html/
  en/    → https://<user>.github.io/<repo>/en/
  ja/    → https://<user>.github.io/<repo>/ja/
```

トップレベルに言語選択ページ（`index.html`）を配置し、
英語版・日本語版へのリンクを提供してください。
