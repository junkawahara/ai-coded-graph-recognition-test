Proper Interval Graph
=====================

Determines whether a graph is a proper interval graph.
An interval graph representable by a family of intervals with no proper containment.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_298.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ProperIntervalAlgorithm``
     - Description
   * - ``PQ_TREE``
     - Interval graph + claw (K_{1,3}) detection via triple loop. Complexity: O(n * Delta^3).
   * - ``FAST_CLAW_CHECK`` **(default)**
     - Interval graph + claw detection via edge counting.
       Only performs detailed search when N(c) has fewer than d(d-1)/2 edges. Complexity: O(m * Delta).

.. doxygenenum:: graph_recognition::ProperIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_proper_interval
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ProperIntervalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_interval_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "proper_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_interval(g);

       std::cout << std::boolalpha << result.is_proper_interval << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "proper_interval_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_interval_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
