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

Both enumerators default to ``INDEPENDENT_SETS``. A new vertex preserves
triangle-freeness exactly when its neighborhood is an independent set.
The shared generator branches on including or excluding a vertex, removing
its neighbors from the remaining candidates when it is included. This emits
every independent set once, including the empty set and non-maximal sets,
without scanning all subsets or calling a recognizer. Neighborhood generation
uses O(k) stack space and O(k) work per mask on a k-vertex parent, excluding
the child search, canonicalization and output construction.

The labeled version adds vertices in label order, with the largest-label
deletion as its unique parent rule. The former all-subsets scan with recognition
is available as ``TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH``.
The unlabeled version retains the all-subsets scan with an independence test
as ``TriangleFreeUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION``.
Both new algorithms preserve the old output order and representative labelings
by generating neighborhood masks in ascending order.

The CLIs accept ``--algo independent_sets`` (default), or ``--algo reverse_search``
for labeled graphs and ``--algo canonical_augmentation`` for unlabeled graphs.
The unlabeled CLI still accepts ``--connected``. The existing Python function
``enumerate_triangle_free_unlabeled_graphs(n, connected_only=False, algorithm=None)``
accepts ``"independent_sets"`` and ``"canonical_augmentation"``; ``None`` selects
the new default, and the range remains ``1 <= n <= 8``.

C++ and the CLI retain ``0 <= n <= 64`` for labeled graphs and ``0 <= n <= 63``
for unlabeled graphs. At n = 0 both modes emit one empty graph, including with
``connected_only``. All outputs and the unlabeled search's per-parent child
canonical forms remain in memory; canonicalization still limits practical sizes.

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
set, and a child
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
