Snark
=====

Determines whether a graph is a snark.
A snark is a cubic graph that is bridgeless, has girth at least 5, is
cyclically 4-edge-connected and has chromatic index 4 (that is, it is not
3-edge-colorable).  The smallest snark is the Petersen graph (n = 10), so
every graph on fewer than 10 vertices is rejected.

Recognition
-----------

.. doxygenenum:: graph_recognition::SnarkAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_snark
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SnarkLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, snarkhunter-style: the
degree-constrained McKay canonical construction path of the cubic
non-isomorphic enumerator with the girth constraint folded into the
search. Girth at least 5 is preserved under vertex deletion, so the
intermediate levels range over the graphs with maximum degree at most 3
and girth at least 5: a candidate neighborhood of the new vertex may
not contain two vertices at distance at most 2 (that would close a
cycle of length at most 4). Every graph reaching the last level is
cubic with girth at least 5 by construction; the remaining snark
conditions (cyclic 4-edge-connectivity and chromatic index 4) are not
monotone along the construction and are checked at emission via
``check_snark``.

.. doxygenenum:: graph_recognition::SnarkUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SnarkUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

The non-isomorphic enumeration was verified through ``n = 16`` against
`OEIS A130315 <https://oeis.org/A130315>`_ (``1, 0, 0, 0`` for
``n = 10, 12, 14, 16``); the single output at ``n = 10`` is the
Petersen graph. It was also cross-checked against the canonicalized
output of the labeled enumerator through ``n = 6`` (both sides are
empty there, the smallest snark having 10 vertices). The static test
cases stop at ``n = 14`` (``n = 16`` takes about six minutes).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "snark.h"

   int main() {
       using namespace graph_recognition;

       // Petersen graph (the smallest snark)
       Graph g(10, {
           {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
           {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6},
           {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}
       });
       auto result = check_snark(g);

       std::cout << std::boolalpha << result.is_snark << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "snark_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_labeled_graphs(10);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "snark_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_unlabeled_graphs(10);
       std::cout << result.graphs.size() << '\n';  // 1 (the Petersen graph)
       return 0;
   }


References
----------

* R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable."
  *The American Mathematical Monthly*, 82(3):221--239, 1975.
  `DOI:10.2307/2319844 <https://doi.org/10.2307/2319844>`_

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, J. Hägglund, K. Markström. "Generation and properties of snarks."
  *Journal of Combinatorial Theory, Series B*, 103(4):468--488, 2013.
  `DOI:10.1016/j.jctb.2013.05.001 <https://doi.org/10.1016/j.jctb.2013.05.001>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
