スナーク
================

グラフがスナークかどうかを判定する。
スナークとは、橋を持たず、内周 (girth) が 5 以上で、巡回 4-辺連結であり、
彩色指数が 4 である (3-辺彩色可能でない) 3-正則グラフである。
最小のスナークは Petersen グラフ (n = 10) なので、頂点数 10 未満のグラフは
すべて非スナークと判定する。

認識
----------

.. doxygenenum:: graph_recognition::SnarkAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_snark
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::SnarkLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_labeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "snark.h"

   int main() {
       using namespace graph_recognition;

       // Petersen graph (the smallest snark)
       Graph g(10, {
           {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
           {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6},
           {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}
       });
       auto result = check_snark(g);

       std::cout << std::boolalpha << result.is_snark << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "snark_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_labeled_graphs(10);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable."
  *The American Mathematical Monthly*, 82(3):221--239, 1975.
  `DOI:10.2307/2319844 <https://doi.org/10.2307/2319844>`_

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_
