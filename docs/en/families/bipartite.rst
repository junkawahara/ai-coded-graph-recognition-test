Bipartite Graph
===============

Determines whether a graph is bipartite.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_69.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - Description
   * - ``BFS`` **(default)**
     - Attempts a 2-coloring via BFS and reports failure on the first color
       conflict. A bipartite graph is reported with its 2-coloring; a
       non-bipartite one with the odd cycle closing the conflicting edge.
       Complexity: O(n + m).

.. doxygenenum:: graph_recognition::BipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BipartiteLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method (the isomorph rejection scheme behind ``geng`` / ``genbg``):
graphs are grown one vertex at a time, which is sound because
bipartiteness is hereditary. The pruning is a 2-coloring test rather than
a recognizer call -- a connected bipartite graph has exactly one
2-coloring up to swapping its two sides, so a new vertex's neighborhood
keeps the graph bipartite exactly when it meets every component in only
one side of that component's bipartition. A child then survives only when
the added vertex lies in the automorphism orbit of the canonically last
vertex of the child. The counts are OEIS A033995(n)
(1, 2, 3, 7, 13, 35, 88, 303, ...), or A005142(n)
(1, 1, 1, 3, 5, 17, 44, 182, ...) with ``connected_only`` set.

.. doxygenenum:: graph_recognition::BipartiteUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BipartiteUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled bipartite graphs
was verified to match `OEIS A047864 <https://oeis.org/A047864>`_:
``2, 7, 41, 376, 5177``. The non-isomorphic enumeration was verified
through ``n = 10`` against `OEIS A033995 <https://oeis.org/A033995>`_
(``1, 2, 3, 7, 13, 35, 88, 303, 1119, 5479``) and, with
``connected_only``, through ``n = 8`` against
`OEIS A005142 <https://oeis.org/A005142>`_
(``1, 1, 1, 3, 5, 17, 44, 182``).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bipartite(g);

       std::cout << std::boolalpha << result.is_bipartite << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/bipartite_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 13 = A033995(5)
       return 0;
   }


References
----------

* D. König. *Theorie der endlichen und unendlichen Graphen.*
  Akademische Verlagsgesellschaft, Leipzig, 1936.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
