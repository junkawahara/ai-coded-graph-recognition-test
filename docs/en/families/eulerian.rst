Eulerian Graph
==============

Determines whether a graph is Eulerian.
A connected graph where every vertex has even degree.

Recognition
-----------

.. doxygenenum:: graph_recognition::EulerianAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_eulerian
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::EulerianEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_eulerian_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number returned by the Eulerian
enumerator was verified to match the row sums of `OEIS A058878
<https://oeis.org/A058878>`_ (equivalently `OEIS A006125
<https://oeis.org/A006125>`_ with the index shifted by one):
``1, 1, 2, 8, 64, 1024``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "eulerian.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_eulerian(g);

       std::cout << std::boolalpha << result.is_eulerian << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "eulerian_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_eulerian_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* L. Euler. "Solutio problematis ad geometriam situs pertinentis."
  *Commentarii Academiae Scientiarum Petropolitanae*, 8:128--140, 1741
  (presented 1736).
