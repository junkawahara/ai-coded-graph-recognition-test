Circle Graph
============

A **circle graph** is the intersection graph of a set of chords of a
circle.  Two vertices are adjacent if and only if their corresponding chords
intersect.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_132.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircleAlgorithm``
     - Description
   * - ``DOW_BACKTRACKING`` **(default)**
     - Dow's chord-diagram backtracking. Attempts to construct a chord diagram
       consistent with the input graph by placing chord endpoints on a circle.

.. doxygenenum:: graph_recognition::CircleAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_circle
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CircleEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circle_graphs_reverse_search
   :project: graph_recognition


References
----------

* S. Even, A. Itai. "Queues, stacks and graphs."
  In Z. Kohavi, A. Paz (eds.), *Theory of Machines and Computations*, Academic Press,
  pp. 71--86, 1971.

* J. P. Spinrad. "Recognition of circle graphs."
  *Journal of Algorithms*, 16(2):264--282, 1994.
  `DOI:10.1006/jagm.1994.1012 <https://doi.org/10.1006/jagm.1994.1012>`_
