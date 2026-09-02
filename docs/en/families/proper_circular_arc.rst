Proper Circular-Arc Graph
=========================

Determines whether a graph is a proper circular-arc graph.
A circular-arc graph representable by arcs with no proper containment.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_297.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenstruct:: graph_recognition::ProperCircularArcResult
   :project: graph_recognition
   :members:

``arcs`` reports a proper arc model -- one where no arc contains another --
using the same encoding as ``CircularArcResult::arcs``.

.. doxygenfunction:: graph_recognition::check_proper_circular_arc
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ProperCircularArcLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperCircularArcLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_circular_arc_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/proper_circular_arc.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_circular_arc(g);

       std::cout << std::boolalpha << result.is_proper_circular_arc << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/proper_circular_arc_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_circular_arc_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Tucker. "Structure theorems for some circular-arc graphs."
  *Discrete Mathematics*, 7(1--2):167--195, 1974.
  `DOI:10.1016/S0012-365X(74)80027-0 <https://doi.org/10.1016/S0012-365X(74)80027-0>`_

* X. Deng, P. Hell, J. Huang. "Linear-time representation algorithms for proper circular-arc graphs and proper interval graphs."
  *SIAM Journal on Computing*, 25(2):390--403, 1996.
  `DOI:10.1137/S0097539792269095 <https://doi.org/10.1137/S0097539792269095>`_
