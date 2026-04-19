Fullerene Graph
===============

A **fullerene graph** is a 3-connected cubic planar graph where every face is
either a pentagon or a hexagon. By Euler's formula, every fullerene has exactly
12 pentagonal faces.

Recognition
-----------

.. doxygenenum:: graph_recognition::FullereneAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_fullerene
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::FullereneEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::FullereneEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_fullerene_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

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

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "fullerene_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_fullerene_graphs(20);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "The generation of fullerenes."
  *Journal of Chemical Information and Modeling*, 52(11):2910--2918, 2012.
  `DOI:10.1021/ci3003107 <https://doi.org/10.1021/ci3003107>`_
