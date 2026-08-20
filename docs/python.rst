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

   from graph_recognition import is_interval, is_chordal, recognize_interval

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

* ``is_chordal`` / ``recognize_chordal`` -- アルゴリズム: ``mcs_peo``, ``bucket_mcs_peo``, ``lexbfs_peo``
* ``is_strongly_chordal`` / ``recognize_strongly_chordal`` -- アルゴリズム: ``strong_elimination``, ``peo_matrix``, ``mcs_seo``
* ``is_chordal_bipartite`` / ``recognize_chordal_bipartite`` -- アルゴリズム: ``cycle_check``, ``bisimplicial``, ``fast_bisimplicial``
* ``is_weakly_chordal`` / ``recognize_weakly_chordal`` -- アルゴリズム: ``co_chordal_bipartite``, ``complement_bfs``
* ``is_split`` / ``recognize_split`` -- アルゴリズム: ``degree_sequence``, ``hammer_simeone``
* ``is_threshold`` / ``recognize_threshold`` -- アルゴリズム: ``degree_sequence``, ``degree_sequence_fast``

**インターバルグラフ系**

* ``is_interval`` / ``recognize_interval`` -- アルゴリズム: ``backtracking``, ``at_free``
* ``is_proper_interval`` / ``recognize_proper_interval`` -- アルゴリズム: ``triple_loop_claw_check``, ``fast_claw_check``
* ``is_unit_interval`` / ``recognize_unit_interval`` -- アルゴリズム: ``proper_interval``
* ``is_co_interval`` / ``recognize_co_interval`` -- アルゴリズム: ``complement``
* ``is_circular_arc`` / ``recognize_circular_arc`` -- アルゴリズム: ``mcconnell``, ``backtracking``

**順列グラフ系**

* ``is_permutation`` / ``recognize_permutation`` -- アルゴリズム: ``backtracking``, ``class_based``
* ``is_comparability`` / ``recognize_comparability`` -- アルゴリズム: ``transitive_orientation``
* ``is_co_comparability`` / ``recognize_co_comparability`` -- アルゴリズム: ``complement``
* ``is_bipartite_permutation`` / ``recognize_bipartite_permutation`` -- アルゴリズム: ``bipartite_and_permutation``
* ``is_trapezoid`` / ``recognize_trapezoid`` -- アルゴリズム: ``chain_cover``

**二部グラフ系**

* ``is_bipartite`` / ``recognize_bipartite`` -- アルゴリズム: ``bfs``
* ``is_convex_bipartite`` / ``recognize_convex_bipartite`` -- アルゴリズム: ``brute_force``, ``c1p``
* ``is_biconvex_bipartite`` / ``recognize_biconvex_bipartite`` -- アルゴリズム: ``brute_force``, ``c1p``
* ``is_chain`` / ``recognize_chain`` -- アルゴリズム: ``neighborhood_inclusion``, ``degree_sort``
* ``is_cochain`` / ``recognize_cochain`` -- アルゴリズム: ``complement``, ``direct``

**完全グラフ**

* ``is_perfect`` / ``recognize_perfect`` -- アルゴリズム: なし (デフォルトのみ)

**禁止部分グラフ系**

* ``is_claw_free`` / ``recognize_claw_free`` -- アルゴリズム: ``triple_loop``, ``edge_count``
* ``is_diamond_free`` / ``recognize_diamond_free`` -- アルゴリズム: ``brute``, ``edge_pair``
* ``is_line_graph`` / ``recognize_line_graph`` -- アルゴリズム: ``brute``, ``krausz``

**その他**

