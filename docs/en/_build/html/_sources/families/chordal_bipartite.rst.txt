Chordal Bipartite Graph
=======================

A graph is **chordal bipartite** if it is bipartite and contains no induced
cycle of length six or more.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_79.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - Description
   * - ``CYCLE_CHECK``
     - Brute-force induced even cycle search
   * - ``BISIMPLICIAL``
     - Bisimplicial edge elimination (brute force), O(m n^4)
   * - ``FAST_BISIMPLICIAL`` **(default)**
     - Fast bisimplicial edge elimination, O(m^2 Delta^2)

.. doxygenenum:: graph_recognition::ChordalBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal_bipartite
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ChordalBipartiteLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_bipartite_labeled_graphs_reverse_search
   :project: graph_recognition


Induced Subgraph Enumeration
----------------------------

``chordal_bipartite_induced_subgraph_enum.h`` enumerates the chordal bipartite
**induced** subgraphs of a given host graph: every vertex subset ``X`` of
``V(G)`` for which ``G[X]`` is chordal bipartite.  An induced subgraph is
determined by its vertex set, so the output is a family of vertex sets rather
than edge lists, and the empty set is always the first solution.

The search is a reverse search on the paper's characterization: a graph is
chordal bipartite exactly when it can be emptied by repeatedly removing a
*weak-simplicial* vertex, one whose neighborhood is independent and totally
ordered by inclusion.  The parent of a nonempty solution removes its largest
weak-simplicial vertex, so ``X`` plus ``v`` is a child of ``X`` precisely when
``v`` is weak-simplicial in the enlarged subgraph and is the largest such
vertex.  No recognizer call is needed to test a child.

The output size is driven by the vertex count rather than the edge count: the
class is hereditary, so a host that is itself chordal bipartite has all ``2^n``
vertex subsets as solutions no matter how sparse it is.  Prefer the streaming
callback API for large hosts.

This implementation recomputes the weak-simplicial vertices per candidate
instead of maintaining them differentially, so the paper's amortized
``O(k t Delta^2)`` bound does not apply; only the delay differs, not the
enumerated family.

.. doxygenenum:: graph_recognition::ChordalBipartiteInducedSubgraphEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteInducedSubgraphEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_bipartite_induced_subgraphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/chordal_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal_bipartite(g);

       std::cout << std::boolalpha << result.is_chordal_bipartite << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/chordal_bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Induced subgraph enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/chordal_bipartite_induced_subgraph_enum.h"

   int main() {
       using namespace graph_recognition;

       // C6: every vertex subset but the whole cycle is chordal bipartite.
       Graph g(6, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
       auto result = enumerate_chordal_bipartite_induced_subgraphs(g);

       std::cout << result.vertex_sets.size() << '\n';  // 63
       return 0;
   }


References
----------

* M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs."
  *Journal of Graph Theory*, 2(2):155--163, 1978.
  `DOI:10.1002/jgt.3190020209 <https://doi.org/10.1002/jgt.3190020209>`_

* A. Lubiw. "Doubly lexical orderings of matrices."
  *SIAM Journal on Computing*, 16(5):854--879, 1987.
  `DOI:10.1137/0216057 <https://doi.org/10.1137/0216057>`_

* K. Kurita, K. Wasa, H. Arimura, T. Uno. "An Efficient Algorithm for
  Enumerating Chordal Bipartite Induced Subgraphs in Sparse Graphs."
  *COCOON 2019*, LNCS 11653, 339--351.
  `DOI:10.1007/978-3-030-26176-4_28 <https://doi.org/10.1007/978-3-030-26176-4_28>`_
