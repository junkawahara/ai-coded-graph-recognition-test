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

Both enumerators default to ``EAR_AUGMENTATION``. Stolee's canonical ear
search starts from the cycles ``C_3, ..., C_n`` and adds paths between
existing vertices, using at most ``n`` vertices. A path with no internal
vertices adds a missing edge. All intermediate graphs are biconnected;
only graphs on exactly ``n`` vertices are emitted, with edge additions
continuing after that size is reached.

A shortest ear whose deletion preserves biconnectivity determines the parent,
with ties broken by its canonically labeled endpoints. Canonical parent
comparison and per-parent sibling forms remove isomorphic duplicates.
The labeled API expands representatives into all ``n! / |Aut(G)|`` distinct
labelings. General canonicalization is worst-case factorial: this implementation
does not use EarSearch's nauty or vertex-pair orbits and claims no polynomial
delay. The DFS path and sibling sets are retained along with all outputs;
label expansion additionally retains visited labelings for one representative.

The previous vertex-addition searches remain selectable as ``REVERSE_SEARCH``
(labeled) and ``CANONICAL_AUGMENTATION`` (unlabeled), preserving their output
order and representative labels. They test biconnectivity at the final level.
Both modes accept an inclusive ``max_edges`` bound (third C++ argument,
``-1`` for unrestricted); ear search prunes a partial ``k``-vertex, ``m``-edge
graph when ``k < n`` and ``m + n - k + 1 > max_edges``.

CLI options are ``--algo ear_augmentation|reverse_search`` for labeled graphs,
``--algo ear_augmentation|canonical_augmentation`` for unlabeled graphs, and
``--max-edges M`` for a nonnegative edge bound. Python provides
``enumerate_biconnected_labeled_graphs(n, algorithm=None, max_edges=None)`` and
``enumerate_biconnected_unlabeled_graphs(n, algorithm=None, max_edges=None)``;
``None`` selects ear augmentation and no edge bound. Python's limits are
6 and 8 vertices respectively. C++/CLI accept up to 63 vertices, except for
labeled ``REVERSE_SEARCH``, which retains its limit of 64. All modes emit
nothing for ``n < 3`` (Python requires positive ``n``). There is no
``connected_only`` flag: biconnected graphs are connected by definition.

Non-isomorphic counts are ``1, 3, 10, 56, 468, 7123`` for ``n = 3, ..., 8``
(`OEIS A002218 <https://oeis.org/A002218>`_). Tests compare both algorithms'
classes through ``n = 7``, labeled edge sets through ``n = 6``, and edge bounds
against filtered unrestricted enumeration.

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
   #include "util/graph.h"
   #include "recognizers/biconnected.h"

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
   #include "enumerators/biconnected_labeled_enum.h"

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
   #include "enumerators/biconnected_unlabeled_enum.h"

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

* D. Stolee. "Isomorph-free generation of 2-connected graphs with applications."
  2011. `arXiv:1104.5261 <https://arxiv.org/abs/1104.5261>`_
