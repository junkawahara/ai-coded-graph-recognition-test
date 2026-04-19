Simple Quadrangulation
======================

A **simple quadrangulation** is a 2-connected planar graph where every face
(including the outer face) is a quadrilateral (4-cycle).

Recognition
-----------

.. doxygenenum:: graph_recognition::SimpleQuadrangulationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_simple_quadrangulation
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SimpleQuadrangulationEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_simple_quadrangulation_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "simple_quadrangulation.h"

   int main() {
       using namespace graph_recognition;

       // Cube graph Q_3 (8 vertices, 12 edges)
       Graph g(8, {
           {1, 2}, {2, 3}, {3, 4}, {4, 1},
           {5, 6}, {6, 7}, {7, 8}, {8, 5},
           {1, 5}, {2, 6}, {3, 7}, {4, 8}
       });
       auto result = check_simple_quadrangulation(g);

       std::cout << std::boolalpha << result.is_simple_quadrangulation << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "simple_quadrangulation_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_simple_quadrangulation_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* G. Brinkmann, B. D. McKay. "Construction of planar triangulations with minimum degree 4."
  *Discrete Mathematics*, 301(2--3):147--163, 2005.
  `DOI:10.1016/j.disc.2005.06.019 <https://doi.org/10.1016/j.disc.2005.06.019>`_
