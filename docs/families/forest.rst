森
==========

グラフが森であるかどうかを判定する。
森とは閉路を持たないグラフ (木の非交和) である。

認識
----------

.. doxygenenum:: graph_recognition::ForestAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ForestResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_forest
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::ForestEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ForestEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ForestEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_forest_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12`` に対して、列挙された非同型な森の個数が
`OEIS A005195 <https://oeis.org/A005195>`_ の ``1, 2, 3, 6, 10, 20, 37, 76, 153, 329,
710, 1601`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "forest.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_forest(g);

       std::cout << std::boolalpha << result.is_forest << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "forest_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_forest_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
