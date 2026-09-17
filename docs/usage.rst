使い方
======

ビルド
------

.. code-block:: bash

   make          # 全ターゲットをビルド
   make clean    # バイナリを削除


入出力形式
----------

**入力** (stdin):

.. code-block:: text

   n m
   u1 v1
   u2 v2
   ...

* ``n``: 頂点数、``m``: 辺数
* 頂点番号は 1-indexed

**出力** (stdout): ``YES`` / ``NO`` + グラフクラス固有の情報。

**終了コード**: 判定を出力できたときは 0。入力が形式に合わないときは stderr に
``error: malformed graph input`` を出して 1 (有向グラフを読む ``digraph`` /
``poset`` / ``tournament`` も同じ)。認識中にメモリ確保に失敗した
(``std::bad_alloc``) など、例外により判定が得られなかったときは stderr に
``error: ...; result unknown`` を出して 2 で終了し、YES/NO は出力しない。
頂点数の 2 乗以上のメモリを使う認識器があるため (本プロジェクトは O(n^2)、O(n^3)、
O(2^n) のアルゴリズムを許容する)、大きな入力ではメモリ不足で 2 になりうる。
再帰の深さが入力に比例する認識器 (line_graph, circular_arc, meyniel,
even_hole_free) は、スタックオーバーフローを避けるための再帰上限に達すると
同じく 2 で終了する。

列挙 CLI (``bin/<type>_labeled_enum`` / ``bin/<type>_unlabeled_enum``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**入力** (stdin): 頂点数 ``n`` (k-正則、k-木、k-退化、部分 k-木、ケージは続けて ``k``
や内周などのパラメータ)。非同型列挙の一部は ``--connected`` オプションで連結な
グラフに限定できる。

**出力** (stdout): 1 行目にグラフの個数、続いてグラフごとに 1 行
``m u1 v1 u2 v2 ...`` (辺数 ``m`` と 1-indexed の辺)。

**終了コード**: 出力できたときは 0。次の場合は stderr に ``error: ...`` を出して
1 で終了し、stdout には何も出さない: ``n`` (やパラメータ) が読めない、``n`` が負、
``n`` が列挙器の表現できる範囲 (隣接関係を 64 ビット語に収める列挙器では 63 または
64、有向の正準形では 33) を超える、未知のオプションが指定された。範囲外の ``n`` で
「0 個」と答えることはない (どのクラスも辺なしグラフを含むため 0 は常に誤り)。
chordal / split / strongly_chordal / proper_chordal のラベル付き列挙は出力量の
都合で CLI 側に小さい上限 (7〜8) を置いている。探索予算を使い切る circle と
five_leaf_power、ケージの探索が打ち切られた場合など、例外で結果が得られなかった
ときは 2。


使用例 (C++)
------------

基本的な使い方:

.. code-block:: cpp

   #include "util/graph.h"
   #include "recognizers/interval.h"
   #include <iostream>

   int main() {
       using namespace graph_recognition;
       Graph g = Graph::read(std::cin);
       IntervalResult res = check_interval(g);
       if (res.is_interval) {
           std::cout << "YES" << std::endl;
           for (int v = 1; v <= g.n; ++v) {
               std::cout << v << ": ["
                         << res.intervals[v].first << ", "
                         << res.intervals[v].second << "]"
                         << std::endl;
           }
       } else {
           std::cout << "NO" << std::endl;
       }
       return 0;
   }


アルゴリズム選択
^^^^^^^^^^^^^^^^

各認識関数は ``enum class`` パラメータでアルゴリズムを選択できます。
デフォルト引数が設定されているため、省略すると推奨アルゴリズムが使用されます。

.. code-block:: cpp

   #include "util/graph.h"
   #include "recognizers/interval.h"

   using namespace graph_recognition;
   Graph g = Graph::read(std::cin);

   // デフォルト (AT_FREE) を使用
   IntervalResult r1 = check_interval(g);

   // 明示的にアルゴリズムを指定
   IntervalResult r2 = check_interval(g, IntervalAlgorithm::BACKTRACKING);
   IntervalResult r3 = check_interval(g, IntervalAlgorithm::AT_FREE);

複数のアルゴリズムが選択可能なクラス:

.. code-block:: cpp

   // 順列グラフ
   check_permutation(g, PermutationAlgorithm::BACKTRACKING);
   check_permutation(g, PermutationAlgorithm::CLASS_BASED);  // デフォルト

   // 弦二部グラフ
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::CYCLE_CHECK);
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::BISIMPLICIAL);
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL);  // デフォルト

   // 距離遺伝グラフ
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::HASH_TWINS);  // デフォルト
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::SORTED_TWINS);


CLI 実行ファイル
----------------

各グラフクラスには ``src/<type>_main.cpp`` に対応する CLI 実行ファイルがある。
``make`` でビルドした後、次のように実行する:

.. code-block:: bash

   printf "4 3\n1 2\n2 3\n3 4\n" | ./bin/interval
   # 出力: YES に続いてインターバル表現

列挙用の実行ファイルは頂点数を受け取る:

.. code-block:: bash

   echo 5 | ./bin/interval_labeled_enum
   # 出力: 1 行目にグラフ数、続いて 5 頂点のラベル付きインターバルグラフの辺リスト


テスト
------

.. code-block:: bash

   make test                                # 既定フィルタ付き (ビルド済みなら約 6 秒)
   make test-quick                          # property テストも実行 (超低速の列挙ケースのみ除外)
   ./gtest_all --gtest_filter='Interval*'   # 部分実行

旧テストインフラ (``check_<type>.py``, ``compare.py``, ``fuzz.sh`` を
含む ``tests/legacy/``) は削除済み。必要な場合は git タグ
``legacy-tests`` から取り出せる。
