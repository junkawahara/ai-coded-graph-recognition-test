閾値グラフ
====================

グラフが **閾値グラフ** であるとは、孤立頂点または全域頂点 (universal vertex;
他のすべての頂点と隣接する頂点) の除去を繰り返して空グラフに帰着できることを
いう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_328.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ThresholdAlgorithm``
     - 説明
   * - ``DEGREE_SEQUENCE``
     - 孤立頂点・全域頂点の反復除去。除去のたびに全頂点を走査するので O(n^2 + m)
   * - ``DEGREE_SEQUENCE_FAST`` **(既定)**
     - 計数ソート + two-pointer シミュレーション。判定自体は O(n) だが、
       返す生成列をグラフに対して再検証するため呼び出し全体は O(n+m)

.. doxygenenum:: graph_recognition::ThresholdAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdResult
   :project: graph_recognition
   :members:

``ThresholdResult`` は生成列そのものも返す。閾値グラフは 1 頂点から
「孤立頂点の追加」または「全域頂点の追加」を繰り返して構成でき、認識器が
行う消去を逆順にするとちょうどその列になる (``creation_order[i]`` が i 番目に
追加する頂点、``creation_kind[i]`` が 0 なら孤立、1 なら全域)。返す前に
グラフに対して再生して検証しているため、証明書として使える。

.. doxygenfunction:: graph_recognition::check_threshold
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ThresholdUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ThresholdUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_threshold_unlabeled_graphs
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_threshold_unlabeled_graphs_cb
   :project: graph_recognition

結果を構築する入口は 2^(n-1) 個のグラフを一度に保持する (n = 18 で既に
131,072 個)。callback 版は各 2 進文字列から独立にグラフを構築するため、
保持するのは常に 1 個だけである。``bin/threshold_unlabeled_enum`` はこちらを
用い、正確な件数 2^(n-1) を先に出力する。範囲外の n (bitmask の範囲は
0..63) は空の結果を件数として出力せず、エラーとして拒否する。

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` について、列挙された非同型な閾値グラフの個数が
`OEIS A011782 <https://oeis.org/A011782>`_ (Hougardy による ``n`` 頂点の
閾値グラフの個数を列挙する数列) の値 ``1, 2, 4, 8, 16, 32`` と一致することを
検証済み。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/threshold.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}});
       auto result = check_threshold(g);

       std::cout << std::boolalpha << result.is_threshold << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/threshold_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_threshold_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* V. Chvátal, P. L. Hammer. "Aggregation of inequalities in integer programming."
  *Annals of Discrete Mathematics*, 1:145--162, 1977.
  `DOI:10.1016/S0167-5060(08)70731-3 <https://doi.org/10.1016/S0167-5060(08)70731-3>`_

* N. V. R. Mahadev, U. N. Peled. *Threshold Graphs and Related Topics.*
  Annals of Discrete Mathematics 56, North-Holland, 1995.
