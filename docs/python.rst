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
* ``is_strongly_chordal`` / ``recognize_strongly_chordal`` -- アルゴリズム: ``strong_elimination``, ``peo_matrix``, ``mcs_seo``, ``farber_seo``
* ``is_chordal_bipartite`` / ``recognize_chordal_bipartite`` -- アルゴリズム: ``cycle_check``, ``bisimplicial``, ``fast_bisimplicial``
* ``is_weakly_chordal`` / ``recognize_weakly_chordal`` -- アルゴリズム: ``co_chordal_bipartite``, ``complement_bfs``
* ``is_split`` / ``recognize_split`` -- アルゴリズム: ``degree_sequence``, ``hammer_simeone``
* ``is_threshold`` / ``recognize_threshold`` -- アルゴリズム: ``degree_sequence``, ``degree_sequence_fast``

**インターバルグラフ系**

* ``is_interval`` / ``recognize_interval`` -- アルゴリズム: ``backtracking``, ``at_free``, ``pq_tree``
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

**基本構造**

* ``is_tree`` / ``recognize_tree`` -- アルゴリズム: ``bfs``
* ``is_forest`` / ``recognize_forest`` -- アルゴリズム: ``bfs``
* ``is_caterpillar`` / ``recognize_caterpillar`` -- アルゴリズム: ``leaf_removal``
* ``is_unicyclic`` / ``recognize_unicyclic`` -- アルゴリズム: ``bfs``
* ``is_cluster`` / ``recognize_cluster`` -- アルゴリズム: ``component_clique``
* ``is_biconnected`` / ``recognize_biconnected`` -- アルゴリズム: ``dfs``, ``block_cut_tree``
* ``is_triconnected`` / ``recognize_triconnected`` -- アルゴリズム: ``naive``
* ``is_eulerian`` / ``recognize_eulerian`` -- アルゴリズム: ``degree_check``
* ``is_cubic`` / ``recognize_cubic`` -- アルゴリズム: ``degree_check``
* ``is_kregular`` / ``recognize_kregular`` -- アルゴリズム: ``degree_check``
* ``is_strongly_regular`` / ``recognize_strongly_regular`` -- アルゴリズム: ``parameter_check``
* ``is_self_complementary`` / ``recognize_self_complementary`` -- アルゴリズム: ``isomorphism_check``
* ``is_ktree`` / ``recognize_ktree`` -- アルゴリズム: ``simplicial_removal``
* ``is_laman`` / ``recognize_laman`` -- アルゴリズム: ``pebble_game``

**平面グラフ族 (追加)**

* ``is_maximal_planar`` / ``recognize_maximal_planar`` -- アルゴリズム: ``planar_edge_count``
* ``is_maximal_outer_planar`` / ``recognize_maximal_outer_planar`` -- アルゴリズム: ``outer_planar_edge_count``
* ``is_apollonian`` / ``recognize_apollonian`` -- アルゴリズム: ``maximal_planar_chordal``
* ``is_cubic_planar`` / ``recognize_cubic_planar`` -- アルゴリズム: ``cubic_and_planar``
* ``is_polyhedral`` / ``recognize_polyhedral`` -- アルゴリズム: ``steinitz``
* ``is_apex`` / ``recognize_apex`` -- アルゴリズム: ``vertex_deletion``
* ``is_halin`` / ``recognize_halin`` -- アルゴリズム: ``face_check``
* ``is_fullerene`` / ``recognize_fullerene`` -- アルゴリズム: ``face_check``
* ``is_simple_quadrangulation`` / ``recognize_simple_quadrangulation`` -- アルゴリズム: ``combined``
* ``is_snark`` / ``recognize_snark`` -- アルゴリズム: ``combined``

**禁止部分グラフ族 (追加)**

