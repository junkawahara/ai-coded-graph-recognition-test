Python API
==========

Python bindings for the Graph Recognition Library.
Uses pybind11 to call C++ recognition algorithms from Python.

Installation
------------

.. code-block:: bash

   pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

   # With NetworkX support
   pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

   # Local build (development)
   cd python
   pip install ".[dev]"

Quick Start
-----------

.. code-block:: python

   from graph_recognition import is_interval, is_chordal, recognize_interval

   # (n, edges) format (1-indexed)
   is_interval(4, [(1, 2), (2, 3), (3, 4)])  # True
   is_interval(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

   # Algorithm selection
   is_chordal(4, [(1, 2), (2, 3), (3, 4)], algorithm="mcs_peo")

   # recognize returns (bool, certificate_dict_or_None)
   result, cert = recognize_interval(4, [(1, 2), (2, 3), (3, 4)])

NetworkX Integration
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import networkx as nx
   from graph_recognition import is_interval

   G = nx.path_graph(5)
   is_interval(G)  # True

   # Arbitrary node types are supported
   G = nx.Graph()
   G.add_edges_from([("a", "b"), ("b", "c")])
   is_interval(G)  # True


Recognition Functions
---------------------

Two function types are provided for each graph class:

* ``is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool``
* ``recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)``

**Chordal Family**

* ``is_chordal`` / ``recognize_chordal`` -- algorithms: ``mcs_peo``, ``bucket_mcs_peo``, ``lexbfs_peo``
* ``is_strongly_chordal`` / ``recognize_strongly_chordal`` -- algorithms: ``strong_elimination``, ``peo_matrix``, ``mcs_seo``
* ``is_split`` / ``recognize_split`` -- algorithms: ``degree_sequence``, ``hammer_simeone``
* ``is_threshold`` / ``recognize_threshold`` -- algorithms: ``degree_sequence``, ``degree_sequence_fast``
* ``is_chordal_bipartite`` / ``recognize_chordal_bipartite`` -- algorithms: ``cycle_check``, ``bisimplicial``, ``fast_bisimplicial``
* ``is_weakly_chordal`` / ``recognize_weakly_chordal`` -- algorithms: ``co_chordal_bipartite``, ``complement_bfs``

**Interval / Arc Family**

* ``is_interval`` / ``recognize_interval`` -- algorithms: ``backtracking``, ``at_free``
* ``is_proper_interval`` / ``recognize_proper_interval`` -- algorithms: ``triple_loop_claw_check``, ``fast_claw_check``
* ``is_unit_interval`` / ``recognize_unit_interval`` -- algorithms: ``proper_interval``
* ``is_co_interval`` / ``recognize_co_interval`` -- algorithms: ``complement``
* ``is_circular_arc`` / ``recognize_circular_arc`` -- algorithms: ``mcconnell``, ``backtracking``

**Permutation / Comparability Family**

* ``is_permutation`` / ``recognize_permutation`` -- algorithms: ``backtracking``, ``class_based``
* ``is_comparability`` / ``recognize_comparability`` -- algorithms: ``transitive_orientation``
* ``is_co_comparability`` / ``recognize_co_comparability`` -- algorithms: ``complement``
* ``is_bipartite_permutation`` / ``recognize_bipartite_permutation`` -- algorithms: ``bipartite_and_permutation``
* ``is_trapezoid`` / ``recognize_trapezoid`` -- algorithms: ``chain_cover``

**Bipartite Family**

* ``is_bipartite`` / ``recognize_bipartite`` -- algorithms: ``bfs``
* ``is_convex_bipartite`` / ``recognize_convex_bipartite`` -- algorithms: ``brute_force``, ``c1p``
* ``is_biconvex_bipartite`` / ``recognize_biconvex_bipartite`` -- algorithms: ``brute_force``, ``c1p``
* ``is_chain`` / ``recognize_chain`` -- algorithms: ``neighborhood_inclusion``, ``degree_sort``
* ``is_cochain`` / ``recognize_cochain`` -- algorithms: ``complement``, ``direct``

**Perfect Graph**

* ``is_perfect`` / ``recognize_perfect``

**Other Classes**

* ``is_cograph`` / ``recognize_cograph`` -- algorithms: ``cotree``, ``partition_refinement``
* ``is_block`` / ``recognize_block`` -- algorithms: ``dfs``, ``chordal_diamond_free``
* ``is_distance_hereditary`` / ``recognize_distance_hereditary`` -- algorithms: ``hashmap_twins``, ``sorted_twins``, ``hash_twins``
* ``is_ptolemaic`` / ``recognize_ptolemaic`` -- algorithms: ``dh_hashmap``, ``dh_sorted``
* ``is_trivially_perfect`` / ``recognize_trivially_perfect`` -- algorithms: ``dfs``
* ``is_quasi_threshold`` / ``recognize_quasi_threshold`` -- algorithms: ``dfs``
* ``is_at_free`` / ``recognize_at_free`` -- algorithms: ``brute_force``
* ``is_co_chordal`` / ``recognize_co_chordal`` -- algorithms: ``complement``
* ``is_three_leaf_power`` / ``recognize_three_leaf_power``

