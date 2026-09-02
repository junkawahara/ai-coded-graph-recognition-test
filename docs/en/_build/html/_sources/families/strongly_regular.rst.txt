Strongly Regular Graph
======================

Determines whether a graph is strongly regular with parameters (n, k, lambda, mu).
A k-regular graph where every pair of adjacent vertices has exactly lambda common neighbors
and every pair of non-adjacent vertices has exactly mu common neighbors.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1185.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::StronglyRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_regular
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::StronglyRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, following the McKay--Spence
recipe: one exhaustive search per feasible parameter tuple
``(k, lambda, mu)`` (prefiltered by the counting identity
``k(k - lambda - 1) = mu(n - k - 1)`` and the eigenvalue-integrality
conditions, shared with the labeled enumerator), with isomorph
rejection inside each search by McKay's canonical construction path on
the shared canonical-augmentation machinery. Strongly regular graphs
are not hereditary, but every induced subgraph of an
``srg(n, k, lambda, mu)`` satisfies the k-regular degree-deficit
conditions plus a common-neighbor window per vertex pair (the count
never exceeds its target ``lambda`` / ``mu``, and each future common
neighbor consumes one unit of both remaining degree deficits), and
those necessary conditions drive the pruning; at the last level they
force every degree and every pairwise common-neighbor count exactly,
so each graph reaching level ``n`` is strongly regular by
construction. No rejection is needed across parameter tuples: a
strongly regular graph determines ``(k, lambda, mu)`` uniquely, so
distinct searches emit disjoint sets of classes.

.. doxygenenum:: graph_recognition::StronglyRegularUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::StronglyRegularUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_unlabeled_graphs
   :project: graph_recognition

Count Check
-----------

The non-isomorphic enumeration reproduces the known classification of
small strongly regular graphs: ``2, 1, 4, 0, 4, 3, 6, 0, 8, 1, 4, 6``
classes for ``n = 4, ..., 15`` (there is no single OEIS sequence; the
counts are parameter-dependent). At ``n = 10`` the six classes are the
Petersen graph, its complement, and the four imprimitive classes
``2K5``, ``5K2`` and their complements; ``n = 13`` yields exactly the
Paley graph. Through ``n = 7`` the output was cross-checked against
the canonicalized labeled enumerator's isomorphism classes. The static
test cases stop at ``n = 13`` (the enumeration is practical to about
``n = 15``).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/strongly_regular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       auto result = check_strongly_regular(g);

       std::cout << std::boolalpha << result.is_strongly_regular << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/strongly_regular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_regular_labeled_graphs(5);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/strongly_regular_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_regular_unlabeled_graphs(10);
       std::cout << result.graphs.size() << '\n';  // 6 (Petersen among them)
       return 0;
   }


References
----------

* R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs."
  *Pacific Journal of Mathematics*, 13(2):389--419, 1963.
  `DOI:10.2140/pjm.1963.13.389 <https://doi.org/10.2140/pjm.1963.13.389>`_

* B. D. McKay, E. Spence. "Classification of regular two-graphs on 36 and 38 vertices."
  *Australasian Journal of Combinatorics*, 24:293--300, 2001.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
