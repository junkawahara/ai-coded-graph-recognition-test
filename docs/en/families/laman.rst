Laman Graph
===========

Determines whether a graph is a Laman graph.
A graph satisfying the Laman condition: m = 2n - 3 and every subgraph on k vertices
has at most 2k - 3 edges. Laman graphs characterize minimally rigid frameworks in the plane.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1206.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::LamanLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, following the same route as the
nauty-laman-plugin: McKay's canonical construction path growing through
(2,3)-sparse graphs. Laman graphs themselves are not closed under vertex
deletion (the edge count 2n - 3 breaks), but (2,3)-sparsity is
hereditary, so the intermediate levels range over the (2,3)-sparse
graphs and tightness (m = 2n - 3) filters at emission. Candidate
children are pruned by the sparsity edge bound, a tightness-reachability
bound (each future vertex adds at most one edge per existing vertex),
a minimum-degree-2 feasibility check on the last two levels, and the
incremental (2,3)-sparsity subset check restricted to the subsets
containing the new vertex. Laman graphs are always connected, so there
is no ``connected_only`` flag.

.. doxygenenum:: graph_recognition::LamanUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

The non-isomorphic enumeration was verified through ``n = 9`` against
`OEIS A227117 <https://oeis.org/A227117>`_
(``1, 1, 1, 1, 3, 13, 70, 608, 7222`` for ``n = 1, ..., 9``; the
``n = 10`` output also matches the 110132 of the sequence). It was also
cross-checked against the canonicalized output of the labeled enumerator
through ``n = 6``. The static test cases stop at ``n = 9`` (``n = 10``
takes about 90 seconds).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "laman.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}});
       auto result = check_laman(g);

       std::cout << std::boolalpha << result.is_laman << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "laman_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "laman_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 13
       return 0;
   }


References
----------

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
