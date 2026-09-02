Triangle-Free Graph
===================

Determines whether a graph is triangle-free.
No three mutually adjacent vertices.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_371.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::TriangleFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_triangle_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::TriangleFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method (the algorithm behind ``geng -t``): graphs are grown one
vertex at a time, the new vertex's neighborhood must be an independent
set (which is exactly what keeps the graph triangle-free), and a child
survives only when the added vertex lies in the automorphism orbit of
the canonically last vertex of the child. The counts are OEIS A006785(n)
(1, 2, 3, 7, 14, 38, 107, 410, ...), or A024607(n)
(1, 1, 1, 3, 6, 19, 59, 267, ...) with ``connected_only`` set.

.. doxygenenum:: graph_recognition::TriangleFreeUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TriangleFreeUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_triangle_free_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled triangle-free
graphs was verified to match `OEIS A213434 <https://oeis.org/A213434>`_:
``2, 7, 41, 388, 5789``. The non-isomorphic enumeration was verified
through ``n = 10`` against `OEIS A006785 <https://oeis.org/A006785>`_
(``1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172``) and, with
``connected_only``, through ``n = 8`` against
`OEIS A024607 <https://oeis.org/A024607>`_
(``1, 1, 1, 3, 6, 19, 59, 267``).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/triangle_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_triangle_free(g);

       std::cout << std::boolalpha << result.is_triangle_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/triangle_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_triangle_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/triangle_free_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_triangle_free_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 14 = A006785(5)
       return 0;
   }


References
----------

* A. Itai, M. Rodeh. "Finding a minimum circuit in a graph."
  *SIAM Journal on Computing*, 7(4):413--423, 1978.
  `DOI:10.1137/0207033 <https://doi.org/10.1137/0207033>`_
* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
