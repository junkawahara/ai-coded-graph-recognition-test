Polyhedral Graph
================

A **polyhedral graph** is a 3-connected planar graph. By Steinitz's theorem,
these are exactly the graphs of convex polyhedra.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_986.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::PolyhedralAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_polyhedral
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PolyhedralLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method on the same machinery as the planar graph enumerator. The
class itself is not hereditary (deleting a vertex can destroy
3-connectivity), but every vertex-deleted subgraph is still planar, so
the search grows *planar* graphs one vertex at a time and runs the full
``check_polyhedral`` on the n-vertex graphs. Every candidate child is
pruned by a ``check_planar`` call and by an edge-count window (an
n-vertex polyhedral graph has ``ceil(3n/2) <= m <= 3n - 6``, from
minimum degree 3 and planarity); both conditions are isomorphism
invariants, so McKay's one-parent-per-class argument is unaffected.
The dedicated generator for this class is plantri's canonical
construction path over the embedded 3-connected planar graphs
themselves (Brinkmann and McKay, millions of graphs per second); this
implementation instead reuses the shared canonical-augmentation
machinery, which is practical to about ``n = 9``. The counts are OEIS
A000944 (1, 2, 7, 34, 257, 2606, ... for n = 4, 5, ...); no graph on
fewer than 4 vertices is 3-connected, so smaller n yield nothing.
Every polyhedral graph is connected, so there is no ``connected_only``
flag.

.. doxygenenum:: graph_recognition::PolyhedralUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 4, 5, 6``, the number of enumerated labeled polyhedral graphs
was verified to match `OEIS A096330 <https://oeis.org/A096330>`_:
``1, 25, 1227``.

The non-isomorphic enumeration reproduces
`OEIS A000944 <https://oeis.org/A000944>`_ through ``n = 9``
(A000944(9) = 2606, about 14 s), and for ``n <= 6`` the canonicalized
output of the labeled reverse-search enumerator yields exactly the same
isomorphism classes. The static test cases stop at ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/polyhedral.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_polyhedral(g);

       std::cout << std::boolalpha << result.is_polyhedral << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 7
       return 0;
   }


References
----------

* E. Steinitz. "Polyeder und Raumeinteilungen."
  *Encyclopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.

* B. Grünbaum. *Convex Polytopes.*
  Graduate Texts in Mathematics 221, Springer, 2nd edition, 2003.
  `DOI:10.1007/978-1-4613-0019-9 <https://doi.org/10.1007/978-1-4613-0019-9>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
