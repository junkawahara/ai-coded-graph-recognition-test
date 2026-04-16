Apex Graph
==========

A graph is an **apex graph** if removing a single vertex makes it planar.
The algorithm tests planarity of G - v for each vertex v.

.. doxygenfile:: apex.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: apex_enum.h
   :project: graph_recognition


References
----------

* N. Robertson, P. D. Seymour. "Graph minors. XIII. The disjoint paths problem."
  *Journal of Combinatorial Theory, Series B*, 63(1):65--110, 1995.
  `DOI:10.1006/jctb.1995.1006 <https://doi.org/10.1006/jctb.1995.1006>`_

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_
