ブルフリーグラフ
========================

グラフがブルフリーかどうかを判定する。
ブル (三角形の相異なる 2 頂点にペンダント辺を 1 本ずつ付けたグラフ)
と同型な誘導部分グラフを含まない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_372.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::BullFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BullFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bull_free
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::BullFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BullFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bull_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/bull_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bull_free(g);

       std::cout << std::boolalpha << result.is_bull_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/bull_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bull_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* M. Chudnovsky. "The structure of bull-free graphs I --- Three-edge-paths with centers and anticenters."
  *Journal of Combinatorial Theory, Series B*, 102(1):233--251, 2012.
  `DOI:10.1016/j.jctb.2011.07.003 <https://doi.org/10.1016/j.jctb.2011.07.003>`_
