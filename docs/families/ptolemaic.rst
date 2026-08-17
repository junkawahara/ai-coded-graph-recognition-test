Ptolemaic グラフ
================================

グラフが **ptolemaic** であるとは、弦グラフかつ距離遺伝グラフ
(distance-hereditary graph) であることをいう。これは gem-free かつ弦グラフで
あることと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_95.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - 説明
   * - ``DH_HASHMAP``
     - ハッシュマップを用いた距離遺伝性の検査
   * - ``DH_SORTED`` **(既定)**
     - ソート済み隣接リストを用いた距離遺伝性の検査

.. doxygenenum:: graph_recognition::PtolemaicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_ptolemaic
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::PtolemaicEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ptolemaic_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "ptolemaic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_ptolemaic(g);

       std::cout << std::boolalpha << result.is_ptolemaic << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ptolemaic_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ptolemaic_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* E. Howorka. "A characterization of Ptolemaic graphs."
  *Journal of Graph Theory*, 5(3):323--331, 1981.
  `DOI:10.1002/jgt.3190050314 <https://doi.org/10.1002/jgt.3190050314>`_
