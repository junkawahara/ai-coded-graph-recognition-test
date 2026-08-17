多面体グラフ
====================

**多面体グラフ** は 3-連結平面グラフである。Steinitz の定理により、
これはちょうど凸多面体のグラフに一致する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_986.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::PolyhedralAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_polyhedral
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::PolyhedralEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 4, 5, 6`` について、列挙されたラベル付き多面体グラフの個数が
`OEIS A096330 <https://oeis.org/A096330>`_ の値 ``1, 25, 1227`` と
一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "polyhedral.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_polyhedral(g);

       std::cout << std::boolalpha << result.is_polyhedral << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "polyhedral_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* E. Steinitz. "Polyeder und Raumeinteilungen."
  *Encyclopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.

* B. Grünbaum. *Convex Polytopes.*
  Graduate Texts in Mathematics 221, Springer, 2nd edition, 2003.
  `DOI:10.1007/978-1-4613-0019-9 <https://doi.org/10.1007/978-1-4613-0019-9>`_
