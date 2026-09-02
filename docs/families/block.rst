ブロックグラフ
============================

グラフが **ブロックグラフ** であるとは、すべての 2 連結成分がクリークで
あることをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_93.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - 説明
   * - ``DFS`` **(既定)**
     - DFS による 2 連結成分の検査、O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - 弦グラフ性 + diamond-free による認識、O(n + m Delta^2)

.. doxygenenum:: graph_recognition::BlockAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_block
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::BlockLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/block.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_block(g);

       std::cout << std::boolalpha << result.is_block << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/block_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_block_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* F. Harary. "A characterization of block-graphs."
  *Canadian Mathematical Bulletin*, 6(1):1--6, 1963.
  `DOI:10.4153/CMB-1963-001-x <https://doi.org/10.4153/CMB-1963-001-x>`_
