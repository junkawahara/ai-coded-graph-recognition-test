キャタピラ木
========================

グラフがキャタピラ木であるかどうかを判定する。
キャタピラ木とは、すべての頂点が中心となるパス (spine) から距離 1 以内にある木である。
同値な定義として、すべての葉を取り除くとパス (または空グラフ) になる木である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_784.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::CaterpillarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CaterpillarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_caterpillar
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CaterpillarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CaterpillarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CaterpillarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_caterpillar_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 3, 4, 5, 6, 7, 8, 9, 10`` に対して、列挙された非同型なキャタピラ木の個数が、
添字を 2 ずらした `OEIS A005418 <https://oeis.org/A005418>`_ の
``1, 2, 3, 6, 10, 20, 36, 72`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/caterpillar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_caterpillar(g);

       std::cout << std::boolalpha << result.is_caterpillar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/caterpillar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_caterpillar_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* F. Harary, A. J. Schwenk. "The number of caterpillars."
  *Discrete Mathematics*, 6(4):359--365, 1973.
  `DOI:10.1016/0012-365X(73)90067-8 <https://doi.org/10.1016/0012-365X(73)90067-8>`_