* ``is_triangle_free`` / ``recognize_triangle_free`` -- アルゴリズム: ``edge_pair``, ``brute``
* ``is_bull_free`` / ``recognize_bull_free`` -- アルゴリズム: ``triangle_search``, ``brute``
* ``is_gem_free`` / ``recognize_gem_free`` -- アルゴリズム: ``neighbor_p4_search``, ``brute``
* ``is_p5_free`` / ``recognize_p5_free`` -- アルゴリズム: ``path_search``, ``brute``
* ``is_even_hole_free`` / ``recognize_even_hole_free``
* ``is_odd_hole_free`` / ``recognize_odd_hole_free``
* ``is_meyniel`` / ``recognize_meyniel`` -- アルゴリズム: ``direct_check``
* ``is_parity`` / ``recognize_parity`` -- アルゴリズム: ``direct_check``

**Leaf power / 順序系 (追加)**

* ``is_four_leaf_power`` / ``recognize_four_leaf_power``
* ``is_five_leaf_power`` / ``recognize_five_leaf_power``
* ``is_proper_chordal`` / ``recognize_proper_chordal``
* ``is_proper_circular_arc`` / ``recognize_proper_circular_arc``
* ``is_circle`` / ``recognize_circle`` -- アルゴリズム: ``naji_system``, ``dow_backtracking``

**有向入力**

以下の認識器では辺リストを**有向**の弧として解釈します
(``is_poset`` の弧は被覆関係で、``(u, v)`` は ``v`` が ``u`` を
被覆することを意味します):

* ``is_digraph`` / ``recognize_digraph`` -- アルゴリズム: ``validity_check``
* ``is_poset`` / ``recognize_poset`` -- アルゴリズム: ``dag_and_reduction``
* ``is_tournament`` / ``recognize_tournament`` -- アルゴリズム: ``arc_check``

**その他**

* ``is_cograph`` / ``recognize_cograph`` -- アルゴリズム: ``cotree``, ``partition_refinement``, ``modular``
* ``is_block`` / ``recognize_block`` -- アルゴリズム: ``dfs``, ``chordal_diamond_free``, ``block_cut_tree``
* ``is_distance_hereditary`` / ``recognize_distance_hereditary`` -- アルゴリズム: ``hashmap_twins``, ``sorted_twins``, ``hash_twins``, ``split_decomposition``
* ``is_ptolemaic`` / ``recognize_ptolemaic`` -- アルゴリズム: ``dh_hashmap``, ``dh_sorted``
* ``is_trivially_perfect`` / ``recognize_trivially_perfect`` -- アルゴリズム: ``dfs``
* ``is_quasi_threshold`` / ``recognize_quasi_threshold`` -- アルゴリズム: ``dfs``
* ``is_at_free`` / ``recognize_at_free`` -- アルゴリズム: ``brute_force``
* ``is_co_chordal`` / ``recognize_co_chordal`` -- アルゴリズム: ``complement``
* ``is_three_leaf_power`` / ``recognize_three_leaf_power`` -- アルゴリズム: なし (デフォルトのみ)

**平面グラフ系**

* ``is_planar`` / ``recognize_planar`` -- アルゴリズム: ``left_right``, ``minor_check``
* ``is_outer_planar`` / ``recognize_outer_planar`` -- アルゴリズム: ``minor_check``, ``augmented_planarity``
* ``is_cactus`` / ``recognize_cactus`` -- アルゴリズム: ``dfs``, ``block_cut_tree``
* ``is_series_parallel`` / ``recognize_series_parallel`` -- アルゴリズム: ``minor_check``, ``queue_reduction``

列挙関数
~~~~~~~~

各列挙関数は頂点数 n を受け取ります。名前に ``_labeled_`` を含む関数は
ラベル付きグラフを列挙し、``_unlabeled_`` を含む関数は同型類ごとに代表元を
1 つ列挙します。
n の上限はラベル付き列挙が 6 (``ENUM_MAX_N``)、非同型列挙が 8
(``UNLABELED_ENUM_MAX_N``) で、それを超えると ``ValueError`` を送出します
(結果全体をメモリ上に構築するため。ラベル付きグラフ数は超指数的に爆発します。
より大きい n はストリーミング出力の C++ CLI を使用してください)。同型類ごとに
代表元 1 つだけを返す非同型列挙は出力がはるかに小さいため、上限が高くなって
います。

**弦グラフ系**

