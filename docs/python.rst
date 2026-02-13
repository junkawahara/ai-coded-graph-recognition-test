Python API
==========

Graph Recognition Library の Python バインディング。
pybind11 を使用して C++ 認識アルゴリズムを Python から呼び出す。

インストール
------------

.. code-block:: bash

   pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

   # NetworkX サポート付き
   pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

   # ローカルビルド (開発用)
   cd python
   pip install ".[dev]"

クイックスタート
----------------

.. code-block:: python

   from graph_recognition import is_interval, is_chordal

   # (n, edges) 形式 (1-indexed)
   is_interval(4, [(1, 2), (2, 3), (3, 4)])  # True
   is_interval(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

   # アルゴリズム選択
   is_chordal(4, [(1, 2), (2, 3), (3, 4)], algorithm="mcs_peo")

   # recognize は将来の証拠返却に備えた関数
   result, cert = recognize_interval(4, [(1, 2), (2, 3), (3, 4)])

NetworkX 連携
~~~~~~~~~~~~~

.. code-block:: python

   import networkx as nx
   from graph_recognition import is_interval

   G = nx.path_graph(5)
   is_interval(G)  # True

   # 任意のノード型をサポート
   G = nx.Graph()
   G.add_edges_from([("a", "b"), ("b", "c")])
   is_interval(G)  # True

関数一覧
--------

認識関数
~~~~~~~~

各グラフクラスに対して ``is_<type>`` と ``recognize_<type>`` の 2 種類を提供。

* ``is_<type>(n_or_graph, edges=None, *, algorithm=None) -> bool``
* ``recognize_<type>(n_or_graph, edges=None, *, algorithm=None) -> (bool, dict | None)``

**弦グラフ系**

* ``is_chordal`` / ``recognize_chordal`` -- アルゴリズム: ``mcs_peo``, ``bucket_mcs_peo``
* ``is_strongly_chordal`` / ``recognize_strongly_chordal`` -- アルゴリズム: ``strong_elimination``, ``peo_matrix``
* ``is_chordal_bipartite`` / ``recognize_chordal_bipartite`` -- アルゴリズム: ``cycle_check``, ``bisimplicial``, ``fast_bisimplicial``
* ``is_weakly_chordal`` / ``recognize_weakly_chordal`` -- アルゴリズム: ``co_chordal_bipartite``, ``complement_bfs``
* ``is_split`` / ``recognize_split`` -- アルゴリズム: ``degree_sequence``, ``hammer_simeone``
* ``is_threshold`` / ``recognize_threshold`` -- アルゴリズム: ``degree_sequence``, ``degree_sequence_fast``

**インターバルグラフ系**

* ``is_interval`` / ``recognize_interval`` -- アルゴリズム: ``backtracking``, ``at_free``
* ``is_proper_interval`` / ``recognize_proper_interval`` -- アルゴリズム: ``pq_tree``, ``fast_claw_check``
* ``is_unit_interval`` / ``recognize_unit_interval`` -- アルゴリズム: ``proper_interval``
* ``is_co_interval`` / ``recognize_co_interval`` -- アルゴリズム: ``complement``
* ``is_circular_arc`` / ``recognize_circular_arc`` -- アルゴリズム: ``mcconnell``, ``backtracking``

**順列グラフ系**

* ``is_permutation`` / ``recognize_permutation`` -- アルゴリズム: ``backtracking``, ``class_based``
* ``is_comparability`` / ``recognize_comparability`` -- アルゴリズム: ``transitive_orientation``
* ``is_co_comparability`` / ``recognize_co_comparability`` -- アルゴリズム: ``complement``
* ``is_bipartite_permutation`` / ``recognize_bipartite_permutation`` -- アルゴリズム: ``chain_both_sides``

**二部グラフ系**

* ``is_bipartite`` / ``recognize_bipartite`` -- アルゴリズム: ``bfs``
* ``is_chain`` / ``recognize_chain`` -- アルゴリズム: ``neighborhood_inclusion``, ``degree_sort``
* ``is_cochain`` / ``recognize_cochain`` -- アルゴリズム: ``complement``, ``direct``

**その他**

* ``is_cograph`` / ``recognize_cograph`` -- アルゴリズム: ``cotree``, ``partition_refinement``
* ``is_block`` / ``recognize_block`` -- アルゴリズム: ``dfs``
* ``is_distance_hereditary`` / ``recognize_distance_hereditary`` -- アルゴリズム: ``hashmap_twins``, ``sorted_twins``, ``hash_twins``
* ``is_ptolemaic`` / ``recognize_ptolemaic`` -- アルゴリズム: ``dh_hashmap``, ``dh_sorted``
* ``is_trivially_perfect`` / ``recognize_trivially_perfect`` -- アルゴリズム: ``dfs``
* ``is_quasi_threshold`` / ``recognize_quasi_threshold`` -- アルゴリズム: ``dfs``
* ``is_at_free`` / ``recognize_at_free`` -- アルゴリズム: ``brute_force``
* ``is_co_chordal`` / ``recognize_co_chordal`` -- アルゴリズム: ``complement``

**平面グラフ系**

* ``is_planar`` / ``recognize_planar`` -- アルゴリズム: ``minor_check``
* ``is_outer_planar`` / ``recognize_outer_planar`` -- アルゴリズム: ``minor_check``
* ``is_cactus`` / ``recognize_cactus`` -- アルゴリズム: ``dfs``
* ``is_series_parallel`` / ``recognize_series_parallel`` -- アルゴリズム: ``minor_check``, ``queue_reduction``

列挙関数
~~~~~~~~

* ``enumerate_chordal_graphs(n)`` -- 頂点数 n のラベル付き弦グラフを全列挙

テスト
------

.. code-block:: bash

   cd python
   pip install ".[dev]"
   pytest tests/ -v
