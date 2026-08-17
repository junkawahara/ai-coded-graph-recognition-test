Laman グラフ
========================

グラフが Laman グラフかどうかを判定する。
Laman 条件、すなわち m = 2n - 3 かつ任意の k 頂点上の部分グラフの辺数が
高々 2k - 3 であることを満たすグラフである。Laman グラフは平面上の極小剛性
フレームワークを特徴づける。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1206.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::LamanEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "laman.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}});
       auto result = check_laman(g);

       std::cout << std::boolalpha << result.is_laman << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "laman_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation and pseudotriangulation."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_
