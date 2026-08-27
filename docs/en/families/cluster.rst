Cluster Graph
=============

A **cluster graph** is a disjoint union of complete graphs.
Equivalently, it is P3-free (contains no induced path on 3 vertices).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1237.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClusterAlgorithm``
     - Description
   * - ``COMPONENT_CLIQUE`` **(default)**
     - Checks that every connected component is a clique.

.. doxygenenum:: graph_recognition::ClusterAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClusterResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cluster
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::ClusterLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ClusterLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cluster_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class. The cliques of a cluster graph are
exactly its connected components, so the multiset of clique sizes is a
complete isomorphism invariant: the classes on n vertices are in bijection
with the integer partitions of n, and walking those partitions in
non-increasing order with one clique per part needs no isomorph rejection.
The counts are OEIS A000041(n) (1, 2, 3, 5, 7, 11, 15, 22, ...). With
``connected_only`` set the output is the single complete graph K_n, the
only connected cluster graph on n vertices.

.. doxygenenum:: graph_recognition::ClusterUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClusterUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ClusterUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cluster_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6, 7, 8``, the number of enumerated labeled
cluster graphs was verified to match the Bell numbers in `OEIS A000110
<https://oeis.org/A000110>`_: ``1, 2, 5, 15, 52, 203, 877, 4140``. The
non-isomorphic enumeration was verified through ``n = 20`` against the
partition numbers of `OEIS A000041 <https://oeis.org/A000041>`_:
``1, 2, 3, 5, 7, 11, 15, 22, 30, 42, 56, 77, 101, 135, 176, 231, 297,
385, 490, 627``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cluster.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {3, 4}});
       auto result = check_cluster(g);

       std::cout << std::boolalpha << result.is_cluster << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cluster_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cluster_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cluster_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cluster_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';  // 5 = number of partitions of 4
       return 0;
   }


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
