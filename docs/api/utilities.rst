ユーティリティ
==============

dsu.h -- Union-Find
-------------------

.. doxygenfile:: dsu.h
   :project: graph_recognition

mcs.h -- Maximum Cardinality Search
------------------------------------

.. doxygenfile:: mcs.h
   :project: graph_recognition

clique.h -- 極大クリーク列挙・クリーク木
-----------------------------------------

クリーク木の構築アルゴリズムを選択できます。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CliqueTreeAlgorithm``
     - 説明
   * - ``KRUSKAL``
     - 最大重みスパニング木 (Kruskal 法) によるクリーク木構築。極大クリーク間の共通頂点数を重みとして最大全域木を構築する。
   * - ``INCREMENTAL`` **(デフォルト)**
     - PEO (Perfect Elimination Ordering) 順にインクリメンタルにクリーク木を構築する。PEO の各頂点を順に処理し、新しい極大クリークを検出したら木に追加する。

.. doxygenfile:: clique.h
   :project: graph_recognition

minor.h -- マイナーチェック
---------------------------

.. doxygenfile:: minor.h
   :project: graph_recognition
