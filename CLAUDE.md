# Graph Recognition Library

C++11 ヘッダオンリーライブラリ。グラフクラスの認識アルゴリズムを提供する。

## ビルド

```
make              # 全 CLI バイナリを bin/ 以下にビルド
make <type>       # 単体ビルド (bin/<type> を生成; 例: make interval)
make gtest_all    # gtest テストバイナリだけをビルド
make clean        # 生成物を削除 (bin/ ごと)
```

CLI ターゲットは `src/*_main.cpp` から自動導出される (`Makefile` にターゲット一覧の手書きは無い)。

コンパイラ: C++11 対応の g++ (デフォルト)。`CXX`, `CXXFLAGS` で変更可能。gtest バイナリのみ C++17 が必要 (ライブラリ本体は C++11 互換を維持)。初回は `git submodule update --init --recursive` で `third_party/googletest` を取得する必要がある。

## プロジェクト構成

```
include/       ヘッダオンリーライブラリ (全アルゴリズム)
  graph.h        グラフ表現 (1-indexed, 隣接リスト+隣接セット)
  <type>.h       各グラフクラスの認識器 (クラスごとに 1 ファイル)。列挙器は <type>_enum.h
  ほか           基本ユーティリティ / 分解構造 / NO 証明書の共有語彙・抽出器
                 (forbidden_subgraph.h / obstruction_extract.h) 等。include/ 全体で 172 ファイル
src/           CLI エントリポイント (<type>_main.cpp, 149 ファイル)
tests/         テストインフラ
  <type>/                       各グラフクラスのテストケース (.in / .exp, 149 ディレクトリ)
  gtest/main.cpp                gtest エントリ
  gtest/helpers/                共通ヘルパー (test_helpers, certificates, bf_oracles)
  gtest/recognizers/            認識テスト (<type>_test.cpp, 76 ファイル)
  gtest/enumerators/            列挙テスト (<type>_enum_test.cpp, 73 ファイル)
  gtest/property/               ランダム差分テスト (*_property_test.cpp, 51 ファイル; *Property, 既定 filter で除外)
third_party/googletest/  Google Test (git submodule)
docs/          Sphinx + Doxygen ドキュメント
```

## テスト

```
make test           # 既定フィルタ付き (ビルド済みなら数十秒)
make test-quick     # 既定フィルタ + property テストも実行 (超低速の列挙ケースだけ除外)
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

旧 Python/Bash テストインフラ (`tests/legacy/`) は削除済み。必要なら git タグ `legacy-tests` から取り出せる。

## 新しいグラフクラスの追加手順

1. `include/<type>.h` にヘッダを作成 (`<Camel>Result { bool is_<type>; ... }` と `check_<type>(const Graph&)` を提供)
2. `src/<type>_main.cpp` に CLI を作成 (任意; gtest だけあれば動作。置くだけで `make` が `bin/<type>` を自動生成)
3. `tests/<type>/` にテストケース (.in / .exp) を配置
4. `tests/gtest/recognizers/<type>_test.cpp` を既存ファイルをコピーして作成
5. `make test` で検証

## Result 構造体の規約

`<Camel>Result` が認識結果の bool に加えて構造 (証明書・分解・モデル) を持つ場合:

- 構造フィールドは `is_<type> == true` のときのみ有効。false のときは空とする。
- ただし頂点添字ベクトル (`side`, `parent` 等) は n+1 サイズの 0 埋めで返し、
  前提条件違反時も `[1, n]` の添字アクセスが境界内に収まるようにする
  (`clique.h` の `enumerate_maximal_cliques` が既存の手本)。
- アルゴリズム variant によって充填されない構造フィールドは、その旨を
  フィールドの Doxygen コメントに明記する。
- 構造が認識の副産物として無償で得られる場合は Result のフィールドにする
  (split の (K,S) 分割、threshold の生成列など)。認識より高いコストが必要な
  場合は `build_<structure>(g)` という別のビルダー関数にして、認識のコストを
  据え置く (`build_clique_tree`, `build_cotree`, `build_pruning_sequence`)。

NO 側の証明書 (禁止部分グラフ・ホール・AT 等) を持つ場合:

- 型は `Obstruction obstruction;` (`include/forbidden_subgraph.h`) に統一する。
  `is_<type> == false` のときのみ有効で、true のときは `kind == NONE`。
- `obstruction.vertices` は頂点リストであり頂点添字ベクトルではないので、
  n+1 の 0 埋め規約は **適用しない**。意味 (並び順) は kind ごとに enum の
  Doxygen コメントで定義する。
- 補グラフ側のパターンは kind を増やさず `in_complement` で表す。co_* 系は
  これで全て賄える。
- 認識と同オーダーで取れる証明書は認識器がその場で充填する。高コストなら
  `build_<type>_obstruction(g)` に分離して既定の認識コストを据え置く
  (`build_split_obstruction`, `build_planar_obstruction` など)。
- variant によって充填されない場合も `kind == NONE` とし、その旨を
  フィールドの Doxygen コメントに明記する (YES 側の構造フィールドと同じ規約)。

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

再実装・変更時に踏み抜きやすい点は以下の別ファイルに記録している。該当箇所を触るときは先に読み、新しい知見もこちらへ追記する:

- `design_notes.md` — 各クラスの認識・列挙アルゴリズムの設計メモ (逆探索の親定義、資源制限、不正と判明した構成など)
- `decomposition_notes.md` — 分解構造の公開 (2026-08-25) の落とし穴 (SEO / modular decomposition / split / SPQR / DMP 平面埋め込み)
- `obstruction_notes.md` — NO 証明書 (obstruction) 抽出の落とし穴と見送ったクラスの理由

## コーディング規約

- C++11 互換 (`std::iota` 不可、range-for の添字は `size_t`)
- ヘッダオンリー (`inline` 関数)
- 名前空間 `graph_recognition`
- 頂点は 1-indexed

## Git Workflow

- **Commit frequently**: Make small, incremental commits after completing each feature or fix
- Do not batch multiple unrelated changes into a single commit
- **Auto-commit after implementation**: When an implementation task is completed and tests pass, automatically commit the changes without waiting for user instruction
