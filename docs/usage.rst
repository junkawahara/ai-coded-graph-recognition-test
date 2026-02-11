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
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::BISIMPLICIAL);  // デフォルト

   // 距離遺伝グラフ
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::HASHMAP_TWINS);
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::SORTED_TWINS);  // デフォルト


テスト
------

.. code-block:: bash

   bash tests/run.sh interval            # interval 静的テスト
   bash tests/run.sh chordal             # chordal 静的テスト
   bash tests/run.sh permutation         # permutation 静的テスト

   # 全グラフクラスのテスト
   for t in interval chordal permutation bipartite chordal_bipartite \
            threshold split cograph block distance_hereditary ptolemaic \
            proper_interval trivially_perfect comparability at_free \
            co_comparability chain cochain co_interval co_chordal \
            unit_interval quasi_threshold strongly_chordal weakly_chordal \
            bipartite_permutation circular_arc planar outer_planar \
            cactus series_parallel; do
       bash tests/run.sh $t
   done