**Planar Family**

* ``is_planar`` / ``recognize_planar`` -- algorithms: ``left_right``, ``minor_check``
* ``is_outer_planar`` / ``recognize_outer_planar`` -- algorithms: ``minor_check``, ``augmented_planarity``
* ``is_cactus`` / ``recognize_cactus`` -- algorithms: ``dfs``
* ``is_series_parallel`` / ``recognize_series_parallel`` -- algorithms: ``minor_check``, ``queue_reduction``

**Forbidden Subgraph Family**

* ``is_claw_free`` / ``recognize_claw_free`` -- algorithms: ``triple_loop``, ``edge_count``
* ``is_diamond_free`` / ``recognize_diamond_free`` -- algorithms: ``brute``, ``edge_pair``
* ``is_line_graph`` / ``recognize_line_graph`` -- algorithms: ``brute``, ``krausz``

**Basic Structure**

* ``is_tree`` / ``recognize_tree`` -- algorithms: ``bfs``
* ``is_forest`` / ``recognize_forest`` -- algorithms: ``bfs``
* ``is_caterpillar`` / ``recognize_caterpillar`` -- algorithms: ``leaf_removal``
* ``is_unicyclic`` / ``recognize_unicyclic`` -- algorithms: ``bfs``
* ``is_cluster`` / ``recognize_cluster`` -- algorithms: ``component_clique``
* ``is_biconnected`` / ``recognize_biconnected`` -- algorithms: ``dfs``
* ``is_triconnected`` / ``recognize_triconnected`` -- algorithms: ``naive``
* ``is_eulerian`` / ``recognize_eulerian`` -- algorithms: ``degree_check``
* ``is_cubic`` / ``recognize_cubic`` -- algorithms: ``degree_check``
* ``is_kregular`` / ``recognize_kregular`` -- algorithms: ``degree_check``
* ``is_strongly_regular`` / ``recognize_strongly_regular`` -- algorithms: ``parameter_check``
* ``is_self_complementary`` / ``recognize_self_complementary`` -- algorithms: ``isomorphism_check``
* ``is_ktree`` / ``recognize_ktree`` -- algorithms: ``simplicial_removal``
* ``is_laman`` / ``recognize_laman`` -- algorithms: ``pebble_game``

**Planar Family (additional)**

* ``is_maximal_planar`` / ``recognize_maximal_planar`` -- algorithms: ``planar_edge_count``
* ``is_cubic_planar`` / ``recognize_cubic_planar`` -- algorithms: ``cubic_and_planar``
* ``is_polyhedral`` / ``recognize_polyhedral`` -- algorithms: ``steinitz``
* ``is_apex`` / ``recognize_apex`` -- algorithms: ``vertex_deletion``
* ``is_halin`` / ``recognize_halin`` -- algorithms: ``face_check``
* ``is_fullerene`` / ``recognize_fullerene`` -- algorithms: ``face_check``
* ``is_simple_quadrangulation`` / ``recognize_simple_quadrangulation`` -- algorithms: ``combined``
* ``is_snark`` / ``recognize_snark`` -- algorithms: ``combined``

**Forbidden Subgraph Family (additional)**

* ``is_triangle_free`` / ``recognize_triangle_free`` -- algorithms: ``edge_pair``, ``brute``
* ``is_bull_free`` / ``recognize_bull_free`` -- algorithms: ``triangle_search``, ``brute``
* ``is_gem_free`` / ``recognize_gem_free`` -- algorithms: ``neighbor_p4_search``, ``brute``
* ``is_p5_free`` / ``recognize_p5_free`` -- algorithms: ``path_search``, ``brute``
* ``is_even_hole_free`` / ``recognize_even_hole_free``
* ``is_odd_hole_free`` / ``recognize_odd_hole_free``
* ``is_meyniel`` / ``recognize_meyniel`` -- algorithms: ``direct_check``
* ``is_parity`` / ``recognize_parity`` -- algorithms: ``direct_check``

**Leaf Powers and Orderings (additional)**

* ``is_four_leaf_power`` / ``recognize_four_leaf_power``
* ``is_five_leaf_power`` / ``recognize_five_leaf_power``
* ``is_proper_chordal`` / ``recognize_proper_chordal``
* ``is_proper_circular_arc`` / ``recognize_proper_circular_arc``
* ``is_circle`` / ``recognize_circle`` -- algorithms: ``naji_system``, ``dow_backtracking``

**Directed Input**

The edge list is interpreted as *directed* arcs for these recognizers
(for ``is_poset`` the arcs are the covering relation: ``(u, v)`` means
``v`` covers ``u``):

* ``is_digraph`` / ``recognize_digraph`` -- algorithms: ``validity_check``
* ``is_poset`` / ``recognize_poset`` -- algorithms: ``dag_and_reduction``
* ``is_tournament`` / ``recognize_tournament`` -- algorithms: ``arc_check``


