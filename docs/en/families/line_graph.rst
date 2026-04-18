Line Graph
==========

A **line graph** L(H) has vertices representing edges of some graph H,
with two vertices adjacent whenever the corresponding edges share an
endpoint.  By Whitney's theorem (1932), a graph is a line graph if and only
if it admits a Krausz partition (an edge-clique cover where each vertex
belongs to at most two cliques).

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
       pruning.  Complexity: O(m * Delta).

.. doxygenenum:: graph_recognition::LineGraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LineGraphResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_line_graph
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::LineGraphEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LineGraphEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_line_graphs_reverse_search
   :project: graph_recognition


References
----------

* H. Whitney. "Congruent graphs and the connectivity of graphs."
  *American Journal of Mathematics*, 54(1):150--168, 1932.
  `DOI:10.2307/2371086 <https://doi.org/10.2307/2371086>`_

* L. W. Beineke. "Derived graphs of digraphs."
  In H. Sachs, H.-J. Voss, H.-J. Walther (eds.), *Beiträge zur Graphentheorie*,
  Teubner, pp. 17--33, 1968.

* N. D. Roussopoulos. "A max {m, n} algorithm for determining the graph H from its line graph G."
  *Information Processing Letters*, 2(4):108--112, 1973.
  `DOI:10.1016/0020-0190(73)90029-X <https://doi.org/10.1016/0020-0190(73)90029-X>`_

* P. G. H. Lehot. "An optimal algorithm to detect a line graph and output its root graph."
  *Journal of the ACM*, 21(4):569--575, 1974.
  `DOI:10.1145/321850.321853 <https://doi.org/10.1145/321850.321853>`_
