Comparability Graph
===================

Determines whether a graph is a comparability graph.
A graph whose edges can be transitively oriented.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_72.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - Description
   * - ``TRANSITIVE_ORIENTATION`` **(default)**
     - Backtracking transitive orientation solver using internal functions from permutation.h.

.. doxygenenum:: graph_recognition::ComparabilityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_comparability
   :project: graph_recognition


Enumeration
-----------

The current enumerator uses a labeled vertex-extension search.  The public
function name retains the historical ``reverse_search`` suffix, but the search
tree is defined by the parent map that deletes the largest-labeled vertex.
Starting from the empty graph, vertices ``1, 2, ..., n`` are added in order; for
each new vertex, every subset of earlier vertices is tried as its neighborhood
and the candidate is filtered by ``check_comparability``.  Since comparability
graphs are hereditary, every labeled comparability graph is reached exactly once
by deleting vertices in decreasing label order.

.. doxygenenum:: graph_recognition::ComparabilityEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_comparability_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "comparability.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_comparability(g);

       std::cout << std::boolalpha << result.is_comparability << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "comparability_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_comparability_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* M. C. Golumbic. *Algorithmic Graph Theory and Perfect Graphs.*
  Academic Press, 1980; 2nd edition, Annals of Discrete Mathematics 57, Elsevier, 2004.
  `DOI:10.1016/S0167-5060(04)80053-0 <https://doi.org/10.1016/S0167-5060(04)80053-0>`_

* D. Avis, K. Fukuda. "Reverse search for enumeration."
  *Discrete Applied Mathematics*, 65(1--3):21--46, 1996.
  `DOI:10.1016/0166-218X(95)00026-N <https://doi.org/10.1016/0166-218X(95)00026-N>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
