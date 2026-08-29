Cubic Graph
===========

Determines whether a graph is cubic (3-regular).
A graph where every vertex has degree exactly 3.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1100.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CubicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CubicLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method with the degree constraint driving the search (the vertex
counterpart of snarkhunter's canonical deletion). Cubic graphs are not
hereditary, but every induced subgraph of a cubic graph has maximum
degree at most 3, so the intermediate levels of the vertex-by-vertex
search range over the graphs with maximum degree at most 3: the new
vertex's neighborhood runs only over the at-most-3-subsets of the
vertices of degree less than 3, further pruned by necessary
completability conditions (with ``r`` vertices still to come, every
degree deficit is at most ``r``, their sum is at most ``3r``, and
``3r`` minus the sum is even), which force every graph reaching the
last level to be cubic. A child survives only when the added vertex
lies in the automorphism orbit of the canonically last vertex of the
child.

.. doxygenenum:: graph_recognition::CubicUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CubicUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 4, 6, 8``, the number of enumerated labeled cubic graphs was
verified to match `OEIS A002829 <https://oeis.org/A002829>`_ (which
indexes ``a(k)`` by ``2k`` vertices, so the vertex count ``n`` here
corresponds to OEIS index ``n/2``): ``1, 70, 19355``.

The non-isomorphic enumeration was verified through ``n = 14`` against
`OEIS A005638 <https://oeis.org/A005638>`_ (indexed the same way;
``1, 2, 6, 21, 94, 540`` for ``n = 4, 6, ..., 14``) and cross-checked
against the canonicalized output of the labeled enumerator through
``n = 6``. With ``connected_only`` the counts match
`OEIS A002851 <https://oeis.org/A002851>`_
(``1, 2, 5, 19, 85, 509``). The static test cases stop at ``n = 12``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cubic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic(g);

       std::cout << std::boolalpha << result.is_cubic << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 6
       return 0;
   }


References
----------

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "Generation of cubic graphs."
  *Discrete Mathematics and Theoretical Computer Science*, 13(2):69--80, 2011.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
