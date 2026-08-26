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

OEIS Count Check
----------------

For ``n = 4, 5, 6``, the number of enumerated labeled polyhedral graphs
was verified to match `OEIS A096330 <https://oeis.org/A096330>`_:
``1, 25, 1227``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "polyhedral.h"

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
   #include "polyhedral_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* E. Steinitz. "Polyeder und Raumeinteilungen."
  *Encyclopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.

* B. Grünbaum. *Convex Polytopes.*
  Graduate Texts in Mathematics 221, Springer, 2nd edition, 2003.
  `DOI:10.1007/978-1-4613-0019-9 <https://doi.org/10.1007/978-1-4613-0019-9>`_
