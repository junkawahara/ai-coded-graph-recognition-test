Poset (Partially Ordered Set)
=============================

Determines whether a directed graph is the **Hasse diagram** (covering
relation) of a partially ordered set.
The input is interpreted as the covering relation, so an arc ``u -> v``
means ``v`` covers ``u`` (``u < v``); the underlying partial order is the
reflexive transitive closure. A valid Hasse diagram is therefore a directed
acyclic graph that is its own transitive reduction. The partial order
itself (with reflexive loops or transitive arcs) must **not** be given
as input.

Recognition
-----------

.. doxygenenum:: graph_recognition::PosetAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PosetResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_poset
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::PosetLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PosetLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_posets
   :project: graph_recognition

The enumerator above emits labeled posets. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method (the nauty suite obtains the same lists via ``genposetg``,
which builds Hasse diagrams level by level). Posets are grown one
element at a time (deleting any element of a poset again yields a
poset, so every class is reachable), and the object grown and
canonicalized is the strict-order digraph — the full transitive
closure, not the Hasse diagram, whose vertex deletions do not commute
with taking sub-posets. Each child adds a new element with an up-set of
successors and a disjoint down-set of predecessors whose every member
lies below every successor; exactly those pairs keep the digraph a
transitively closed DAG, so no recognition filter is needed. Isomorph
rejection reuses the genuinely directed canonical form of the digraph
enumerator. Output is the Hasse diagram of each poset, matching the
labeled enumerator. Practical to about ``n = 9`` (183231 classes,
about 33 s). There is no ``connected_only`` flag, matching the labeled
enumerator.

.. doxygenenum:: graph_recognition::PosetUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PosetUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PosetUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_poset_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled posets was
verified to match `OEIS A001035 <https://oeis.org/A001035>`_:
``1, 3, 19, 219, 4231``.

The non-isomorphic enumeration reproduces
`OEIS A000112 <https://oeis.org/A000112>`_ (posets with n unlabeled
elements): ``1, 2, 5, 16, 63, 318, 2045, 16999`` for ``n = 1, ..., 8``
in the static test cases (``n = 8`` about 1.3 s), plus ``183231`` at
``n = 9`` (about 33 s) checked by hand. For ``n <= 5`` the
canonicalized output of the labeled enumerator yields exactly the same
isomorphism classes.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "recognizers/poset.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 3}};
       auto result = check_poset(3, arcs);

       std::cout << std::boolalpha << result.is_poset << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/poset_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_posets(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/poset_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_poset_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';  // 16
       return 0;
   }


References
----------

* R. P. Dilworth. "A decomposition theorem for partially ordered sets."
  *Annals of Mathematics*, 51(1):161--166, 1950.
  `DOI:10.2307/1969503 <https://doi.org/10.2307/1969503>`_

* G. Brinkmann, B. D. McKay. "Posets on up to 16 Points."
  *Order*, 19(2):147--179, 2002.
  `DOI:10.1023/A:1016543307592 <https://doi.org/10.1023/A:1016543307592>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
