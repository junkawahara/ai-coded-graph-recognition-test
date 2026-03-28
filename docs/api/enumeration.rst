列挙アルゴリズム
================

各列挙関数は頂点数 n を受け取り、その頂点数のラベル付きグラフを全列挙します。
主に逆探索 (reverse search) に基づく手法を用います。


.. _chordal-enum:

弦グラフ系
----------

chordal_enum.h -- 弦グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chordal_enum.h
   :project: graph_recognition


split_enum.h -- スプリットグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: split_enum.h
   :project: graph_recognition


threshold_enum.h -- 閾値グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: threshold_enum.h
   :project: graph_recognition


ptolemaic_enum.h -- プトレマイオスグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: ptolemaic_enum.h
   :project: graph_recognition


trivially_perfect_enum.h -- 自明完全グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: trivially_perfect_enum.h
   :project: graph_recognition


.. _interval-enum:

インターバルグラフ系
--------------------

interval_enum.h -- インターバルグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: interval_enum.h
   :project: graph_recognition


proper_interval_enum.h -- 固有インターバルグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: proper_interval_enum.h
   :project: graph_recognition


.. _permutation-enum:

順列グラフ系
------------

permutation_enum.h -- 順列グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: permutation_enum.h
   :project: graph_recognition


comparability_enum.h -- 比較可能性グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: comparability_enum.h
   :project: graph_recognition


co_comparability_enum.h -- 余比較可能性グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: co_comparability_enum.h
   :project: graph_recognition


.. _bipartite-enum:

二部グラフ系
------------

bipartite_enum.h -- 二部グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: bipartite_enum.h
   :project: graph_recognition


bipartite_permutation_enum.h -- 二部順列グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: bipartite_permutation_enum.h
   :project: graph_recognition


convex_bipartite_enum.h -- 凸二部グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: convex_bipartite_enum.h
   :project: graph_recognition


biconvex_bipartite_enum.h -- 双凸二部グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: biconvex_bipartite_enum.h
   :project: graph_recognition


chain_enum.h -- チェーングラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chain_enum.h
   :project: graph_recognition


cochain_enum.h -- 余チェーングラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: cochain_enum.h
   :project: graph_recognition


chordal_bipartite_enum.h -- 弦二部グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chordal_bipartite_enum.h
   :project: graph_recognition


.. _other-enum:

その他のグラフクラス
--------------------

cograph_enum.h -- コグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: cograph_enum.h
   :project: graph_recognition


block_enum.h -- ブロックグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: block_enum.h
   :project: graph_recognition


distance_hereditary_enum.h -- 距離遺伝グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: distance_hereditary_enum.h
   :project: graph_recognition


three_leaf_power_enum.h -- 3-leaf power グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: three_leaf_power_enum.h
   :project: graph_recognition


.. _planar-enum:

平面グラフ系
------------

planar_enum.h -- 平面グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: planar_enum.h
   :project: graph_recognition


outer_planar_enum.h -- 外平面グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: outer_planar_enum.h
   :project: graph_recognition


cactus_enum.h -- カクタスグラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: cactus_enum.h
   :project: graph_recognition


series_parallel_enum.h -- 直並列グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: series_parallel_enum.h
   :project: graph_recognition


.. _forbidden-enum:

禁止部分グラフ系
----------------

claw_free_enum.h -- Claw-free グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: claw_free_enum.h
   :project: graph_recognition


diamond_free_enum.h -- Diamond-free グラフの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: diamond_free_enum.h
   :project: graph_recognition


line_graph_enum.h -- Line graph の列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: line_graph_enum.h
   :project: graph_recognition
