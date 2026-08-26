プロパー円弧グラフ
========================

グラフがプロパー円弧グラフかどうかを判定する。
真の包含関係を持たない弧で表現できる円弧グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_297.html>`_ を参照。

認識
------------

.. doxygenstruct:: graph_recognition::ProperCircularArcResult
   :project: graph_recognition
   :members:

``arcs`` は proper なアーク模型 (どのアークも他を包含しない) を返す。
符号化は ``CircularArcResult::arcs`` と同じ。

.. doxygenfunction:: graph_recognition::check_proper_circular_arc
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ProperCircularArcLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperCircularArcLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_circular_arc_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "proper_circular_arc.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_circular_arc(g);

       std::cout << std::boolalpha << result.is_proper_circular_arc << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "proper_circular_arc_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_circular_arc_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* A. Tucker. "Structure theorems for some circular-arc graphs."
  *Discrete Mathematics*, 7(1--2):167--195, 1974.
  `DOI:10.1016/S0012-365X(74)80027-0 <https://doi.org/10.1016/S0012-365X(74)80027-0>`_

* X. Deng, P. Hell, J. Huang. "Linear-time representation algorithms for proper circular-arc graphs and proper interval graphs."
  *SIAM Journal on Computing*, 25(2):390--403, 1996.
  `DOI:10.1137/S0097539792269095 <https://doi.org/10.1137/S0097539792269095>`_
