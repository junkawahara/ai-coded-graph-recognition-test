Chordal Family
==============

The chordal family comprises graph classes defined by cycle restrictions and
perfect elimination orderings. A chordal graph -- a graph with no induced cycle
of length four or more -- sits at the centre of this family. Subclasses such as
split, threshold, and block graphs impose additional structural constraints,
while extensions such as weakly chordal and chordal bipartite graphs relax the
chordality condition in different directions.


chordal.h -- Chordal Graph
--------------------------

A graph is **chordal** (triangulated) if it contains no induced cycle of length
four or more. Equivalently, the graph admits a perfect elimination ordering
(PEO).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalAlgorithm``
     - Description
   * - ``MCS_PEO``
     - Priority-queue Maximum Cardinality Search, O(n + m log n)
   * - **``BUCKET_MCS_PEO``** **(default)**
     - Bucket-sort Maximum Cardinality Search, O(n + m)
   * - ``LEXBFS_PEO``
     - LexBFS by Rose-Tarjan-Lueker 1976, O(n + m)

.. doxygenfile:: chordal.h
   :project: graph_recognition


chordal_enum.h -- Chordal Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chordal_enum.h
   :project: graph_recognition


strongly_chordal.h -- Strongly Chordal Graph
---------------------------------------------

A graph is **strongly chordal** if it is chordal and every even cycle of length
six or more has an odd chord (a chord connecting two vertices at odd distance
along the cycle).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - Description
   * - ``STRONG_ELIMINATION``
     - Strong elimination ordering check, O(n^4)
   * - ``PEO_MATRIX``
     - PEO-based matrix method, O(n^2 + n m Delta)
   * - **``MCS_SEO``** **(default)**
     - MCS-based strong elimination ordering, O(n^2 + n m)

.. doxygenfile:: strongly_chordal.h
   :project: graph_recognition


strongly_chordal_enum.h -- Strongly Chordal Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: strongly_chordal_enum.h
   :project: graph_recognition


proper_chordal.h -- Proper Chordal Graph
-----------------------------------------

A graph is **proper chordal** if it is the intersection graph of a family of
subtrees of a tree such that no subtree is contained in another (the
nesting-free property).

.. doxygenfile:: proper_chordal.h
   :project: graph_recognition


proper_chordal_enum.h -- Proper Chordal Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: proper_chordal_enum.h
   :project: graph_recognition


split.h -- Split Graph
----------------------

A graph is a **split graph** if its vertex set can be partitioned into a clique
and an independent set.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Degree-sequence based recognition, O(n^2)
   * - **``HAMMER_SIMEONE``** **(default)**
     - Hammer-Simeone degree-sequence condition, O(n + m)

.. doxygenfile:: split.h
   :project: graph_recognition


split_enum.h -- Split Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: split_enum.h
   :project: graph_recognition


threshold.h -- Threshold Graph
------------------------------

A graph is a **threshold graph** if it can be reduced to the empty graph by
repeatedly removing an isolated vertex or a universal vertex (a vertex adjacent
to all others).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ThresholdAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Degree-sequence based recognition, O(n m)
   * - **``DEGREE_SEQUENCE_FAST``** **(default)**
     - Counting sort + two-pointer simulation, O(n + m)

.. doxygenfile:: threshold.h
   :project: graph_recognition


threshold_enum.h -- Threshold Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: threshold_enum.h
   :project: graph_recognition


chordal_bipartite.h -- Chordal Bipartite Graph
-----------------------------------------------

A graph is **chordal bipartite** if it is bipartite and contains no induced
cycle of length six or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - Description
   * - ``CYCLE_CHECK``
     - Brute-force induced even cycle search
   * - ``BISIMPLICIAL``
     - Bisimplicial edge elimination, O(m n^2)
   * - **``FAST_BISIMPLICIAL``** **(default)**
     - Fast bisimplicial edge elimination, O(m deg^2)

.. doxygenfile:: chordal_bipartite.h
   :project: graph_recognition


chordal_bipartite_enum.h -- Chordal Bipartite Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chordal_bipartite_enum.h
   :project: graph_recognition


weakly_chordal.h -- Weakly Chordal Graph
-----------------------------------------

A graph is **weakly chordal** if neither the graph nor its complement contains
an induced cycle of length five or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``WeaklyChordalAlgorithm``
     - Description
   * - ``CO_CHORDAL_BIPARTITE``
     - Co-chordal-bipartite based recognition, O(n^2 + n m)
   * - **``COMPLEMENT_BFS``** **(default)**
     - Complement BFS approach, O(n m)

.. doxygenfile:: weakly_chordal.h
   :project: graph_recognition


weakly_chordal_enum.h -- Weakly Chordal Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: weakly_chordal_enum.h
   :project: graph_recognition


block.h -- Block Graph
----------------------

A graph is a **block graph** if every biconnected component is a clique.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - Description
   * - **``DFS``** **(default)**
     - DFS-based biconnected component check, O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - Chordal + diamond-free recognition, O(n + m Delta)

.. doxygenfile:: block.h
   :project: graph_recognition


block_enum.h -- Block Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: block_enum.h
   :project: graph_recognition


ptolemaic.h -- Ptolemaic Graph
------------------------------

A graph is **ptolemaic** if it is both chordal and distance-hereditary.
Equivalently, the graph is gem-free and chordal.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - Description
   * - ``DH_HASHMAP``
     - Distance-hereditary check using hash maps
   * - **``DH_SORTED``** **(default)**
     - Distance-hereditary check using sorted adjacency lists

.. doxygenfile:: ptolemaic.h
   :project: graph_recognition


ptolemaic_enum.h -- Ptolemaic Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: ptolemaic_enum.h
   :project: graph_recognition


trivially_perfect.h -- Trivially Perfect Graph
-----------------------------------------------

A graph is **trivially perfect** if it is both chordal and a cograph.
Equivalently, for every connected induced subgraph, the graph has a universal
vertex; or equivalently, the adjacency relation coincides with the
ancestor-descendant relation in a DFS tree.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TriviallyPerfectAlgorithm``
     - Description
   * - **``DFS``** **(default)**
     - DFS-based recognition

.. doxygenfile:: trivially_perfect.h
   :project: graph_recognition


trivially_perfect_enum.h -- Trivially Perfect Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: trivially_perfect_enum.h
   :project: graph_recognition


quasi_threshold.h -- Quasi-Threshold Graph
-------------------------------------------

A **quasi-threshold graph** is an alternative name for a trivially perfect graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``QuasiThresholdAlgorithm``
     - Description
   * - **``DFS``** **(default)**
     - DFS-based recognition

.. doxygenfile:: quasi_threshold.h
   :project: graph_recognition


ktree.h -- K-Tree
------------------

A graph is a **k-tree** if it is chordal, every maximal clique has size exactly
k + 1, and every minimal separator has size exactly k. Equivalently, a k-tree
can be constructed starting from a complete graph on k vertices and repeatedly
adding a new vertex adjacent to exactly k vertices that form a clique.

.. doxygenfile:: ktree.h
   :project: graph_recognition


ktree_enum.h -- K-Tree Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: ktree_enum.h
   :project: graph_recognition
