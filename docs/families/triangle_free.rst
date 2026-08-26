三角形フリーグラフ
==========================

グラフが三角形フリーかどうかを判定する。
互いに隣接する 3 頂点を含まない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_371.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::TriangleFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_triangle_free
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::TriangleFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付き三角形フリー
グラフの個数が `OEIS A213434 <https://oeis.org/A213434>`_ の
``2, 7, 41, 388, 5789`` と一致することを検証した。


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "triangle_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_triangle_free(g);

       std::cout << std::boolalpha << result.is_triangle_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "triangle_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_triangle_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* A. Itai, M. Rodeh. "Finding a minimum circuit in a graph."
  *SIAM Journal on Computing*, 7(4):413--423, 1978.
  `DOI:10.1137/0207033 <https://doi.org/10.1137/0207033>`_
