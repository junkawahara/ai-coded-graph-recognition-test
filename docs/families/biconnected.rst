2-連結グラフ
========================

グラフが 2-連結 (biconnected) であるかどうかを判定する。
2-連結グラフとは、頂点数が 3 以上で切断点を持たない連結グラフである。
頂点数が 3 未満のグラフは 2-連結でないと判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_771.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::BiconnectedAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconnected
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::BiconnectedEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "biconnected.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_biconnected(g);

       std::cout << std::boolalpha << result.is_biconnected << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "biconnected_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* R. Tarjan. "Depth-first search and linear graph algorithms."
  *SIAM Journal on Computing*, 1(2):146--160, 1972.
  `DOI:10.1137/0201010 <https://doi.org/10.1137/0201010>`_
