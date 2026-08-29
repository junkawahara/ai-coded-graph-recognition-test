Biconnected Graph
=================

Determines whether a graph is biconnected (2-connected).
A connected graph with at least 3 vertices and no cut vertex.
Graphs with fewer than 3 vertices are reported as not biconnected.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_771.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::BiconnectedAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconnected
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BiconnectedLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method with geng ``-C`` style connectivity constraints. Biconnected
graphs are not hereditary -- deleting a vertex can disconnect the graph
-- so the intermediate levels of the vertex-by-vertex search generate
all graphs. Biconnectivity instead constrains the last two levels: in a
biconnected graph ``G`` the graph ``G - v`` is connected for every
vertex ``v``, so only connected graphs are generated at level ``n - 1``,
and the last vertex must receive degree at least 2 with every vertex of
degree below 2 among its neighbors (the final minimum degree is 2).
These conditions are necessary but not sufficient, so a full cut-vertex
test runs at emission. A child survives only when the added vertex lies
in the automorphism orbit of the canonically last vertex of the child.
Biconnected graphs are connected by definition, so there is no
``connected_only`` flag. The counts were verified through ``n = 9``
against `OEIS A002218 <https://oeis.org/A002218>`_
(``1, 3, 10, 56, 468, 7123, 194066`` for ``n = 3, 4, ...``) and
cross-checked against the canonicalized output of the labeled enumerator
through ``n = 6``. The static test cases stop at ``n = 8``.

.. doxygenenum:: graph_recognition::BiconnectedUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BiconnectedUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_unlabeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "biconnected.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_biconnected(g);

       std::cout << std::boolalpha << result.is_biconnected << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "biconnected_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "biconnected_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 56 = A002218(6)
       return 0;
   }


References
----------

* R. Tarjan. "Depth-first search and linear graph algorithms."
  *SIAM Journal on Computing*, 1(2):146--160, 1972.
  `DOI:10.1137/0201010 <https://doi.org/10.1137/0201010>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
