Even-Hole-Free Graph
====================

An **even-hole-free graph** contains no induced even cycle of length four
or more.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_547.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

The recognition function ``check_even_hole_free`` does not accept an algorithm
parameter. For each edge (u, v), the implementation searches for odd-length
induced paths between N(u) and N(v) restricted to the subgraph outside
N[u] ∪ N[v], which completes an even hole together with u-v.

.. doxygenstruct:: graph_recognition::EvenHoleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_even_hole_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::EvenHoleFreeEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EvenHoleFreeEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_even_hole_free_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "even_hole_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_even_hole_free(g);

       std::cout << std::boolalpha << result.is_even_hole_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "even_hole_free_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_even_hole_free_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Conforti, G. Cornuéjols, A. Kapoor, K. Vušković. "Even-hole-free graphs, Part I: Decomposition theorem."
  *Journal of Graph Theory*, 39(1):6--49, 2002.
  `DOI:10.1002/jgt.10006 <https://doi.org/10.1002/jgt.10006>`_

* M. V. G. da Silva, K. Vušković. "Decomposition of even-hole-free graphs with star cutsets and 2-joins."
  *Journal of Combinatorial Theory, Series B*, 103(1):144--183, 2013.
  `DOI:10.1016/j.jctb.2012.09.001 <https://doi.org/10.1016/j.jctb.2012.09.001>`_
