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

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled outerplanar
graphs was verified to match `OEIS A098000 <https://oeis.org/A098000>`_:
``1, 2, 8, 63, 893``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "outer_planar.h"

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
   #include "outer_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_outer_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* G. Chartrand, F. Harary. "Planar permutation graphs."
  *Annales de l'Institut Henri Poincaré B*, 3(4):433--438, 1967.

* S. L. Mitchell. "Linear algorithms to recognize outerplanar and maximal outerplanar graphs."
  *Information Processing Letters*, 9(5):229--232, 1979.
  `DOI:10.1016/0020-0190(79)90075-9 <https://doi.org/10.1016/0020-0190(79)90075-9>`_
