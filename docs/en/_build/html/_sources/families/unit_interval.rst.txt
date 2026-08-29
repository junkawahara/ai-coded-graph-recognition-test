Unit Interval Graph
===================

Determines whether a graph is a unit interval graph.
An interval graph where all intervals have the same length.
By Roberts' theorem, equivalent to proper interval graphs.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_299.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``UnitIntervalAlgorithm``
     - Description
   * - ``PROPER_INTERVAL`` **(default)**
     - Reduces to proper interval graph recognition (Roberts' theorem).

.. doxygenenum:: graph_recognition::UnitIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::UnitIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_unit_interval
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "unit_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_unit_interval(g);

       std::cout << std::boolalpha << result.is_unit_interval << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

This class does not currently expose a dedicated enumeration function. For very small ``n``, enumerate candidate labeled graphs and filter them with recognition:

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "graph.h"
   #include "unit_interval.h"

   int main() {
       using namespace graph_recognition;

       const int n = 4;
       std::vector<std::pair<int, int>> all_edges;
       for (int u = 1; u <= n; ++u) {
           for (int v = u + 1; v <= n; ++v) {
               all_edges.push_back(std::make_pair(u, v));
           }
       }

       int count = 0;
       const int total_masks = 1 << static_cast<int>(all_edges.size());
       for (int mask = 0; mask < total_masks; ++mask) {
           std::vector<std::pair<int, int>> edges;
           for (int i = 0; i < static_cast<int>(all_edges.size()); ++i) {
               if (mask & (1 << i)) {
                   edges.push_back(all_edges[i]);
               }
           }

           Graph g(n, edges);
           if (check_unit_interval(g).is_unit_interval) {
               ++count;
           }
       }

       std::cout << count << '\n';
       return 0;
   }


References
----------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.
