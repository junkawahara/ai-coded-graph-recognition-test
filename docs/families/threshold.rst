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
     - 次数列に基づく認識、O(n m)
   * - ``DEGREE_SEQUENCE_FAST`` **(既定)**
     - 計数ソート + two-pointer シミュレーション、O(n + m)

.. doxygenenum:: graph_recognition::ThresholdAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_threshold
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ThresholdEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ThresholdEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_threshold_graphs
   :project: graph_recognition

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
   #include "graph.h"
   #include "threshold.h"

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
   #include "threshold_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_threshold_graphs(4);
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
