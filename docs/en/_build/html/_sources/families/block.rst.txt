Block Graph
===========

A graph is a **block graph** if every biconnected component is a clique.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_93.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - DFS-based biconnected component check, O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - Chordal + diamond-free recognition, O(n + m Delta^2)

.. doxygenenum:: graph_recognition::BlockAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_block
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BlockLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/block.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_block(g);

       std::cout << std::boolalpha << result.is_block << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/block_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_block_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary. "A characterization of block-graphs."
  *Canadian Mathematical Bulletin*, 6(1):1--6, 1963.
  `DOI:10.4153/CMB-1963-001-x <https://doi.org/10.4153/CMB-1963-001-x>`_
