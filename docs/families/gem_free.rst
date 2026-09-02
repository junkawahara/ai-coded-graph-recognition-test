ジェムフリーグラフ
==========================

グラフがジェムフリーかどうかを判定する。
ジェム (扇グラフ F_{1,4}、すなわちパス P_4 に P_4 の全頂点と隣接する
頂点を 1 つ加えたグラフ) と同型な誘導部分グラフを含まない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_354.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::GemFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::GemFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_gem_free
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::GemFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::GemFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_gem_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/gem_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_gem_free(g);

       std::cout << std::boolalpha << result.is_gem_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/gem_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_gem_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
