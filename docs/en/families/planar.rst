Planar Graph
============

A graph is **planar** if it can be drawn in the plane without edge crossings.
By Wagner's theorem, a graph is planar if and only if it contains no
K\ :sub:`5` or K\ :sub:`3,3` minor. (Kuratowski's theorem gives the
equivalent characterization in terms of subdivisions rather than minors.)

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_43.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PlanarAlgorithm``
     - Description
   * - ``LEFT_RIGHT`` **(default)**
     - Edge bound precheck (3n - 6), then the left-right planarity criterion of
       de Fraysseix, Ossona de Mendez and Rosenstiehl (two DFS passes with a
       stack of conflict pairs). Runs in O(n + m).
   * - ``MINOR_CHECK``
     - Edge bound precheck (3n - 6), then backtracking search for K\ :sub:`5` and K\ :sub:`3,3` minors.
       Exact but exponential in the worst case; kept as a cross-check for small graphs.

.. doxygenenum:: graph_recognition::PlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_planar_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method — the same scheme as the chordal and split enumerators.
Graphs are grown one vertex at a time, which is sound because planar
graphs are hereditary. The pruning is a ``check_planar`` call (the
linear-time left-right planarity test) on every candidate child; it runs
before the isomorph rejection, so the more expensive canonicalization
only ever sees planar graphs. The counts are OEIS A005470(n)
(1, 2, 4, 11, 33, 142, 822, 6966, 79853, ...), or the connected ones
among them (A003094: 1, 1, 2, 6, 20, 99, 646, 5974, ...) with
``connected_only`` set.

.. doxygenenum:: graph_recognition::PlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_planar_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled planar graphs
was verified to match `OEIS A066537 <https://oeis.org/A066537>`_:
``1, 2, 8, 64, 1023``.

The non-isomorphic enumeration was verified through ``n = 9`` against
`OEIS A005470 <https://oeis.org/A005470>`_
(``1, 2, 4, 11, 33, 142, 822, 6966, 79853``), and through ``n = 8``
independently of the enumerator itself: the same counts come out of
generating all unlabeled graphs on ``n`` vertices and filtering them by
``check_planar``. With ``connected_only`` the counts match
`OEIS A003094 <https://oeis.org/A003094>`_
(``1, 1, 2, 6, 20, 99, 646, 5974``). The static test cases stop at
``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_planar(g);

       std::cout << std::boolalpha << result.is_planar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_planar_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 142 = A005470(6)
       return 0;
   }


References
----------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* K. Wagner. "Über eine Eigenschaft der ebenen Komplexe."
  *Mathematische Annalen*, 114(1):570--590, 1937.
  `DOI:10.1007/BF01594196 <https://doi.org/10.1007/BF01594196>`_

* J. Hopcroft, R. Tarjan. "Efficient planarity testing."
  *Journal of the ACM*, 21(4):549--568, 1974.
  `DOI:10.1145/321850.321852 <https://doi.org/10.1145/321850.321852>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
