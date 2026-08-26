フラーレングラフ
========================

**フラーレングラフ** は、すべての面が五角形または六角形である
3-連結 3-正則平面グラフである。オイラーの公式により、フラーレングラフは
常にちょうど 12 個の五角形面を持つ。

認識
----------

.. doxygenenum:: graph_recognition::FullereneAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_fullerene
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::FullereneUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::FullereneUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_fullerene_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "fullerene.h"

   int main() {
       using namespace graph_recognition;

       // Dodecahedron (C_20 fullerene: 20 vertices, 30 edges)
       Graph g(20, {
           {1, 2}, {1, 5}, {1, 6},
           {2, 3}, {2, 7},
           {3, 4}, {3, 8},
           {4, 5}, {4, 9},
           {5, 10},
           {6, 11}, {6, 15},
           {7, 11}, {7, 12},
           {8, 12}, {8, 13},
           {9, 13}, {9, 14},
           {10, 14}, {10, 15},
           {11, 16},
           {12, 17},
           {13, 18},
           {14, 19},
           {15, 20},
           {16, 17}, {16, 20},
           {17, 18},
           {18, 19},
           {19, 20}
       });
       auto result = check_fullerene(g);

       std::cout << std::boolalpha << result.is_fullerene << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "fullerene_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_fullerene_unlabeled_graphs(20);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "The generation of fullerenes."
  *Journal of Chemical Information and Modeling*, 52(11):2910--2918, 2012.
  `DOI:10.1021/ci3003107 <https://doi.org/10.1021/ci3003107>`_