* ``enumerate_chordal_labeled_graphs(n)`` -- 弦グラフ
* ``enumerate_strongly_chordal_labeled_graphs(n)`` -- 強弦グラフ
* ``enumerate_weakly_chordal_labeled_graphs(n)`` -- 弱弦グラフ
* ``enumerate_split_labeled_graphs(n)`` -- スプリットグラフ
* ``enumerate_threshold_unlabeled_graphs(n)`` -- 閾値グラフ
* ``enumerate_ptolemaic_labeled_graphs(n)`` -- プトレマイオスグラフ
* ``enumerate_trivially_perfect_labeled_graphs(n)`` -- 自明完全グラフ

**インターバルグラフ系**

* ``enumerate_interval_labeled_graphs(n)`` -- インターバルグラフ
* ``enumerate_proper_interval_labeled_graphs(n)`` -- 固有インターバルグラフ
* ``enumerate_co_interval_labeled_graphs(n)`` -- 余インターバルグラフ
* ``enumerate_circular_arc_labeled_graphs(n)`` -- 円弧グラフ

**順列グラフ系**

* ``enumerate_permutation_labeled_graphs(n)`` -- 順列グラフ
* ``enumerate_comparability_labeled_graphs(n)`` -- 比較可能性グラフ
* ``enumerate_co_comparability_labeled_graphs(n)`` -- 余比較可能性グラフ
* ``enumerate_trapezoid_labeled_graphs(n)`` -- 台形グラフ

**二部グラフ系**

* ``enumerate_bipartite_labeled_graphs(n)`` -- 二部グラフ
* ``enumerate_bipartite_permutation_labeled_graphs(n)`` -- 二部順列グラフ
* ``enumerate_convex_bipartite_labeled_graphs(n)`` -- 凸二部グラフ
* ``enumerate_biconvex_bipartite_labeled_graphs(n)`` -- 双凸二部グラフ
* ``enumerate_chain_unlabeled_graphs(n)`` -- チェーングラフ
* ``enumerate_cochain_unlabeled_graphs(n)`` -- 余チェーングラフ
* ``enumerate_chordal_bipartite_labeled_graphs(n)`` -- 弦二部グラフ

**完全グラフ**

* ``enumerate_perfect_labeled_graphs(n)`` -- パーフェクトグラフ

**その他**

* ``enumerate_cograph_labeled_graphs(n)`` -- コグラフ
* ``enumerate_block_labeled_graphs(n)`` -- ブロックグラフ
* ``enumerate_distance_hereditary_labeled_graphs(n)`` -- 距離遺伝グラフ
* ``enumerate_three_leaf_power_labeled_graphs(n)`` -- 3-leaf power グラフ
* ``enumerate_at_free_labeled_graphs(n)`` -- AT-free グラフ
* ``enumerate_co_chordal_labeled_graphs(n)`` -- 余弦グラフ

**平面グラフ系**

* ``enumerate_planar_labeled_graphs(n)`` -- 平面グラフ
* ``enumerate_outer_planar_labeled_graphs(n)`` -- 外平面グラフ
* ``enumerate_cactus_labeled_graphs(n)`` -- カクタスグラフ
* ``enumerate_series_parallel_labeled_graphs(n)`` -- 直並列グラフ

**禁止部分グラフ系**

* ``enumerate_claw_free_labeled_graphs(n)`` -- Claw-free グラフ
* ``enumerate_diamond_free_labeled_graphs(n)`` -- Diamond-free グラフ
* ``enumerate_line_graph_labeled_graphs(n)`` -- Line graph

**追加の非同型列挙**

