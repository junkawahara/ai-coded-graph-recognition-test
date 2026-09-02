Co-Comparability Graph
======================

Determines whether a graph is a co-comparability graph.
A graph whose complement is a comparability graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_147.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoComparabilityAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies comparability recognition.

.. doxygenenum:: graph_recognition::CoComparabilityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoComparabilityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_comparability
   :project: graph_recognition


Enumeration
-----------

The current enumerator uses a labeled vertex-extension search.  The public
function name retains the historical ``reverse_search`` suffix, but the search
tree is defined by the parent map that deletes the largest-labeled vertex.
Starting from the empty graph, vertices ``1, 2, ..., n`` are added in order; for
each new vertex, every subset of earlier vertices is tried as its neighborhood
and the candidate is filtered by ``check_co_comparability``.  Since
co-comparability graphs are hereditary, every labeled co-comparability graph is
reached exactly once by deleting vertices in decreasing label order.

.. doxygenenum:: graph_recognition::CoComparabilityLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoComparabilityLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_comparability_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. The other enumerator emits one
representative per isomorphism class: it enumerates the non-isomorphic
comparability graphs (McKay canonical construction path with
``check_comparability`` pruning) and outputs the complement of each.
Complementation commutes with relabeling, so it is a bijection on
isomorphism classes, and the counts equal the comparability counts,
OEIS A123416.

.. doxygenstruct:: graph_recognition::CoComparabilityUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CoComparabilityUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_comparability_unlabeled_graphs
   :project: graph_recognition


OEIS Count Check
----------------

For the non-isomorphic enumeration, the counts were checked against
`OEIS A123416 <https://oeis.org/A123416>`_ (the comparability counts;
``1, 2, 4, 11, 33, 144, 824, 6793``) up to ``n = 8``, and the output was
checked to coincide with the set of isomorphism classes obtained by
canonicalizing the labeled enumerator's output up to ``n = 6``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/co_comparability.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_comparability(g);

       std::cout << std::boolalpha << result.is_co_comparability << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_comparability_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_comparability_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_comparability_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_comparability_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 144 = A123416(6)
       return 0;
   }


References
----------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* D. Avis, K. Fukuda. "Reverse search for enumeration."
  *Discrete Applied Mathematics*, 65(1--3):21--46, 1996.
  `DOI:10.1016/0166-218X(95)00026-N <https://doi.org/10.1016/0166-218X(95)00026-N>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* R. H. Möhring. "Almost all comparability graphs are UPO."
  *Discrete Mathematics*, 50:63--70, 1984.
  `DOI:10.1016/0012-365X(84)90035-6 <https://doi.org/10.1016/0012-365X(84)90035-6>`_
