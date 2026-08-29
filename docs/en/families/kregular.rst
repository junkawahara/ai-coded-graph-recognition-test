K-Regular Graph
===============

Determines whether a graph is k-regular.
A graph where every vertex has the same degree k.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1149.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::KRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_kregular
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::KRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_kregular_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method with the degree constraint driving the search (GENREG's
orderly-generation strategy on the shared canonical-augmentation
machinery; the degree-parameterized generalization of the cubic
unlabeled enumerator). k-regular graphs are not hereditary, but every
induced subgraph of a k-regular graph has maximum degree at most k, so
the intermediate levels of the vertex-by-vertex search range over the
graphs with maximum degree at most k: the new vertex's neighborhood
runs only over the at-most-k-subsets of the vertices of degree less
than k, further pruned by necessary completability conditions (with
``r`` vertices still to come, every degree deficit is at most ``r``,
their sum is at most ``kr``, and ``kr`` minus the sum is even), which
force every graph reaching the last level to be k-regular. A child
survives only when the added vertex lies in the automorphism orbit of
the canonically last vertex of the child.

.. doxygenenum:: graph_recognition::KRegularUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::KRegularUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_kregular_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

The non-isomorphic enumeration was verified against the triangle
`OEIS A051031 <https://oeis.org/A051031>`_ (row n, column k), including
the complement symmetry between columns ``k`` and ``n-1-k`` within each
row: at ``k = 2`` the counts are the partitions of ``n`` into parts at
least 3 (disjoint unions of cycles), at ``k = 3`` they reproduce
`OEIS A005638 <https://oeis.org/A005638>`_ (agreeing with the dedicated
cubic enumerator through ``n = 10``), and at ``k = 4`` they match
`OEIS A033301 <https://oeis.org/A033301>`_ through ``n = 12``
(``1, 1, 2, 6, 16, 60, 266, 1547`` for ``n = 5, ..., 12``). With
``connected_only`` the ``k = 4`` counts match
`OEIS A006820 <https://oeis.org/A006820>`_ (``1, 1, 2, 6, 16, 59, 265``
for ``n = 5, ..., 11``). Through ``n = 6`` the output was cross-checked
against the canonicalized labeled enumerator for every
``0 <= k < n``. The static test cases stop at ``n = 10``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "kregular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_kregular(g);

       std::cout << std::boolalpha << result.is_kregular << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "kregular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_kregular_labeled_graphs_reverse_search(4, 2);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "kregular_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_kregular_unlabeled_graphs(9, 4);
       std::cout << result.graphs.size() << '\n';  // 16
       return 0;
   }


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_

* M. Meringer. "Fast generation of regular graphs and construction of cages."
  *Journal of Graph Theory*, 30(2):137--146, 1999.
  `DOI:10.1002/(SICI)1097-0118(199902)30:2\<137::AID-JGT7\>3.0.CO;2-G <https://doi.org/10.1002/(SICI)1097-0118(199902)30:2%3C137::AID-JGT7%3E3.0.CO;2-G>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
