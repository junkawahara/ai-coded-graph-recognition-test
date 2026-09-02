Maximal Planar Graph
====================

A **maximal planar graph** (triangulation) is a planar graph where adding any
edge would violate planarity. In a maximal planar graph, every face
(including the outer face) is a triangle.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_981.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::MaximalPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::MaximalPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method on the same machinery as the planar graph enumerator. The
class itself is not hereditary (deleting a vertex of a triangulation
leaves a planar graph that is no longer edge-maximal), but every
vertex-deleted subgraph is still planar, so the search grows *planar*
graphs one vertex at a time and emits the n-vertex graphs whose edge
count reaches ``3n - 6``. Every candidate child is pruned by a
``check_planar`` call and by an edge-count window (the child must still
be able to reach ``3n - 6`` edges); both conditions are isomorphism
invariants, so McKay's one-parent-per-class argument is unaffected.
The dedicated generator for this class is plantri's canonical
construction path over the embedded plane triangulations themselves
(Brinkmann and McKay, millions of graphs per second); this
implementation instead reuses the shared canonical-augmentation
machinery, which is practical to about ``n = 10``. The counts are 1
for n = 1, 2 (K1 and K2) and OEIS A000109 (1, 1, 1, 2, 5, 14, 50,
233, ... for n = 3, 4, ...) from there. Every maximal planar graph is
connected, so there is no ``connected_only`` flag.

.. doxygenenum:: graph_recognition::MaximalPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_unlabeled_graphs
   :project: graph_recognition

Count Check
-----------

The non-isomorphic enumeration reproduces OEIS A000109 through
``n = 10`` (A000109(10) = 233, about 16 s), and for ``n <= 6`` the
canonicalized output of the labeled reverse-search enumerator yields
exactly the same isomorphism classes. The static test cases stop at
``n = 9``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "maximal_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_maximal_planar(g);

       std::cout << std::boolalpha << result.is_maximal_planar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "maximal_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "maximal_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 14
       return 0;
   }


References
----------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
