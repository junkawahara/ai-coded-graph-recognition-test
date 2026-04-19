Quasi-Threshold Graph
=====================

A **quasi-threshold graph** is an alternative name for a trivially perfect graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_781.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``QuasiThresholdAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - DFS-based recognition

.. doxygenenum:: graph_recognition::QuasiThresholdAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::QuasiThresholdResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_quasi_threshold
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "quasi_threshold.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_quasi_threshold(g);

       std::cout << std::boolalpha << result.is_quasi_threshold << '\n';
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
   #include "quasi_threshold.h"

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
           if (check_quasi_threshold(g).is_quasi_threshold) {
               ++count;
           }
       }

       std::cout << count << '\n';
       return 0;
   }


References
----------

* E. S. Wolk. "The comparability graph of a tree."
  *Proceedings of the American Mathematical Society*, 13(5):789--795, 1962.
  `DOI:10.1090/S0002-9939-1962-0172273-0 <https://doi.org/10.1090/S0002-9939-1962-0172273-0>`_

* J.-H. Yan, J.-J. Chen, G. J. Chang. "Quasi-threshold graphs."
  *Discrete Applied Mathematics*, 69(3):247--255, 1996.
  `DOI:10.1016/0166-218X(96)00094-7 <https://doi.org/10.1016/0166-218X(96)00094-7>`_
