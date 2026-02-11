# Graph Recognition Library

C++11 ヘッダオンリーライブラリ。グラフクラスの認識アルゴリズムを提供する。

## ビルド

```
make          # 全ターゲットをビルド
make clean    # バイナリを削除
```

コンパイラ: C++11 対応の g++ (デフォルト)。`CXX`, `CXXFLAGS` で変更可能。

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
  <type>/        各グラフクラスのテストケース (.in / .exp)
  check_<type>.py  チェッカースクリプト
  run.sh         テストランナー
  compare.py     差分テスト (2バイナリ比較)
  fuzz.sh        ファズテスト
docs/          Sphinx + Doxygen ドキュメント
```

## テスト

```
bash tests/run.sh interval            # interval 静的テスト
bash tests/run.sh chordal             # chordal 静的テスト
python3 tests/compare.py ./interval ./interval 1000  # 自己検証 (1000ケース)
```

## 新しいグラフクラスの追加手順

1. `include/<type>.h` にヘッダを作成
2. `src/<type>_main.cpp` に CLI を作成
3. `Makefile` の `TARGETS` に `<type>` を追加し、ビルドルールを記述
4. `tests/<type>/` にテストケース (.in / .exp) を配置
5. `tests/check_<type>.py` にチェッカーを作成
6. `bash tests/run.sh <type>` で検証

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

## コーディング規約

- C++11 互換 (`std::iota` 不可、range-for の添字は `size_t`)
- ヘッダオンリー (`inline` 関数)
- 名前空間 `graph_recognition`
- 頂点は 1-indexed

## Git Workflow

- **Commit frequently**: Make small, incremental commits after completing each feature or fix
- Do not batch multiple unrelated changes into a single commit
- **Auto-commit after implementation**: When an implementation task is completed and tests pass, automatically commit the changes without waiting for user instruction
