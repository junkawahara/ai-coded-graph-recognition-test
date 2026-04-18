Co-Chordal Graph
================

A graph is **co-chordal** if its complement is a chordal graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_145.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoChordalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies chordal recognition.

.. doxygenenum:: graph_recognition::CoChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_chordal
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CoChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_chordal_graphs_reverse_search
   :project: graph_recognition


References
----------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
