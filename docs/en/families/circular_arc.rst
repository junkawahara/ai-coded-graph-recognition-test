Circular-Arc Graph
==================

Determines whether a graph is a circular-arc graph.
Each vertex corresponds to an arc on a circle,
with edges between overlapping arcs.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_133.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircularArcAlgorithm``
     - Description
   * - ``MCCONNELL`` **(default)**
     - Enumerates the maximal cliques and builds a circular clique ordering.
       Polynomial on Helly circular-arc inputs, but falls back to exponential
       backtracking over clique orderings in the non-Helly case.
   * - ``BACKTRACKING``
     - Searches for an endpoint ordering on the circle via backtracking,
       with 2-SAT pruning. Exponential time; intended for small graphs.

.. doxygenenum:: graph_recognition::CircularArcAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircularArcResult
   :project: graph_recognition
   :members:

The ``BACKTRACKING`` variant constructs the arc model itself and returns it
(``arcs[v] = (start, end)`` covers slots start .. end-1 clockwise on a circle
of 2n slots, so ``start > end`` simply means the arc wraps around). The model
is checked against the graph before it is returned. ``MCCONNELL`` decides
without building a model, so ``arcs`` stays empty there.

.. doxygenfunction:: graph_recognition::check_circular_arc
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CircularArcEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircularArcEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circular_arc_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "circular_arc.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_circular_arc(g);

       std::cout << std::boolalpha << result.is_circular_arc << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "circular_arc_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_circular_arc_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Tucker. "An efficient test for circular-arc graphs."
  *SIAM Journal on Computing*, 9(1):1--24, 1980.
  `DOI:10.1137/0209001 <https://doi.org/10.1137/0209001>`_

* R. M. McConnell. "Linear-time recognition of circular-arc graphs."
  *Algorithmica*, 37(2):93--147, 2003.
  `DOI:10.1007/s00453-003-1032-7 <https://doi.org/10.1007/s00453-003-1032-7>`_
