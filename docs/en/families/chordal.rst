Chordal Graph
=============

A graph is **chordal** (triangulated) if it contains no induced cycle of length
four or more. Equivalently, the graph admits a perfect elimination ordering
(PEO).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_32.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalAlgorithm``
     - Description
   * - ``MCS_PEO``
     - Priority-queue Maximum Cardinality Search, O(n + m log n)
   * - ``BUCKET_MCS_PEO`` **(default)**
     - Bucket-sort Maximum Cardinality Search, O(n + m)
   * - ``LEXBFS_PEO``
     - LexBFS by Rose-Tarjan-Lueker 1976, O(n + m)

.. doxygenenum:: graph_recognition::ChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal
   :project: graph_recognition


Enumeration
-----------

The default ``KIYOMI_UNO`` engine is the chordal-specific reverse search whose
roots are the one-edge graphs.  Its parent removes a minimum-degree simplicial
vertex, breaking ties by the smallest label.  Children add a new vertex whose
neighborhood is a clique and use the Kiyomi--Uno child conditions to avoid
duplicates.  A PEO is maintained throughout the search and drives clique
enumeration.

The paper's O(1) amortized time and O(1) delay bounds use its optimized
difference-output implementation.  This library preserves the dedicated search
tree but favors straightforward O(n^2) state and constructs a complete edge
list for every callback, so those bounds do not apply to this API.  The former
largest-label vertex search remains available as
``LEGACY_VERTEX_REVERSE_SEARCH`` for comparison and compatibility checks.

.. doxygenenum:: graph_recognition::ChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_graphs_reverse_search
   :project: graph_recognition


Subgraph Enumeration
--------------------

``chordal_subgraph_enum.h`` enumerates the chordal subgraphs of a **given host
graph** instead of all chordal graphs on ``n`` vertices, which is the problem
Kiyomi and Uno actually state; the enumeration above is its ``G = K_n`` case.
A chordal subgraph here is a spanning subgraph ``(V, E')`` with ``E'`` a subset
of the host's edges, so the outputs are in bijection with the chordal edge
subsets and the empty edge set is always among them.

It is the same reverse search with child generation filtered by host
adjacency.  That filter alone is correct because the parent rule only deletes
edges, so the subgraphs of a fixed host are closed under it and the restricted
search tree is rooted at the edges of the host.

The output size is driven by the edge count rather than the vertex count:
every subgraph of a forest is chordal, so a host with ``m`` edges can have up
to ``2^m`` chordal subgraphs.  Prefer the streaming callback API for large
hosts.

.. doxygenenum:: graph_recognition::ChordalSubgraphEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalSubgraphEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_subgraphs
   :project: graph_recognition


OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled chordal graphs
was verified to match `OEIS A058862 <https://oeis.org/A058862>`_:
``2, 8, 61, 822, 18154``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal(g);

       std::cout << std::boolalpha << result.is_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Subgraph enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_subgraph_enum.h"

   int main() {
       using namespace graph_recognition;

       // C4: every edge subset but C4 itself is chordal, so 15.
       Graph host(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = enumerate_chordal_subgraphs(host);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs."
  *SIAM Journal on Computing*, 5(2):266--283, 1976.
  `DOI:10.1137/0205021 <https://doi.org/10.1137/0205021>`_

* M. Kiyomi, T. Uno. "Generating chordal graphs included in given graphs."
  *IEICE Transactions on Information and Systems*, E89-D(2):763--770, 2006.
  `DOI:10.1093/ietisy/e89-d.2.763 <https://doi.org/10.1093/ietisy/e89-d.2.763>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
