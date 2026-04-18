Co-Interval Graph
=================

Determines whether a graph is a co-interval graph.
A graph whose complement is an interval graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_157.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoIntervalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies interval graph recognition.

.. doxygenenum:: graph_recognition::CoIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_interval
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CoIntervalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_interval_graphs_reverse_search
   :project: graph_recognition


References
----------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_
