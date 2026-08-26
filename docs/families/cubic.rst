3-正則グラフ (cubic)
========================================

グラフが 3-正則 (cubic) であるかどうかを判定する。
3-正則グラフとはすべての頂点の次数がちょうど 3 であるグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1100.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::CubicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CubicLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_labeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 4, 6, 8`` に対して、列挙されたラベル付き 3-正則グラフの個数が
`OEIS A002829 <https://oeis.org/A002829>`_ (``a(k)`` は頂点数 ``2k`` で添字付けされて
いるため、ここでの頂点数 ``n`` は OEIS の添字 ``n/2`` に対応する) の
``1, 70, 19355`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cubic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic(g);

       std::cout << std::boolalpha << result.is_cubic << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "Generation of cubic graphs."
  *Discrete Mathematics and Theoretical Computer Science*, 13(2):69--80, 2011.
