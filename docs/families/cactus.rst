カクタスグラフ
====================

各二連結成分が単一の辺または単純サイクルであるグラフを **カクタスグラフ** という。
同値な定義として、任意の 2 つの単純サイクルが高々 1 つの頂点しか共有しない、とも言える。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_108.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - 説明
   * - ``DFS`` **(既定)**
     - DFS による二連結成分分解。各成分が単一の辺または単純サイクルであることを検証する。計算量: O(n + m)。

.. doxygenenum:: graph_recognition::CactusAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cactus
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CactusEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cactus_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cactus.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_cactus(g);

       std::cout << std::boolalpha << result.is_cactus << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cactus_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cactus_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* F. Harary, G. E. Uhlenbeck. "On the number of husimi trees, I."
  *Proceedings of the National Academy of Sciences*, 39(4):315--322, 1953.
  `DOI:10.1073/pnas.39.4.315 <https://doi.org/10.1073/pnas.39.4.315>`_
