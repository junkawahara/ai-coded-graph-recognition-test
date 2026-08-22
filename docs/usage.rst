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


使用例 (C++)
------------

基本的な使い方:

.. code-block:: cpp

   #include "graph.h"
   #include "interval.h"
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

   #include "graph.h"
   #include "interval.h"

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

   echo 5 | ./bin/interval_enum
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
