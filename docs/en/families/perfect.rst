Perfect / Structural Family
============================

Recognition and enumeration algorithms for perfect graphs and structurally
characterized graph classes.

Each recognition function accepts an ``enum class`` parameter to choose
the algorithm.  The **bold** entry is the default.


.. _perfect-family:

Perfect Graph
-------------

perfect.h -- Perfect Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
"""""""""""

.. doxygenfile:: perfect_enum.h
   :project: graph_recognition


.. _structural-classes:

Structural Graph Classes
------------------------

cograph.h -- Cograph
^^^^^^^^^^^^^^^^^^^^^

A graph is a **cograph** if it contains no induced P4 (path on 4 vertices).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CographAlgorithm``
     - Description
   * - ``COTREE``
     - Cotree construction via recursive connected-component /
       co-component decomposition.
   * - ``PARTITION_REFINEMENT`` **(default)**
     - Same decomposition with a fast linked-list co-component search.
       Complexity: O(n + m).

.. doxygenfile:: cograph.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: cograph_enum.h
   :project: graph_recognition


distance_hereditary.h -- Distance-Hereditary Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A graph is **distance-hereditary** if, in every connected induced subgraph,
the distances between vertices are preserved from the original graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DistanceHereditaryAlgorithm``
     - Description
   * - ``HASHMAP_TWINS``
     - Iterative pendant/twin vertex removal using hash-map twin detection.
   * - ``SORTED_TWINS``
     - Iterative pendant/twin vertex removal using sorted neighbor-list
       comparison.  Deterministic.
   * - ``HASH_TWINS`` **(default)**
     - XOR hash incremental twin detection.  Each vertex receives a random
       64-bit weight; upon removal the neighbor hashes are updated in O(1).
       Hash matches are verified by exact neighbor-list comparison.
       Complexity: O(n + m) expected.

.. doxygenfile:: distance_hereditary.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: distance_hereditary_enum.h
   :project: graph_recognition


at_free.h -- AT-Free Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^

A graph is **AT-free** (asteroidal triple-free) if it contains no asteroidal
triple -- three vertices such that each pair is connected by a path that
avoids the closed neighborhood of the third vertex.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ATFreeAlgorithm``
     - Description
   * - ``BRUTE_FORCE`` **(default)**
     - Checks all triples (u, v, w) via BFS to determine whether each pair
       can be connected by a path avoiding the neighborhood of the third.

.. doxygenfile:: at_free.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: at_free_enum.h
   :project: graph_recognition


co_chordal.h -- Co-Chordal Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A graph is **co-chordal** if its complement is a chordal graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoChordalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies chordal recognition.

.. doxygenfile:: co_chordal.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: co_chordal_enum.h
   :project: graph_recognition


line_graph.h -- Line Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^

A **line graph** L(H) has vertices representing edges of some graph H,
with two vertices adjacent whenever the corresponding edges share an
endpoint.  By Whitney's theorem (1932), a graph is a line graph if and only
if it admits a Krausz partition (an edge-clique cover where each vertex
belongs to at most two cliques).

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

.. doxygenfile:: line_graph.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: line_graph_enum.h
   :project: graph_recognition


circle.h -- Circle Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^

A **circle graph** is the intersection graph of a set of chords of a
circle.  Two vertices are adjacent if and only if their corresponding chords
intersect.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircleAlgorithm``
     - Description
   * - **(default)**
     - Default circle graph recognition algorithm.

.. doxygenfile:: circle.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: circle_enum.h
   :project: graph_recognition


meyniel.h -- Meyniel Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A **Meyniel graph** is a graph in which every odd cycle of length five or
more has at least two chords.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``MeynielAlgorithm``
     - Description
   * - **(default)**
     - Default Meyniel graph recognition algorithm.

.. doxygenfile:: meyniel.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: meyniel_enum.h
   :project: graph_recognition


parity.h -- Parity Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^

A **parity graph** is a graph in which every pair of vertices has all
induced paths between them of the same parity (all even-length or all
odd-length).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ParityAlgorithm``
     - Description
   * - **(default)**
     - Default parity graph recognition algorithm.

.. doxygenfile:: parity.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: parity_enum.h
   :project: graph_recognition


even_hole_free.h -- Even-Hole-Free Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An **even-hole-free graph** contains no induced even cycle of length four
or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``EvenHoleFreeAlgorithm``
     - Description
   * - **(default)**
     - Default even-hole-free graph recognition algorithm.

.. doxygenfile:: even_hole_free.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: even_hole_free_enum.h
   :project: graph_recognition


odd_hole_free.h -- Odd-Hole-Free Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An **odd-hole-free graph** contains no induced odd cycle of length five
or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``OddHoleFreeAlgorithm``
     - Description
   * - **(default)**
     - Default odd-hole-free graph recognition algorithm.

.. doxygenfile:: odd_hole_free.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: odd_hole_free_enum.h
   :project: graph_recognition


cluster.h -- Cluster Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A **cluster graph** is a disjoint union of complete graphs.
Equivalently, it is P3-free (contains no induced path on 3 vertices).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClusterAlgorithm``
     - Description
   * - **(default)**
     - Default cluster graph recognition algorithm.

.. doxygenfile:: cluster.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: cluster_enum.h
   :project: graph_recognition


self_complementary.h -- Self-Complementary Graph
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A **self-complementary graph** is a graph that is isomorphic to its own
complement.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SelfComplementaryAlgorithm``
     - Description
   * - **(default)**
     - Default self-complementary graph recognition algorithm.

.. doxygenfile:: self_complementary.h
   :project: graph_recognition


Enumeration
"""""""""""

.. doxygenfile:: self_complementary_enum.h
   :project: graph_recognition
