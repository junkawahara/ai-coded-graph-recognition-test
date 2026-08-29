Line Graph
==========

A **line graph** L(H) has vertices representing edges of some graph H,
with two vertices adjacent whenever the corresponding edges share an
endpoint.  By Krausz's theorem (1943), a graph is a line graph if and only
if it admits a Krausz partition (an edge-clique cover where each vertex
belongs to at most two cliques).  (Whitney's theorem (1932) is the different
statement that H is determined by L(H), except for K_3 and K_{1,3}.)

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_249.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``LineGraphAlgorithm``
     - Description
   * - ``BRUTE``
     - Backtracking search for a Krausz partition.  Enumerates all
       clique candidates containing each edge.  Suitable for small graphs.
   * - ``KRAUSZ`` **(default)**
     - Prerequisite filter (bipartiteness check of the complement of each
       vertex's neighborhood) followed by Krausz partition construction with
       pruning.  The O(m * Delta) filter decides most inputs, but ambiguous
       ones fall back to a backtracking construction of the partition, so the
       worst case is exponential.

.. doxygenenum:: graph_recognition::LineGraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LineGraphResult
   :project: graph_recognition
   :members:

``LineGraphResult`` reports the Krausz partition (``krausz``), a root graph
(``root_graph``), and the edge of it that each vertex represents
(``vertex_to_root_edge``). A vertex of g becomes an edge of H joining the (at
most two) cliques that contain it; a vertex in fewer than two cliques gets
private H-vertices to fill the missing endpoints, which is what makes
isolated vertices and pendant edges come out right. L(root_graph) == g is
verified before the result is returned.

.. doxygenfunction:: graph_recognition::check_line_graph
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::LineGraphLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LineGraphLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_line_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "line_graph.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_line_graph(g);

       std::cout << std::boolalpha << result.is_line_graph << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "line_graph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_line_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* H. Whitney. "Congruent graphs and the connectivity of graphs."
  *American Journal of Mathematics*, 54(1):150--168, 1932.
  `DOI:10.2307/2371086 <https://doi.org/10.2307/2371086>`_

* J. Krausz. "Démonstration nouvelle d'un théorème de Whitney sur les réseaux."
  *Matematikai és Fizikai Lapok*, 50:75--85, 1943.

* L. W. Beineke. "Derived graphs of digraphs."
  In H. Sachs, H.-J. Voss, H.-J. Walther (eds.), *Beiträge zur Graphentheorie*,
  Teubner, pp. 17--33, 1968.

* N. D. Roussopoulos. "A max {m, n} algorithm for determining the graph H from its line graph G."
  *Information Processing Letters*, 2(4):108--112, 1973.
  `DOI:10.1016/0020-0190(73)90029-X <https://doi.org/10.1016/0020-0190(73)90029-X>`_

* P. G. H. Lehot. "An optimal algorithm to detect a line graph and output its root graph."
  *Journal of the ACM*, 21(4):569--575, 1974.
  `DOI:10.1145/321850.321853 <https://doi.org/10.1145/321850.321853>`_