* ``is_cograph`` / ``recognize_cograph`` -- アルゴリズム: ``cotree``, ``partition_refinement``
* ``is_block`` / ``recognize_block`` -- アルゴリズム: ``dfs``, ``chordal_diamond_free``
* ``is_distance_hereditary`` / ``recognize_distance_hereditary`` -- アルゴリズム: ``hashmap_twins``, ``sorted_twins``, ``hash_twins``
* ``is_ptolemaic`` / ``recognize_ptolemaic`` -- アルゴリズム: ``dh_hashmap``, ``dh_sorted``
* ``is_trivially_perfect`` / ``recognize_trivially_perfect`` -- アルゴリズム: ``dfs``
* ``is_quasi_threshold`` / ``recognize_quasi_threshold`` -- アルゴリズム: ``dfs``
* ``is_at_free`` / ``recognize_at_free`` -- アルゴリズム: ``brute_force``
* ``is_co_chordal`` / ``recognize_co_chordal`` -- アルゴリズム: ``complement``
* ``is_three_leaf_power`` / ``recognize_three_leaf_power`` -- アルゴリズム: なし (デフォルトのみ)

**平面グラフ系**

* ``is_planar`` / ``recognize_planar`` -- アルゴリズム: ``left_right``, ``minor_check``
* ``is_outer_planar`` / ``recognize_outer_planar`` -- アルゴリズム: ``minor_check``, ``augmented_planarity``
* ``is_cactus`` / ``recognize_cactus`` -- アルゴリズム: ``dfs``
* ``is_series_parallel`` / ``recognize_series_parallel`` -- アルゴリズム: ``minor_check``, ``queue_reduction``

列挙関数
~~~~~~~~

各列挙関数は頂点数 n を受け取り、そのラベル付きグラフを全列挙します。

**弦グラフ系**

* ``enumerate_chordal_graphs(n)`` -- 弦グラフ
* ``enumerate_split_graphs(n)`` -- スプリットグラフ
* ``enumerate_threshold_graphs(n)`` -- 閾値グラフ
* ``enumerate_ptolemaic_graphs(n)`` -- プトレマイオスグラフ
* ``enumerate_trivially_perfect_graphs(n)`` -- 自明完全グラフ

**インターバルグラフ系**

* ``enumerate_interval_graphs(n)`` -- インターバルグラフ
* ``enumerate_proper_interval_graphs(n)`` -- 固有インターバルグラフ

**順列グラフ系**

* ``enumerate_permutation_graphs(n)`` -- 順列グラフ
* ``enumerate_comparability_graphs(n)`` -- 比較可能性グラフ
* ``enumerate_co_comparability_graphs(n)`` -- 余比較可能性グラフ

**二部グラフ系**

* ``enumerate_bipartite_graphs(n)`` -- 二部グラフ
* ``enumerate_bipartite_permutation_graphs(n)`` -- 二部順列グラフ
* ``enumerate_convex_bipartite_graphs(n)`` -- 凸二部グラフ
* ``enumerate_biconvex_bipartite_graphs(n)`` -- 双凸二部グラフ
* ``enumerate_chain_graphs(n)`` -- チェーングラフ
* ``enumerate_cochain_graphs(n)`` -- 余チェーングラフ
* ``enumerate_chordal_bipartite_graphs(n)`` -- 弦二部グラフ

**その他**

* ``enumerate_cograph_graphs(n)`` -- コグラフ
* ``enumerate_block_graphs(n)`` -- ブロックグラフ
* ``enumerate_distance_hereditary_graphs(n)`` -- 距離遺伝グラフ
* ``enumerate_three_leaf_power_graphs(n)`` -- 3-leaf power グラフ

**平面グラフ系**

* ``enumerate_planar_graphs(n)`` -- 平面グラフ
* ``enumerate_outer_planar_graphs(n)`` -- 外平面グラフ
* ``enumerate_cactus_graphs(n)`` -- カクタスグラフ
* ``enumerate_series_parallel_graphs(n)`` -- 直並列グラフ

**禁止部分グラフ系**

* ``enumerate_claw_free_graphs(n)`` -- Claw-free グラフ
* ``enumerate_diamond_free_graphs(n)`` -- Diamond-free グラフ
* ``enumerate_line_graph_graphs(n)`` -- Line graph

テスト
------

.. code-block:: bash

   cd python
   pip install ".[dev]"
   pytest tests/ -v
