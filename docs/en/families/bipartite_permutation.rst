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

Both enumerators default to ``STRING_REPRESENTATION``. The connected
backend uses the endpoint-swap reverse search of Saitoh et al. (Section 4):
for each bipartition size ``p >= q``, start from ``K_{p,q}`` and invert the
parent at up to three nominated positions. Strict positive prefix heights
test connectivity. Canonical line strings remove reflection, line-exchange
and rotation symmetries without a recognizer or general graph canonicalizer.
Canonicity filters emission only: noncanonical orientations may have canonical
descendants and must remain searchable.

Disconnected graphs are multisets of connected representatives. Component
sizes are nonincreasing; indices for equal-sized components are nondecreasing,
so repeated isomorphic components introduce no duplicates. The labeled API
expands each representative into all ``n! / |Aut(G)|`` distinct labelings
using adjacent label swaps and visited edge sets.

The previous algorithms remain available as
``BipartitePermutationLabeledEnumAlgorithm::REVERSE_SEARCH`` and
``BipartitePermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION``.
They retain recognizer-pruned vertex additions and their original output order;
the unlabeled legacy mode also uses McKay's canonical-parent test. The default
can change output order and the labelings of unlabeled representatives.

The CLI takes ``--algo string_representation`` (default), or
``--algo reverse_search`` (labeled) / ``--algo canonical_augmentation``
(unlabeled). The unlabeled CLI also accepts ``--connected`` with either mode.
Python exposes the same names through ``algorithm=``; the unlabeled function
keeps ``connected_only=False`` as its second positional argument.

C++/CLI retain the existing vertex limits: 64 labeled, 63 unlabeled.
Both include the empty graph at ``n = 0``, including with ``connected_only``.
Python accepts ``1 <= n <= 6`` labeled and ``1 <= n <= 8`` unlabeled.

String scans cost O(n), edge construction O(n²), and the connected search uses
O(n²) amortized time per representative and O(n²) search space, excluding
output. Component representatives, visited labelings and all results are kept
in memory. Constant-time updates, prepostorder traversal and jumps between
bipartition sizes are not implemented; the paper's O(1) delay does not apply.
The non-isomorphic counts are ``1, 2, 3, 7, 13, 34, 81, 239, 693, ...`` for
``n = 1, 2, ...``, or ``1, 1, 1, 3, 5, 16, 38, 126, 375, ...`` with
``connected_only``.

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
   #include "util/graph.h"
   #include "recognizers/bipartite_permutation.h"

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
   #include "enumerators/bipartite_permutation_labeled_enum.h"

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
   #include "enumerators/bipartite_permutation_unlabeled_enum.h"

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
