k-正則グラフ
========================

グラフが k-正則であるかどうかを判定する。
k-正則グラフとはすべての頂点が同じ次数 k を持つグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1149.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::KRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_kregular
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::KRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_kregular_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "kregular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_kregular(g);

       std::cout << std::boolalpha << result.is_kregular << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "kregular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_kregular_labeled_graphs_reverse_search(4, 2);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
