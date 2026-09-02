Outerplanar Graph
=================

A graph is **outerplanar** if it can be embedded in the plane with all vertices
on the outer face. Equivalently, it contains no K\ :sub:`4` or K\ :sub:`2,3` minor.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_110.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``OuterPlanarAlgorithm``
     - Description
   * - ``AUGMENTED_PLANARITY`` **(default)**
     - Tests planarity of the augmented graph G + K\ :sub:`1` (a new vertex adjacent to all others). G is outerplanar if and only if G + K\ :sub:`1` is planar.
   * - ``MINOR_CHECK``
     - Edge bound precheck (2n - 3), then backtracking search for K\ :sub:`4` and K\ :sub:`2,3` minors.

.. doxygenenum:: graph_recognition::OuterPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_outer_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::OuterPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_outer_planar_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method — the same scheme as the permutation, circle, chordal, and
interval enumerators. Graphs are grown one vertex at a time, which is
sound because outerplanar graphs are hereditary. The pruning is a
``check_outer_planar`` call (the linear-time planarity test of
G + K\ :sub:`1`) on every candidate child; it runs before the isomorph
rejection, so the more expensive canonicalization only ever sees
outerplanar graphs. The dedicated enumerator of Wang and Nagamochi
generates rooted connected outerplanar graphs in constant time per
graph and never canonicalizes; this implementation instead reuses the
shared canonical-augmentation machinery, which is practical to about
``n = 10``. The counts are OEIS A111564(n)
(1, 2, 4, 10, 25, 80, 277, 1150, 5291, ...), or the connected ones
among them (A111563: 1, 1, 2, 5, 13, 46, 172, 777, 3783, ...) with
``connected_only`` set.

.. doxygenenum:: graph_recognition::OuterPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::OuterPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_outer_planar_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled outerplanar
graphs was verified to match `OEIS A098000 <https://oeis.org/A098000>`_:
``1, 2, 8, 63, 893``.

The non-isomorphic enumeration was verified through ``n = 9`` against
`OEIS A111564 <https://oeis.org/A111564>`_
(``1, 2, 4, 10, 25, 80, 277, 1150, 5291``), independently of the
enumerator itself: the same counts come out of filtering the
non-isomorphic planar enumeration by ``check_outer_planar``. With
``connected_only`` the counts match `OEIS A111563
<https://oeis.org/A111563>`_ (``1, 1, 2, 5, 13, 46, 172, 777``).
The static test cases stop at ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/outer_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_outer_planar(g);

       std::cout << std::boolalpha << result.is_outer_planar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/outer_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_outer_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/outer_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_outer_planar_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 80 = A111564(6)
       return 0;
   }


References
----------

* G. Chartrand, F. Harary. "Planar permutation graphs."
  *Annales de l'Institut Henri Poincaré B*, 3(4):433--438, 1967.

* S. L. Mitchell. "Linear algorithms to recognize outerplanar and maximal outerplanar graphs."
  *Information Processing Letters*, 9(5):229--232, 1979.
  `DOI:10.1016/0020-0190(79)90075-9 <https://doi.org/10.1016/0020-0190(79)90075-9>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* J. Wang, H. Nagamochi. "Constant time generation of rooted and colored outerplanar graphs."
  *Algorithmic Aspects in Information and Management (AAIM 2010)*, LNCS 6124, 300--309, 2010.
  `DOI:10.1007/978-3-642-14355-7_31 <https://doi.org/10.1007/978-3-642-14355-7_31>`_
