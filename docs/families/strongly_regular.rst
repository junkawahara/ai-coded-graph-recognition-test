強正則グラフ
========================

グラフがパラメータ (n, k, lambda, mu) の強正則グラフかどうかを判定する。
k-正則グラフであって、隣接する任意の 2 頂点がちょうど lambda 個の共通近傍を持ち、
非隣接な任意の 2 頂点がちょうど mu 個の共通近傍を持つグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1185.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::StronglyRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_regular
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::StronglyRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_labeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "strongly_regular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       auto result = check_strongly_regular(g);

       std::cout << std::boolalpha << result.is_strongly_regular << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "strongly_regular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_regular_labeled_graphs(5);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs."
  *Pacific Journal of Mathematics*, 13(2):389--419, 1963.
  `DOI:10.2140/pjm.1963.13.389 <https://doi.org/10.2140/pjm.1963.13.389>`_