* ``enumerate_apollonian_unlabeled_graphs(n)``
* ``enumerate_biconnected_unlabeled_graphs(n)``
* ``enumerate_bipartite_permutation_unlabeled_graphs(n)``
* ``enumerate_bipartite_unlabeled_graphs(n)``
* ``enumerate_cactus_unlabeled_graphs(n)``
* ``enumerate_chordal_unlabeled_graphs(n)``
* ``enumerate_circle_unlabeled_graphs(n)``
* ``enumerate_cluster_unlabeled_graphs(n)``
* ``enumerate_co_chordal_unlabeled_graphs(n)``
* ``enumerate_co_comparability_unlabeled_graphs(n)``
* ``enumerate_co_interval_unlabeled_graphs(n)``
* ``enumerate_cograph_unlabeled_graphs(n)``
* ``enumerate_comparability_unlabeled_graphs(n)``
* ``enumerate_cubic_planar_unlabeled_graphs(n)``
* ``enumerate_cubic_unlabeled_graphs(n)``
* ``enumerate_distance_hereditary_unlabeled_graphs(n)``
* ``enumerate_eulerian_unlabeled_graphs(n)``
* ``enumerate_interval_unlabeled_graphs(n)``
* ``enumerate_maximal_outer_planar_unlabeled_graphs(n)``
* ``enumerate_maximal_planar_unlabeled_graphs(n)``
* ``enumerate_outer_planar_unlabeled_graphs(n)``
* ``enumerate_permutation_unlabeled_graphs(n)``
* ``enumerate_planar_unlabeled_graphs(n)``
* ``enumerate_polyhedral_unlabeled_graphs(n)``
* ``enumerate_proper_interval_unlabeled_graphs(n)``
* ``enumerate_ptolemaic_unlabeled_graphs(n)``
* ``enumerate_self_complementary_unlabeled_graphs(n)``
* ``enumerate_series_parallel_unlabeled_graphs(n)``
* ``enumerate_split_unlabeled_graphs(n)``
* ``enumerate_three_leaf_power_unlabeled_graphs(n)``
* ``enumerate_triangle_free_unlabeled_graphs(n)``
* ``enumerate_trivially_perfect_unlabeled_graphs(n)``

部分グラフ列挙関数
~~~~~~~~~~~~~~~~~~

頂点数ではなく\ **ホストグラフ**\ を受け取り、そのグラフに含まれるクラスの部分グラフを
列挙します。入力形式は認識関数と同じ ``(n, edges)`` または ``networkx.Graph`` です。

* ``enumerate_chordal_subgraphs(n_or_graph, edges=None)`` -- 弦部分グラフ

ここでの部分グラフは全域部分グラフ ``(V, E')``（``E'`` はホストの辺集合の部分集合）
です。頂点集合は固定され孤立頂点も保持されるため、結果はクラスに属する辺部分集合と
1 対 1 に対応し、空の辺集合も常に含まれます。

上限は頂点数ではなく辺数にかかります。森の部分グラフはすべて弦グラフなので、辺数
``m`` のホストは最大 ``2^m`` 個の弦部分グラフを持ちうるためです。相異なる辺が 16 本
(``ENUM_MAX_M``) を超えるホストは ``ValueError`` を送出します。それより大きい
ホストにはストリーミング出力の C++ API を使用してください。

.. code-block:: python

   from graph_recognition import enumerate_chordal_subgraphs

   # C4: 自身以外のすべての辺部分集合が弦グラフ
   len(enumerate_chordal_subgraphs(4, [(1, 2), (2, 3), (3, 4), (4, 1)]))
   # 15

誘導部分グラフ列挙関数
~~~~~~~~~~~~~~~~~~~~~~

こちらも\ **ホストグラフ**\ を受け取りますが、解は ``G[X]`` がクラスに属する頂点
部分集合 ``X`` なので、戻り値は辺リストではなく頂点リストのリストです。

* ``enumerate_chordal_bipartite_induced_subgraphs(n_or_graph, edges=None)``
  -- 弦二部誘導部分グラフ

各頂点集合は昇順にソートされており、空集合が常に先頭に来ます。

上限は辺数ではなく頂点数にかかります。これらのクラスは遺伝的なので、ホスト自身が
クラスに属していれば、どれだけ疎であっても ``2^n`` 個の頂点部分集合がすべて解に
なるためです。頂点数が 16 (``INDUCED_ENUM_MAX_N``) を超えるホストは ``ValueError``
を送出します。それより大きいホストにはストリーミング出力の C++ API を使用して
ください。

.. code-block:: python

   from graph_recognition import enumerate_chordal_bipartite_induced_subgraphs

   # C6: 全体以外のすべての頂点部分集合が弦二部
   len(enumerate_chordal_bipartite_induced_subgraphs(
       6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)]))
   # 63

テスト
------

.. code-block:: bash

   cd python
   pip install ".[dev]"
   pytest tests/ -v
