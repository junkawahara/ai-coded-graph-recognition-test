Trapezoid Graph
===============

Determines whether a graph is a trapezoid graph.
Vertices correspond to trapezoids between two horizontal lines,
with edges between overlapping trapezoids.
Equivalent to being a co-comparability graph whose corresponding
partial order has interval dimension at most 2.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_59.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TrapezoidAlgorithm``
     - Description
   * - ``CHAIN_COVER`` **(default)**
     - Based on Cogis (1982) characterization.
       (1) Determines co-comparability via transitive orientation of the complement.
       (2) Constructs the bipartite graph B(P) of the corresponding partial order P
       (edge (x,y) iff NOT x <_P y), then checks bipartiteness of the incompatibility
       graph I(B) (edges from 2+2 patterns) via BFS.
       Trivial 2K_2 (fewer than 4 distinct elements) are excluded.

.. doxygenenum:: graph_recognition::TrapezoidAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TrapezoidResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_trapezoid
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::TrapezoidEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TrapezoidEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trapezoid_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "trapezoid.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_trapezoid(g);

       std::cout << std::boolalpha << result.is_trapezoid << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "trapezoid_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_trapezoid_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* I. Dagan, M. C. Golumbic, R. Y. Pinter. "Trapezoid graphs and their coloring."
  *Discrete Applied Mathematics*, 21(1):35--46, 1988.
  `DOI:10.1016/0166-218X(88)90032-7 <https://doi.org/10.1016/0166-218X(88)90032-7>`_

* O. Cogis. "On the Ferrers dimension of a digraph."
  *Discrete Mathematics*, 38(1):47--52, 1982.
  `DOI:10.1016/0012-365X(82)90167-4 <https://doi.org/10.1016/0012-365X(82)90167-4>`_

* T.-H. Ma, J. P. Spinrad. "On the 2-chain subgraph cover and related problems."
  *Journal of Algorithms*, 17(2):251--268, 1994.
  `DOI:10.1006/jagm.1994.1034 <https://doi.org/10.1006/jagm.1994.1034>`_
