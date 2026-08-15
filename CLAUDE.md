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
  ...            (その他 150 ヘッダ; include/ 全体で 158 ファイル)
src/           CLI エントリポイント (<type>_main.cpp, 149 ファイル)
tests/         テストインフラ
  <type>/                       各グラフクラスのテストケース (.in / .exp, 141 ディレクトリ)
  gtest/main.cpp                gtest エントリ
  gtest/helpers/                共通ヘルパー (test_helpers, certificates)
  gtest/recognizers/            認識テスト (<type>_test.cpp, 68 ファイル)
  gtest/enumerators/            列挙テスト (<type>_enum_test.cpp, 72 ファイル)
  gtest/property/               ランダム差分テスト (*_property_test.cpp, 39 ファイル; *Property, 既定 filter で除外)
  legacy/                       旧テストインフラ (check_*.py, run.sh, compare.py, fuzz.sh, compare_*.cpp)
third_party/googletest/  Google Test (git submodule)
docs/          Sphinx + Doxygen ドキュメント
```

## テスト

```
make test           # 既定フィルタ付き (888 テスト / 約 23 秒; ビルド済みの場合)
make test-quick     # property テストだけ除外
make test-all       # 全テスト実行 (fullerene/cubic_planar/circular_arc の大 n は時間超過)
./gtest_all --gtest_filter='Interval*'   # 部分実行
```

`make test` の既定フィルタは Makefile 内の `TEST_DEFAULT_FILTER` で管理。除外しているのは以下の 4 項目のみ (Makefile 側のコメントと同期させること):

| フィルタパターン | 除外理由 |
| --- | --- |
| `*FullereneEnum*` | 列挙器の n>=20 が数時間かかる (全ケース除外) |
| `*CubicPlanarEnum*case6` | n=10 (5826240 グラフ) で約 280 秒かかる |
| `*/CircularArcEnumTest.*case6` | n=6 (28081 グラフ) の逆探索列挙が単独で 約 250 秒 (並列負荷時 約 520 秒)。残り全部で約 5 秒なので、このケースだけで実行時間を支配していた。先頭の `/` により `ProperCircularArcEnumTest` は除外されない |
| `*Property*` | ランダム差分テスト (`make test-quick` / `make test-all` で実行) |

上記フィルタ下での実測値: 892 テスト / 141 テストスイート、全て PASS。gtest 実行時間はアイドル時 約 5 秒。ヘッダ変更後の初回は `make test` にフルリビルドの +50 秒程度が加わる。かつて全体の 8 割以上を占めていた `CircleEnumTest/case6` (n=6, 32636 グラフ, 単独 20 秒) は、circle 認識の Naji 化により約 0.2 秒に短縮された。現在の最遅ケースは `HalinEnumTest/case10` (約 0.7 秒)。

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

### Circle 認識 (circle.h)
- **既定は Naji の線形システム** (多項式時間、判定のみ): G が circle ⟺ 順序対ごとの変数 β(u,v) ∈ GF(2) に対する連立方程式 NS1 (辺 vw: β(v,w)+β(w,v)=1)、NS2 (辺 vw と両方に非隣接な x: β(x,v)+β(x,w)=0)、NS3 (非辺 {v,w} と共通近傍 x: β(v,w)+β(w,v)+β(x,v)+β(x,w)=1) が可解 (Naji 1985 / Gasse 1997 / Geelen–Lee 2020, arXiv:1807.10988)。
- **実装上の縮約**: NS1 は辺ごとに 1 変数へ代入消去。NS2 は「β(x,·) が G−N[x] の連結成分上で定数」と等価なので、(x, 成分) ごとの 1 変数に商を取って消去。残る NS3 (各 4 変数) だけを RREF 維持の逐次ビットセットガウス消去へ。基底を RREF に保つと 1 本の追加は係数 4 個分の XOR パスで済む。
- **実測**: ランダム G(n,1/2) n=200 で 0.13 秒 (NO)、ランダム弦図由来の circle graph n=300 (m≈16000) で 1.2 秒 (YES)。旧 DOW バックトラッキングは n=10 の NO でタイムアウトしていた。
- **DOW_BACKTRACKING は証明書用に残置** (YES 時に DOW を返す唯一の手段; NO の証明が指数時間で実用上限 n≈9)。列挙器のフィルタが Naji になったことで `CircleEnumTest/case6` は 20 秒 → 0.2 秒。

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
