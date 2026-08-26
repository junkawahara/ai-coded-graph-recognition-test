双凸二部グラフ
========================================

グラフが biconvex bipartite グラフであるかを判定する。
両側それぞれに線形順序を与えて、各頂点の反対側の近傍が連続した区間を
なすようにできる二部グラフ G=(X, Y, E) を指す。
双隣接行列の行と列の両方が連続 1 性を満たす。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_66.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BiconvexBipartiteAlgorithm``
     - 説明
   * - ``BRUTE_FORCE``
     - 各側の全順列を独立に試す (X 側の C1P と Y 側の C1P は同時にではなく
       別々に検査する)。
       計算量: :math:`O((|X|! + |Y|!) \cdot (n + m))`。
   * - ``C1P`` **(既定)**
     - PQ 木 (Booth & Lueker 1976) により両側の C1P を検査する。
       計算量: 最悪の場合 :math:`O(n \cdot m)` -- この PQ 木実装は
       原論文の BUBBLE パスを省略し、各リダクションで全ての pertinent 葉から
       木の根まで辿るため、Booth & Lueker の O(n + m) の計算量には達しない。

.. doxygenenum:: graph_recognition::BiconvexBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconvexBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconvex_bipartite
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::BiconvexBipartiteLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconvexBipartiteLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconvex_bipartite_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "biconvex_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_biconvex_bipartite(g);

       std::cout << std::boolalpha << result.is_biconvex_bipartite << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "biconvex_bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconvex_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* N. Abbas, L. K. Stewart. "Biconvex graphs: ordering and algorithms."
  *Discrete Applied Mathematics*, 103(1--3):1--19, 2000.
  `DOI:10.1016/S0166-218X(99)00217-6 <https://doi.org/10.1016/S0166-218X(99)00217-6>`_

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
