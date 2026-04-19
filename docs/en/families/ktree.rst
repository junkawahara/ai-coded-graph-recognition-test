K-Tree
======

A graph is a **k-tree** if it is chordal, every maximal clique has size exactly
k + 1, and every minimal separator has size exactly k. Equivalently, a k-tree
can be constructed starting from the complete graph ``K_{k+1}`` and repeatedly
adding a new vertex adjacent to exactly k vertices that form a clique.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_594.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::KTreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_ktree
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::KTreeEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ktree_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "ktree.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_ktree(g);

       std::cout << std::boolalpha << result.is_ktree << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ktree_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ktree_graphs_reverse_search(4, 1);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* D. J. Rose. "On simple characterizations of k-trees."
  *Discrete Mathematics*, 7(3--4):317--322, 1974.
  `DOI:10.1016/0012-365X(74)90042-9 <https://doi.org/10.1016/0012-365X(74)90042-9>`_

* L. W. Beineke, R. E. Pippert. "The number of labeled k-dimensional trees."
  *Journal of Combinatorial Theory*, 6(2):200--205, 1969.
  `DOI:10.1016/S0021-9800(69)80120-1 <https://doi.org/10.1016/S0021-9800(69)80120-1>`_
