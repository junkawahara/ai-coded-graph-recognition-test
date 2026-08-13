Bipartite Permutation Graph
===========================

Determines whether a graph is a bipartite permutation graph.
A graph that is both bipartite and a permutation graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_81.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartitePermutationAlgorithm``
     - Description
   * - ``BIPARTITE_AND_PERMUTATION`` **(default)**
     - Runs the bipartite recognizer and then the generic permutation graph
       recognizer, accepting only if both succeed. No chain structure or strong
       ordering is computed.

.. doxygenenum:: graph_recognition::BipartitePermutationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite_permutation
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BipartitePermutationEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_permutation_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "bipartite_permutation.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bipartite_permutation(g);

       std::cout << std::boolalpha << result.is_bipartite_permutation << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_permutation_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_permutation_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs."
  *Discrete Applied Mathematics*, 18(3):279--292, 1987.
  `DOI:10.1016/S0166-218X(87)80003-3 <https://doi.org/10.1016/S0166-218X(87)80003-3>`_

* T. Saitoh, Y. Otachi, K. Yamanaka, R. Uehara. "Random generation and enumeration of bipartite permutation graphs."
  *Journal of Discrete Algorithms*, 10:84--97, 2012.
  `DOI:10.1016/j.jda.2011.12.006 <https://doi.org/10.1016/j.jda.2011.12.006>`_
