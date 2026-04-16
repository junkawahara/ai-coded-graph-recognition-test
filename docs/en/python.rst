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

   from graph_recognition import is_interval, is_chordal

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

* ``is_chordal`` / ``recognize_chordal`` -- algorithms: ``mcs_peo``, ``bucket_mcs_peo``
* ``is_strongly_chordal`` / ``recognize_strongly_chordal`` -- algorithms: ``strong_elimination``, ``peo_matrix``
* ``is_split`` / ``recognize_split`` -- algorithms: ``degree_sequence``, ``hammer_simeone``
* ``is_threshold`` / ``recognize_threshold`` -- algorithms: ``degree_sequence``, ``degree_sequence_fast``
* ``is_chordal_bipartite`` / ``recognize_chordal_bipartite`` -- algorithms: ``cycle_check``, ``bisimplicial``, ``fast_bisimplicial``
* ``is_weakly_chordal`` / ``recognize_weakly_chordal`` -- algorithms: ``co_chordal_bipartite``, ``complement_bfs``

**Interval / Arc Family**

* ``is_interval`` / ``recognize_interval`` -- algorithms: ``backtracking``, ``at_free``
* ``is_proper_interval`` / ``recognize_proper_interval`` -- algorithms: ``pq_tree``, ``fast_claw_check``
* ``is_unit_interval`` / ``recognize_unit_interval`` -- algorithms: ``proper_interval``
* ``is_co_interval`` / ``recognize_co_interval`` -- algorithms: ``complement``
* ``is_circular_arc`` / ``recognize_circular_arc`` -- algorithms: ``mcconnell``, ``backtracking``

**Permutation / Comparability Family**

* ``is_permutation`` / ``recognize_permutation`` -- algorithms: ``backtracking``, ``class_based``
* ``is_comparability`` / ``recognize_comparability`` -- algorithms: ``transitive_orientation``
* ``is_co_comparability`` / ``recognize_co_comparability`` -- algorithms: ``complement``
* ``is_bipartite_permutation`` / ``recognize_bipartite_permutation`` -- algorithms: ``chain_both_sides``
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
* ``is_block`` / ``recognize_block`` -- algorithms: ``dfs``
* ``is_distance_hereditary`` / ``recognize_distance_hereditary`` -- algorithms: ``hashmap_twins``, ``sorted_twins``, ``hash_twins``
* ``is_ptolemaic`` / ``recognize_ptolemaic`` -- algorithms: ``dh_hashmap``, ``dh_sorted``
* ``is_trivially_perfect`` / ``recognize_trivially_perfect`` -- algorithms: ``dfs``
* ``is_quasi_threshold`` / ``recognize_quasi_threshold`` -- algorithms: ``dfs``
* ``is_at_free`` / ``recognize_at_free`` -- algorithms: ``brute_force``
* ``is_co_chordal`` / ``recognize_co_chordal`` -- algorithms: ``complement``
* ``is_three_leaf_power`` / ``recognize_three_leaf_power``

**Planar Family**

* ``is_planar`` / ``recognize_planar`` -- algorithms: ``minor_check``
* ``is_outer_planar`` / ``recognize_outer_planar`` -- algorithms: ``minor_check``
* ``is_cactus`` / ``recognize_cactus`` -- algorithms: ``dfs``
* ``is_series_parallel`` / ``recognize_series_parallel`` -- algorithms: ``minor_check``, ``queue_reduction``

**Forbidden Subgraph Family**

* ``is_claw_free`` / ``recognize_claw_free`` -- algorithms: ``triple_loop``, ``edge_count``
* ``is_diamond_free`` / ``recognize_diamond_free`` -- algorithms: ``brute``, ``edge_pair``
* ``is_line_graph`` / ``recognize_line_graph`` -- algorithms: ``brute``, ``krausz``


Enumeration Functions
---------------------

Each enumeration function takes a vertex count ``n`` and generates all labeled graphs of that size.

**Chordal Family**

* ``enumerate_chordal_graphs(n)``
* ``enumerate_split_graphs(n)``
* ``enumerate_threshold_graphs(n)``
* ``enumerate_ptolemaic_graphs(n)``
* ``enumerate_trivially_perfect_graphs(n)``

**Interval Family**

* ``enumerate_interval_graphs(n)``
* ``enumerate_proper_interval_graphs(n)``

**Permutation / Comparability Family**

* ``enumerate_permutation_graphs(n)``
* ``enumerate_comparability_graphs(n)``
* ``enumerate_co_comparability_graphs(n)``

**Bipartite Family**

* ``enumerate_bipartite_graphs(n)``
* ``enumerate_bipartite_permutation_graphs(n)``
* ``enumerate_convex_bipartite_graphs(n)``
* ``enumerate_biconvex_bipartite_graphs(n)``
* ``enumerate_chain_graphs(n)``
* ``enumerate_cochain_graphs(n)``
* ``enumerate_chordal_bipartite_graphs(n)``

**Other Classes**

* ``enumerate_cograph_graphs(n)``
* ``enumerate_block_graphs(n)``
* ``enumerate_distance_hereditary_graphs(n)``
* ``enumerate_three_leaf_power_graphs(n)``

**Planar Family**

* ``enumerate_planar_graphs(n)``
* ``enumerate_outer_planar_graphs(n)``
* ``enumerate_cactus_graphs(n)``
* ``enumerate_series_parallel_graphs(n)``

**Forbidden Subgraph Family**

* ``enumerate_claw_free_graphs(n)``
* ``enumerate_diamond_free_graphs(n)``
* ``enumerate_line_graphs(n)``


Testing
-------

.. code-block:: bash

   cd python
   pip install ".[dev]"
   pytest tests/ -v
