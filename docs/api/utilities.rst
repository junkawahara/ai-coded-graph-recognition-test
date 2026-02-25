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

lexbfs.h -- Lexicographic Breadth-First Search
-----------------------------------------------

LexBFS 順序の計算アルゴリズムを選択できます。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``LexBFSAlgorithm``
     - 説明
   * - ``SIMPLE_LEXBFS``
     - ラベルリスト (整数列) の辞書式比較で最大ラベル頂点を選択する単純実装。計算量: O(n² + nm)。
   * - ``PARTITION_LEXBFS`` **(デフォルト)**
     - Habib, McConnell, Paul, Viennot (2000) の分割細分化手法。頂点クラスの順序リストを管理し、各ステップで隣接頂点を所属クラスの前方に分離する。計算量: O(n + m)。

.. doxygenfile:: lexbfs.h
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
