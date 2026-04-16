Co-Interval Graph
=================

Determines whether a graph is a co-interval graph.
A graph whose complement is an interval graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoIntervalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies interval graph recognition.

.. doxygenfile:: co_interval.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: co_interval_enum.h
   :project: graph_recognition


References
----------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_
