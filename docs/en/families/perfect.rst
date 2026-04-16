Perfect Graph
=============

A graph is **perfect** if every induced subgraph has chromatic number equal
to its clique number.  By the Strong Perfect Graph Theorem
(Chudnovsky-Robertson-Seymour-Thomas 2006), this is equivalent to
containing no odd hole (induced odd cycle of length >= 5) and no odd
antihole (complement of an odd hole of length >= 5).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Algorithm
     - Description
   * - **(default)**
     - For each edge (u,v), builds a restricted graph and checks shortest-path
       parity via BFS to detect odd holes.  Odd antiholes are detected by
       running the same procedure on the complement graph.

.. doxygenfile:: perfect.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: perfect_enum.h
   :project: graph_recognition


References
----------

* M. Chudnovsky, N. Robertson, P. Seymour, R. Thomas. "The strong perfect graph theorem."
  *Annals of Mathematics*, 164(1):51--229, 2006.
  `DOI:10.4007/annals.2006.164.51 <https://doi.org/10.4007/annals.2006.164.51>`_

* M. Chudnovsky, G. Cornuéjols, X. Liu, P. Seymour, K. Vušković. "Recognizing Berge graphs."
  *Combinatorica*, 25(2):143--186, 2005.
  `DOI:10.1007/s00493-005-0012-8 <https://doi.org/10.1007/s00493-005-0012-8>`_
