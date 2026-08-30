Cubic Planar Graph
==================

A **cubic planar graph** is a planar graph where every vertex has degree
exactly 3.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1102.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CubicPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CubicPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method with the degree-constrained search of the cubic graph
enumerator plus a planarity prune. Both constraints are hereditary under
vertex deletion (maximum degree at most 3, and planarity), so the
intermediate levels range over the planar graphs with maximum degree at
most 3 and every graph reaching level n is cubic planar by construction;
each candidate child is pruned by the cubic completability conditions
and one ``check_planar`` call (skipped below 6 vertices, where a
max-degree-3 graph is always planar). The dedicated generator for this
class is plantri (Brinkmann and McKay: generate plane triangulations and
dualize, or generate embedded cubic planar graphs directly); this
implementation instead reuses the shared canonical-augmentation
machinery, which is practical to about ``n = 14``. With
``connected_only`` the counts are OEIS A005964 (1, 1, 3, 9, 32, 133,
... for n = 4, 6, 8, ...); the totals allowing disconnected graphs
(1, 1, 4, 10, 37, 146, ...) are not in the OEIS. No cubic graph exists
for odd n or n < 4, so those n yield nothing.

.. doxygenenum:: graph_recognition::CubicPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CubicPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

The non-isomorphic enumeration reproduces
`OEIS A005964 <https://oeis.org/A005964>`_ (connected cubic planar
graphs) with ``connected_only`` through ``n = 14`` (A005964 gives 133
at n = 14, about 94 s), the counts for ``n <= 10`` equal the number of
planar graphs among the cubic representatives of the general cubic
enumerator, and for ``n <= 6`` the canonicalized output of the labeled
reverse-search enumerator yields exactly the same isomorphism classes.
The static test cases stop at ``n = 12``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cubic_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic_planar(g);

       std::cout << std::boolalpha << result.is_cubic_planar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 4
       return 0;
   }


References
----------

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
