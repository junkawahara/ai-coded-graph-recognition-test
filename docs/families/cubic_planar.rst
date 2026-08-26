3-正則平面グラフ
========================

**3-正則平面グラフ** は、すべての頂点の次数がちょうど 3 である平面グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1102.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::CubicPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic_planar
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CubicPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_labeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cubic_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic_planar(g);

       std::cout << std::boolalpha << result.is_cubic_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