Enumeration Functions
---------------------

Each enumeration function takes a vertex count ``n`` and generates all labeled
graphs of that size (``chain``, ``cochain`` and ``threshold`` are the three
exceptions: they enumerate up to isomorphism).
``n`` is capped at 6 (``ENUM_MAX_N``); larger values raise ``ValueError``,
because the full result is materialized in memory and the number of labeled
graphs explodes super-exponentially (use the streaming C++ CLI for larger ``n``).

**Chordal Family**

* ``enumerate_chordal_graphs(n)``
* ``enumerate_strongly_chordal_graphs(n)``
* ``enumerate_weakly_chordal_graphs(n)``
* ``enumerate_split_graphs(n)``
* ``enumerate_threshold_graphs(n)``
* ``enumerate_ptolemaic_graphs(n)``
* ``enumerate_trivially_perfect_graphs(n)``

**Interval Family**

* ``enumerate_interval_graphs(n)``
* ``enumerate_proper_interval_graphs(n)``
* ``enumerate_co_interval_graphs(n)``
* ``enumerate_circular_arc_graphs(n)``

**Permutation / Comparability Family**

* ``enumerate_permutation_graphs(n)``
* ``enumerate_comparability_graphs(n)``
* ``enumerate_co_comparability_graphs(n)``
* ``enumerate_trapezoid_graphs(n)``

**Bipartite Family**

* ``enumerate_bipartite_graphs(n)``
* ``enumerate_bipartite_permutation_graphs(n)``
* ``enumerate_convex_bipartite_graphs(n)``
* ``enumerate_biconvex_bipartite_graphs(n)``
* ``enumerate_chain_graphs(n)``
* ``enumerate_cochain_graphs(n)``
* ``enumerate_chordal_bipartite_graphs(n)``

**Perfect Graph**

* ``enumerate_perfect_graphs(n)``

**Other Classes**

* ``enumerate_cograph_graphs(n)``
* ``enumerate_block_graphs(n)``
* ``enumerate_distance_hereditary_graphs(n)``
* ``enumerate_three_leaf_power_graphs(n)``
* ``enumerate_at_free_graphs(n)``
* ``enumerate_co_chordal_graphs(n)``

**Planar Family**

* ``enumerate_planar_graphs(n)``
* ``enumerate_outer_planar_graphs(n)``
* ``enumerate_cactus_graphs(n)``
* ``enumerate_series_parallel_graphs(n)``

**Forbidden Subgraph Family**

* ``enumerate_claw_free_graphs(n)``
* ``enumerate_diamond_free_graphs(n)``
* ``enumerate_line_graph_graphs(n)``


Subgraph Enumeration Functions
------------------------------

These take a *host graph* instead of a vertex count and enumerate the
subgraphs of it that belong to the class.  The input is the same
``(n, edges)`` pair or ``networkx.Graph`` the recognizers accept.

* ``enumerate_chordal_subgraphs(n_or_graph, edges=None)``

A subgraph here is a spanning subgraph ``(V, E')`` with ``E'`` a subset of the
host's edges: the vertex set is fixed and isolated vertices are kept, so the
results are in bijection with the class's edge subsets of the host and the
empty edge set is always among them.

The cap is on the host's edge count, not its vertex count: every subgraph of a
forest is chordal, so a host with ``m`` edges can have up to ``2^m`` chordal
subgraphs.  Hosts with more than 16 distinct edges (``ENUM_MAX_M``) raise
``ValueError``; use the streaming C++ API for larger ones.

.. code-block:: python

   from graph_recognition import enumerate_chordal_subgraphs

   # C4: every edge subset but C4 itself is chordal.
   len(enumerate_chordal_subgraphs(4, [(1, 2), (2, 3), (3, 4), (4, 1)]))
   # 15


Induced Subgraph Enumeration Functions
--------------------------------------

These also take a *host graph*, but their solutions are the vertex subsets
``X`` for which ``G[X]`` belongs to the class, so they return lists of vertex
lists rather than edge lists.

* ``enumerate_chordal_bipartite_induced_subgraphs(n_or_graph, edges=None)``

Each vertex list is sorted ascending, and the empty set always comes first.

The cap is on the host's vertex count, not its edge count: these classes are
hereditary, so a host that is itself in the class has all ``2^n`` vertex
subsets as solutions no matter how sparse it is.  Hosts with more than 16
vertices (``INDUCED_ENUM_MAX_N``) raise ``ValueError``; use the streaming C++
API for larger ones.

.. code-block:: python

   from graph_recognition import enumerate_chordal_bipartite_induced_subgraphs

   # C6: every vertex subset but the whole cycle is chordal bipartite.
   len(enumerate_chordal_bipartite_induced_subgraphs(
       6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)]))
   # 63


Testing
-------

.. code-block:: bash

   cd python
   pip install ".[dev]"
   pytest tests/ -v
