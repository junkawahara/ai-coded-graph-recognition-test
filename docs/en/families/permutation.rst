Permutation Graph
=================

Determines whether a graph is a permutation graph.
For a permutation pi, vertices i and j are adjacent iff their order is reversed by pi.
Equivalent to both G and complement(G) being comparability graphs.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_23.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PermutationAlgorithm``
     - Description
   * - ``BACKTRACKING``
     - Searches for transitive orientations of both G and complement(G) via backtracking.
   * - ``CLASS_BASED`` **(default)**
     - Checks orientation consistency per Gamma class (edge equivalence class).
       Greedily selects unoriented edges and propagates constraints within each class.

.. doxygenenum:: graph_recognition::PermutationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_permutation
   :project: graph_recognition

Permutation diagram construction
----------------------------------

``build_permutation_realizer()`` constructs the diagram itself (Pnueli,
Lempel & Even 1971; Golumbic, *Algorithmic Graph Theory and Perfect Graphs*,
Ch. 7). If F1 transitively orients G and F2 orients its complement, then
F1 union F2 is a transitive tournament, and so is F1-reversed union F2;
reading a vertex's rank off each tournament places it on the two lines.

The ranks are taken as in-degrees rather than by sorting with a comparator: a
transitive tournament on n vertices has in-degrees exactly 0..n-1, so the
in-degrees already are the positions.

.. doxygenstruct:: graph_recognition::PermutationRealizerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_permutation_realizer
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PermutationEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_permutation_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "permutation.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_permutation(g);

       std::cout << std::boolalpha << result.is_permutation << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "permutation_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_permutation_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Pnueli, A. Lempel, S. Even. "Transitive orientation of graphs and identification of permutation graphs."
  *Canadian Journal of Mathematics*, 23(1):160--175, 1971.
  `DOI:10.4153/CJM-1971-016-5 <https://doi.org/10.4153/CJM-1971-016-5>`_

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_
