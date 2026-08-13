# Graph Recognition Library

C++11 ヘッダオンリーライブラリ。グラフクラスの認識アルゴリズムを提供する。

## ビルド

```
make              # 全 CLI バイナリをビルド
make gtest_all    # gtest テストバイナリだけをビルド
make clean        # 生成物を削除
```

コンパイラ: C++11 対応の g++ (デフォルト)。`CXX`, `CXXFLAGS` で変更可能。gtest バイナリのみ C++17 が必要 (ライブラリ本体は C++11 互換を維持)。初回は `git submodule update --init --recursive` で `third_party/googletest` を取得する必要がある。

## プロジェクト構成

```
include/       ヘッダオンリーライブラリ (全アルゴリズム)
  graph.h        グラフ表現 (1-indexed, 隣接リスト+隣接セット)
  dsu.h          Union-Find
  mcs.h          Maximum Cardinality Search
  minor.h        グラフマイナーチェック
  chordal.h      弦グラフ認識
  clique.h       極大クリーク列挙 / クリーク木構築
  interval.h     インターバルグラフ認識
  permutation.h  順列グラフ認識
  ...            (その他 30+ ヘッダ)
src/           CLI エントリポイント
tests/         テストインフラ
  <type>/                       各グラフクラスのテストケース (.in / .exp)
  gtest/main.cpp                gtest エントリ
  gtest/helpers/                共通ヘルパー (test_helpers, certificates)
  gtest/recognizers/            認識テスト (<type>_test.cpp, 68 ファイル)
  gtest/enumerators/            列挙テスト (<type>_enum_test.cpp, 72 ファイル)
  gtest/property/               ランダム差分テスト (*Property, 既定 filter で除外)
  legacy/                       旧テストインフラ (check_*.py, run.sh, compare.py, fuzz.sh, compare_*.cpp)
third_party/googletest/  Google Test (git submodule)
docs/          Sphinx + Doxygen ドキュメント
```

## テスト

```
make test           # 既定フィルタ付き (876 テスト / ~280 秒)
make test-quick     # property テストだけ除外
make test-all       # 全テスト実行 (fullerene/cubic_planar の大 n は時間超過)
./gtest_all --gtest_filter='Interval*'   # 部分実行
```

`make test` の既定フィルタは Makefile 内の `TEST_DEFAULT_FILTER` で管理。既知の時間超過 (FullereneEnum 全ケース, CubicPlanarEnum/case6 (n=10)) と property テスト (`*Property*`) を除外している。

## 新しいグラフクラスの追加手順

1. `include/<type>.h` にヘッダを作成 (`<Camel>Result { bool is_<type>; ... }` と `check_<type>(const Graph&)` を提供)
2. `src/<type>_main.cpp` に CLI を作成 (任意; gtest だけあれば動作)
3. `Makefile` の `TARGETS` に `<type>` を追加
4. `tests/<type>/` にテストケース (.in / .exp) を配置
5. `tests/gtest/recognizers/<type>_test.cpp` を既存ファイルをコピーして作成
6. `make test` で検証

## 入出力形式

入力 (stdin):
```
n m
u1 v1
u2 v2
...
```
n: 頂点数, m: 辺数。頂点は 1-indexed。

出力 (stdout): `YES` / `NO` + グラフクラス固有の情報。

## アルゴリズム設計メモ

### Circular-Arc 認識 (circular_arc.h)
- **Circular-arc は disjoint union に対して閉じていない**: 非 interval な成分のアークが円全体をカバーするため、他成分を配置不可。Disconnected グラフは全成分が interval の場合のみ circular-arc。
- **円環クリーク順序**: 開始クリークの頂点はラップアラウンド（"must continue" 免除）が必要。貪欲法では tie-breaking 失敗あり → バックトラッキング必須。
- **ブルートフォース検証**: complement + C1P（全順列）で n≤8 まで検証可。`tests/legacy/check_circular_arc_brute.py`。

### Trapezoid 認識 (trapezoid.h)
- **Cogis/PS(P) 構成は G[K̄₂] と等価**: lexicographic product の性質により comparability が保存され、permutation と等価になってしまう。
- **Incidence poset Inc(P) 構成も不正**: dim(Inc(P)) ≠ idim(P) の場合がある。
- **正しいアプローチ**: B(P) 上の 2+2 パターン検出。B(P) の辺 (x,y) は NOT x<_P y。2 辺が非両立 ⟺ 4 元が distinct かつ x₁<_P y₂, x₂<_P y₁。**trivial 2K₂（4元未満）を除外することが必須**。
- **方向付け非依存性**: 4 distinct 元の 2K₂ は 2+2 に対応し、2+2 の存在は比較可能性のみに依存するため、推移的向き付けの選択に依存しない。

### Chordal Bipartite 認識 (chordal_bipartite.h)
- **DLO + Gamma-free は不正**: 木でも Gamma パターンが出現する。正しくは bisimplicial edge elimination（一辺ずつ）。
- **Bulk removal (N(y)×N(x)) も不正**: 完全二部部分グラフ内の非 bisimplicial 辺が誘導サイクルの一部になりうる。

## コーディング規約

- C++11 互換 (`std::iota` 不可、range-for の添字は `size_t`)
- ヘッダオンリー (`inline` 関数)
- 名前空間 `graph_recognition`
- 頂点は 1-indexed

## Git Workflow

- **Commit frequently**: Make small, incremental commits after completing each feature or fix
- Do not batch multiple unrelated changes into a single commit
- **Auto-commit after implementation**: When an implementation task is completed and tests pass, automatically commit the changes without waiting for user instruction
