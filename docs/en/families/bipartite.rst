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

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled bipartite graphs
was verified to match `OEIS A047864 <https://oeis.org/A047864>`_:
``2, 7, 41, 376, 5177``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "bipartite.h"

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
   #include "bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* D. König. *Theorie der endlichen und unendlichen Graphen.*
  Akademische Verlagsgesellschaft, Leipzig, 1936.
