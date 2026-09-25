Simple Quadrangulation
======================

A **simple quadrangulation** is a 3-connected planar graph where every face
(including the outer face) is a quadrilateral (4-cycle). The smallest one is
the cube (n = 8); C4 and K_{2,3} are only 2-connected and are rejected.

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

The default ``PLANTRI`` algorithm applies P1/P3 expansions from every
pseudo-double wheel, as in Brinkmann et al., Theorem 3. P1's separating-triple
test preserves 3-connectivity. Canonical reductions and sibling codes
remove duplicates, including reflections. No external program is required.
The former 4-regular graph generation and dualization remains available as
``REVERSE_SEARCH`` (CLI: ``--algo reverse_search``; default: ``--algo plantri``).
Both modes retain all outputs and return no graphs for n < 8 or n = 9;
odd orders starting at n = 11 are included. The input range is unchanged
(``0..max_vertex_count``), with much smaller practical limits.

.. doxygenenum:: graph_recognition::SimpleQuadrangulationUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_simple_quadrangulation_unlabeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/simple_quadrangulation.h"

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
   #include "enumerators/simple_quadrangulation_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       // The smallest simple quadrangulation is the cube graph (n = 8),
       // so this prints 1.
       auto result = enumerate_simple_quadrangulation_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* G. Brinkmann, S. Greenberg, C. Greenhill, B. D. McKay, R. Thomas, P. Wollan.
  "Generation of simple quadrangulations of the sphere."
  *Discrete Mathematics*, 305:33--54, 2005. Theorem 3, Figure 3 and Table 3.
  `DOI:10.1016/j.disc.2005.10.005 <https://doi.org/10.1016/j.disc.2005.10.005>`_
