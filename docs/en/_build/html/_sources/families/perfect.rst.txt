Perfect Graph
=============

A graph is **perfect** if every induced subgraph has chromatic number equal
to its clique number.  By the Strong Perfect Graph Theorem
(Chudnovsky-Robertson-Seymour-Thomas 2006), this is equivalent to
containing no odd hole (induced odd cycle of length >= 5) and no odd
antihole (complement of an odd hole of length >= 5).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_56.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Algorithm
     - Description
   * - **(default)**
     - For each edge (u,v), builds a restricted graph and first uses BFS to rule
       out the bipartite cases, which cannot contain an odd hole.  The remaining
       cases fall back to a backtracking DFS for an even-length induced path,
       which closes into an odd hole.  Odd antiholes are detected by running the
       same procedure on the complement graph.  The DFS is exponential in the
       worst case, and the antihole pass always builds a complement with
       Theta(n^2) edges, so a few hundred vertices is the practical limit.

.. doxygenstruct:: graph_recognition::PerfectResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_perfect
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PerfectLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PerfectLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_perfect_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "perfect.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_perfect(g);

       std::cout << std::boolalpha << result.is_perfect << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "perfect_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_perfect_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Chudnovsky, N. Robertson, P. Seymour, R. Thomas. "The strong perfect graph theorem."
  *Annals of Mathematics*, 164(1):51--229, 2006.
  `DOI:10.4007/annals.2006.164.51 <https://doi.org/10.4007/annals.2006.164.51>`_

* M. Chudnovsky, G. Cornuéjols, X. Liu, P. Seymour, K. Vušković. "Recognizing Berge graphs."
  *Combinatorica*, 25(2):143--186, 2005.
  `DOI:10.1007/s00493-005-0012-8 <https://doi.org/10.1007/s00493-005-0012-8>`_
