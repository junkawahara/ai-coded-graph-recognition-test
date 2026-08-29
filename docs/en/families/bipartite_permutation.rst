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

.. doxygenenum:: graph_recognition::BipartitePermutationLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_permutation_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method — the same scheme as the permutation, circle, chordal,
outerplanar, series-parallel, and cactus enumerators. Graphs are grown
one vertex at a time, which is sound because the class is hereditary
(both bipartiteness and being a permutation graph are preserved by
vertex deletion). The pruning is a ``check_bipartite_permutation`` call
(a bipartiteness test plus the generic permutation graph test) on every
candidate child; it runs before the isomorph rejection, so the more
expensive canonicalization only ever sees bipartite permutation graphs.
The dedicated enumerator of Saitoh, Otachi, Yamanaka and Uehara
generates the connected members through a canonical string encoding
without isomorph rejection, and the BDD-based construction of Kawahara,
Saitoh, Takeda, Yoshinaka and Yoshioka enumerates the non-isomorphic
members in time polynomial in n; this implementation instead reuses the
shared canonical-augmentation machinery, which is practical to about
``n = 10``. The counts are 1, 2, 3, 7, 13, 34, 81, 239, 693, ... for
n = 1, 2, ... (not in the OEIS as of 2026), or the connected ones among
them (1, 1, 1, 3, 5, 16, 38, 126, 375, ...; also not in the OEIS) with
``connected_only`` set.

.. doxygenenum:: graph_recognition::BipartitePermutationUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BipartitePermutationUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_permutation_unlabeled_graphs
   :project: graph_recognition

Count Check
-----------

The non-isomorphic enumeration was verified through ``n = 8``
independently of the enumerator itself: filtering the bipartite
unlabeled enumeration (every bipartite permutation graph is bipartite)
through ``check_bipartite_permutation`` gives the same counts
(``1, 2, 3, 7, 13, 34, 81, 239``; connected
``1, 1, 1, 3, 5, 16, 38, 126``). Neither the total nor the connected
sequence is in the OEIS (checked 2026-08). The static test cases stop
at ``n = 9``.


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
   #include "bipartite_permutation_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_permutation_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_permutation_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_permutation_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 34
       return 0;
   }


References
----------

* J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs."
  *Discrete Applied Mathematics*, 18(3):279--292, 1987.
  `DOI:10.1016/S0166-218X(87)80003-3 <https://doi.org/10.1016/S0166-218X(87)80003-3>`_

* T. Saitoh, Y. Otachi, K. Yamanaka, R. Uehara. "Random generation and enumeration of bipartite permutation graphs."
  *Journal of Discrete Algorithms*, 10:84--97, 2012.
  `DOI:10.1016/j.jda.2011.11.001 <https://doi.org/10.1016/j.jda.2011.11.001>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* J. Kawahara, T. Saitoh, H. Takeda, R. Yoshinaka, Y. Yoshioka. "Efficient non-isomorphic graph enumeration algorithms for several intersection graph classes."
  *Theoretical Computer Science*, 1003:114591, 2024.
  `DOI:10.1016/j.tcs.2024.114591 <https://doi.org/10.1016/j.tcs.2024.114591>`_
