Block Graph
===========

A graph is a **block graph** if every biconnected component is a clique.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_93.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - DFS-based biconnected component check, O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - Chordal + diamond-free recognition, O(n + m Delta^2)
   * - ``BLOCK_CUT_TREE``
     - Shared block decomposition (block-cut tree) plus a clique test per
       block, O(n + m)

.. doxygenenum:: graph_recognition::BlockAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_block
   :project: graph_recognition


Enumeration
-----------

Both enumerators default to ``BLOCK_GRAPH_GEN`` (Ekim--Shalom--Yirik, 2024).
Weighted block trees alternate cut nodes of weight 1 with block nodes weighted
by their number of non-cut vertices. Ordered multisets of branches and the
three weighted-centroid cases avoid isomorphic trees, including zero-weight
internal blocks. Each block becomes a clique; multisets of connected components
give all disconnected representatives. Labeled enumeration expands each
representative into its ``n! / |Aut(G)|`` distinct labelings.

The labeled ``REVERSE_SEARCH`` option retains the original chordal search with
block recognition and its output order. The historical labeled function name
is unchanged; the two algorithms may return graphs in different orders.
The unlabeled API supports ``connected_only``. Both C++ APIs and CLIs accept
``0 <= n <= max_vertex_count`` without an adjacency-bitmask restriction.
At zero vertices there is one empty graph, or no graph in connected-only mode.

Rooted branches up to weight ``floor(n/2)`` are cached. Component composition
also retains connected representatives. Reconstruction and sorting of one
edge list cost ``O(n + m log(m+1))``, separately from tree generation and label
expansion. All results are retained, and label expansion also keeps a visited
set. The paper's linear delay for tree representations is not claimed for
these materializing APIs.

.. code-block:: bash

   echo "6" | ./bin/block_labeled_enum --algo block_graph_gen
   echo "6" | ./bin/block_labeled_enum --algo reverse_search
   echo "6" | ./bin/block_unlabeled_enum --connected --algo block_graph_gen

Python exposes ``enumerate_block_labeled_graphs(n, algorithm="block_graph_gen")``
and ``enumerate_block_unlabeled_graphs(n, connected_only=False,
algorithm="block_graph_gen")``. Use ``algorithm="reverse_search"`` for the
legacy labeled algorithm. Python accepts 1--6 vertices for labeled graphs and
1--8 for unlabeled graphs. For example, six vertices give 7,739 labeled graphs,
50 unlabeled graphs, or 22 connected unlabeled graphs.

.. doxygenenum:: graph_recognition::BlockLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenenum:: graph_recognition::BlockUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BlockUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_unlabeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/block.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_block(g);

       std::cout << std::boolalpha << result.is_block << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/block_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_block_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* T. Ekim, M. Shalom, M. A. Yirik. "Generation of weighted trees, block trees
  and block graphs." 2024.
  `arXiv:2401.09764 <https://arxiv.org/abs/2401.09764>`_
* F. Harary. "A characterization of block-graphs."
  *Canadian Mathematical Bulletin*, 6(1):1--6, 1963.
  `DOI:10.4153/CMB-1963-001-x <https://doi.org/10.4153/CMB-1963-001-x>`_
