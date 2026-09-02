P5 フリーグラフ
========================

グラフが P_5-free かどうかを判定する。
5 頂点の誘導パスを含まない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_396.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::P5FreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::P5FreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_p5_free
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::P5FreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::P5FreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_p5_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/p5_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_p5_free(g);

       std::cout << std::boolalpha << result.is_p5_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/p5_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_p5_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
