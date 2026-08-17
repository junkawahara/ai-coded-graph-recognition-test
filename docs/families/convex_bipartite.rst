凸二部グラフ
========================

グラフが凸二部グラフであるかを判定する。
Y に線形順序を与えて、X の各頂点の近傍が連続した区間をなすように
できる二部グラフ G=(X, Y, E) を指す。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_67.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ConvexBipartiteAlgorithm``
     - 説明
   * - ``BRUTE_FORCE``
     - Y の全順列を試して連続 1 性 (C1P) を検査する。
       計算量: :math:`O(|Y|! \cdot (|X| + m))`。
   * - ``C1P`` **(default)**
     - PQ 木により C1P を判定する。計算量: 最悪の場合 :math:`O(n \cdot m)` --
       この PQ 木実装は Booth & Lueker (1976) の BUBBLE パスを省略し、
       各リダクションで全ての pertinent 葉から木の根まで辿るため、
       原論文の O(n + m) の計算量には達しない。

.. doxygenenum:: graph_recognition::ConvexBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ConvexBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_convex_bipartite
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::ConvexBipartiteEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ConvexBipartiteEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_convex_bipartite_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "convex_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_convex_bipartite(g);

       std::cout << std::boolalpha << result.is_convex_bipartite << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "convex_bipartite_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_convex_bipartite_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
