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

The default ``BUCKYGEN`` algorithm applies the L/B patch expansions of
Brinkmann--Goedgebeur--McKay (2012) to embedded dual triangulations. Seeds
include C20, C28(Td), and the separately constructed (5,0) nanotubes
C30, C40, ... . Canonical inverse reductions and sibling codes remove
isomorphic copies, including reflections, before dualizing the results.
There is no external buckygen dependency or planarity recognition in this mode.

``REVERSE_SEARCH`` preserves the previous degree-constrained dual-candidate
search and its output order. The CLI accepts ``--algo buckygen`` (default)
or ``--algo reverse_search``. Both modes return an empty result for orders
below 20, odd orders, and order 22; both retain the range
``0 <= n <= max_vertex_count``. There is no Python enumeration binding.

The implementation copies patches and rotations, retains pending children,
sibling codes and all outputs, and uses its own canonical reduction order.
It does not reproduce buckygen's in-place updates, orbit pruning, look-aheads
or performance guarantees.

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


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/fullerene.h"

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
   #include "enumerators/fullerene_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_fullerene_unlabeled_graphs(20);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "The generation of fullerenes."
  *Journal of Chemical Information and Modeling*, 52(11):2910--2918, 2012.
  `DOI:10.1021/ci3003107 <https://doi.org/10.1021/ci3003107>`_
