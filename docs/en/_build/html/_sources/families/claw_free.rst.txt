Claw-Free Graph
===============

Determines whether a graph is claw-free (K_{1,3}-free).
No induced subgraph isomorphic to K_{1,3} (claw).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_62.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClawFreeAlgorithm``
     - Description
   * - ``TRIPLE_LOOP``
     - Searches for an independent set of size 3 in each vertex's neighborhood.
       Complexity: O(n * Delta^3).
   * - ``EDGE_COUNT`` **(default)**
     - Uses edge counting to test neighborhood completeness;
       performs detailed search only when incomplete.  The filter itself is
       O(m * Delta), but the search over an incomplete neighborhood costs
       O(deg^3), so the worst case is O(n * Delta^3).

.. doxygenenum:: graph_recognition::ClawFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClawFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_claw_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ClawFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClawFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_claw_free_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "claw_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_claw_free(g);

       std::cout << std::boolalpha << result.is_claw_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "claw_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_claw_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* R. Faudree, E. Flandrin, Z. Ryjáček. "Claw-free graphs --- A survey."
  *Discrete Mathematics*, 164(1--3):87--147, 1997.
  `DOI:10.1016/S0012-365X(96)00045-3 <https://doi.org/10.1016/S0012-365X(96)00045-3>`_
