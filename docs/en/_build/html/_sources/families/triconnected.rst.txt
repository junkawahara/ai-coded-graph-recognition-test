Triconnected Graph
==================

Determines whether a graph is triconnected (3-connected).
A graph on at least 4 vertices that remains connected after removing any two
vertices (vertex connectivity kappa(G) >= 3).  Graphs on fewer than 4 vertices
are reported as not triconnected.

Recognition
-----------

.. doxygenenum:: graph_recognition::TriconnectedAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriconnectedResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_triconnected
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/triconnected.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_triconnected(g);

       std::cout << std::boolalpha << result.is_triconnected << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

This class does not currently expose a dedicated enumeration function. For very small ``n``, enumerate candidate labeled graphs and filter them with recognition:

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "util/graph.h"
   #include "recognizers/triconnected.h"

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
           if (check_triconnected(g).is_triconnected) {
               ++count;
           }
       }

       std::cout << count << '\n';
       return 0;
   }


References
----------

* J. Hopcroft, R. Tarjan. "Dividing a graph into triconnected components."
  *SIAM Journal on Computing*, 2(3):135--158, 1973.
  `DOI:10.1137/0202012 <https://doi.org/10.1137/0202012>`_
