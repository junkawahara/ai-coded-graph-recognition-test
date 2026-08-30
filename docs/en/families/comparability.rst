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

.. doxygenenum:: graph_recognition::ComparabilityLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_comparability_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method — the same scheme as the chordal enumerator. Graphs are
grown one vertex at a time, which is sound because comparability graphs
are hereditary: restricting a transitive orientation to an induced
subgraph keeps it transitive. The pruning is a ``check_comparability``
call (Golumbic's O(nm) Gamma-class transitive orientation) on every
candidate child; it runs before the isomorph rejection, so the more
expensive canonicalization only ever sees comparability graphs. The
counts are OEIS A123416(n) (1, 2, 4, 11, 33, 144, 824, 6793, 75400,
...), or the connected ones among them (1, 1, 2, 6, 20, 101, 646, 5797,
...; not in the OEIS) with ``connected_only`` set.

.. doxygenenum:: graph_recognition::ComparabilityUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ComparabilityUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_comparability_unlabeled_graphs
   :project: graph_recognition


OEIS Count Check
----------------

The non-isomorphic enumeration was verified through ``n = 9`` against
`OEIS A123416 <https://oeis.org/A123416>`_ (number of comparability
graphs: ``1, 2, 4, 11, 33, 144, 824, 6793, 75400``); the ``n = 8`` and
``n = 9`` terms are independent published computations. Up to ``n = 6``
the output was also checked to coincide with the set of isomorphism
classes obtained by canonicalizing the labeled enumerator's output. The
static test cases stop at ``n = 8``.


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
   #include "comparability_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_comparability_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "comparability_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_comparability_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 144 = A123416(6)
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

* R. H. Möhring. "Almost all comparability graphs are UPO."
  *Discrete Mathematics*, 50:63--70, 1984.
  `DOI:10.1016/0012-365X(84)90035-6 <https://doi.org/10.1016/0012-365X(84)90035-6>`_
