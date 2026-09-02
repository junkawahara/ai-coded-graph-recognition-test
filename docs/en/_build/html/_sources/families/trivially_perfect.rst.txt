Trivially Perfect Graph
=======================

A graph is **trivially perfect** if it is both chordal and a cograph.
Equivalently, for every connected induced subgraph, the graph has a universal
vertex; or equivalently, the adjacency relation coincides with the
ancestor-descendant relation in a DFS tree.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_327.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TriviallyPerfectAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - DFS-based recognition

.. doxygenenum:: graph_recognition::TriviallyPerfectAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_trivially_perfect
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::TriviallyPerfectLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trivially_perfect_labeled_graphs_uvd
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, through the classical bijection with
rooted forests (Wolk): a rooted tree yields a connected trivially perfect
graph by joining every vertex to all of its ancestors, and the unique
universal vertex decomposition inverts the map, so closing the canonical
rooted trees of ``tree_unlabeled_enum.h`` and composing components over
integer partitions covers each class exactly once. The counts are OEIS
A000081(n+1) (1, 2, 4, 9, 20, 48, 115, 286, ...), or A000081(n)
(1, 1, 2, 4, 9, 20, 48, 115, ...) with ``connected_only`` set.

.. doxygenenum:: graph_recognition::TriviallyPerfectUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TriviallyPerfectUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trivially_perfect_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled trivially
perfect graphs was verified to match `OEIS A058864
<https://oeis.org/A058864>`_: ``1, 2, 8, 49, 402``. The non-isomorphic
enumeration was verified through ``n = 9`` against `OEIS A000081
<https://oeis.org/A000081>`_ shifted by one (``1, 2, 4, 9, 20, 48, 115,
286, 719``), and against ``A000081`` itself (``1, 1, 2, 4, 9, 20, 48,
115``) with ``connected_only`` set.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/trivially_perfect.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_trivially_perfect(g);

       std::cout << std::boolalpha << result.is_trivially_perfect << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/trivially_perfect_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_trivially_perfect_labeled_graphs_uvd(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* E. S. Wolk. "The comparability graph of a tree."
  *Proceedings of the American Mathematical Society*, 13(5):789--795, 1962.
  `DOI:10.1090/S0002-9939-1962-0172273-0 <https://doi.org/10.1090/S0002-9939-1962-0172273-0>`_

* M. C. Golumbic. "Trivially perfect graphs."
  *Discrete Mathematics*, 24(1):105--107, 1978.
  `DOI:10.1016/0012-365X(78)90178-4 <https://doi.org/10.1016/0012-365X(78)90178-4>`_

* T. Beyer, S. M. Hedetniemi. "Constant time generation of rooted trees."
  *SIAM Journal on Computing*, 9(4):706--712, 1980.
  `DOI:10.1137/0209055 <https://doi.org/10.1137/0209055>`_
