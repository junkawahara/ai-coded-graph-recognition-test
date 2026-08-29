Ptolemaic Graph
===============

A graph is **ptolemaic** if it is both chordal and distance-hereditary.
Equivalently, the graph is gem-free and chordal.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_95.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - Description
   * - ``DH_HASHMAP``
     - Distance-hereditary check using hash maps
   * - ``DH_SORTED`` **(default)**
     - Distance-hereditary check using sorted adjacency lists

.. doxygenenum:: graph_recognition::PtolemaicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_ptolemaic
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PtolemaicLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ptolemaic_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, from the one-vertex extensions of
Bandelt & Mulder (1986) that generate the distance-hereditary graphs --
adding a pendant vertex, a true twin, or a false twin -- restricted to the
ones that also preserve chordality. Only the false twin needs a restriction:
a false twin of ``v`` closes the induced 4-cycle ``u-a-v-b-u`` for any two
non-adjacent ``a, b`` in ``N(v)``, so it is allowed exactly when ``v`` is
simplicial. The connected members on k vertices are then exactly the
restricted extensions of the connected members on k - 1 vertices, so the
search grows level by level from K1 and rejects isomorphs by a canonical-form
set per level. No recognizer is called anywhere. Disconnected members are
composed from the connected ones -- the class is closed under disjoint union
-- as multisets of components over the integer partitions of n. The counts
are OEIS A287888(n) (1, 1, 2, 5, 14, 47, 170, 676, 2834, ...) with
``connected_only`` set, or its Euler transform
(1, 2, 4, 10, 26, 82, 278, 1053, 4251, ...) without it.

.. doxygenenum:: graph_recognition::PtolemaicUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PtolemaicUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ptolemaic_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

With ``connected_only``, the non-isomorphic enumeration was verified through
``n = 9`` against `OEIS A287888 <https://oeis.org/A287888>`_:
``1, 1, 2, 5, 14, 47, 170, 676, 2834``. The counts including disconnected
members, ``1, 2, 4, 10, 26, 82, 278, 1053, 4251``, were checked through
``n = 7`` against the set of isomorphism classes obtained by canonicalizing
the labeled enumerator's output.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "ptolemaic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_ptolemaic(g);

       std::cout << std::boolalpha << result.is_ptolemaic << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ptolemaic_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ptolemaic_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ptolemaic_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ptolemaic_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 26
       return 0;
   }


References
----------

* E. Howorka. "A characterization of Ptolemaic graphs."
  *Journal of Graph Theory*, 5(3):323--331, 1981.
  `DOI:10.1002/jgt.3190050314 <https://doi.org/10.1002/jgt.3190050314>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_
