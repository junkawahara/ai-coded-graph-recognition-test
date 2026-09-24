ブロックグラフ
============================

グラフが **ブロックグラフ** であるとは、すべての 2 連結成分がクリークで
あることをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_93.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - 説明
   * - ``DFS`` **(既定)**
     - DFS による 2 連結成分の検査、O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - 弦グラフ性 + diamond-free による認識、O(n + m Delta^2)
   * - ``BLOCK_CUT_TREE``
     - 共有のブロック分解 (block-cut tree) を構築し、各ブロックがクリークで
       あることを検査する、O(n + m)

.. doxygenenum:: graph_recognition::BlockAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_block
   :project: graph_recognition


列挙
------------

両列挙器の既定は Ekim--Shalom--Yirik (2024) の ``BLOCK_GRAPH_GEN`` です。
重み1の関節点ノードと、非関節点数を重みとするブロックノードの交互木を
生成します。部分木の多重集合と、重心が1個・2個・3個の場合分けによって
同型重複を除き、重み0の内部ブロックも扱います。各ブロックをクリークに
復元し、連結成分の多重集合から非連結グラフを構成します。ラベル付き版は
各代表を ``n! / |Aut(G)|`` 個の異なるラベル付きグラフに展開します。

ラベル付き版の ``REVERSE_SEARCH`` は従来の弦グラフ逆探索とブロック認識を
残し、その出力順も維持します。既存の関数名は変わりません。方式によって
出力順は異なります。ラベルなし版は ``connected_only`` に対応します。
C++・CLI はともに ``0 <= n <= max_vertex_count`` を受け付け、ビットマスクの
制限はありません。0頂点では空グラフ1個、連結指定では0個を返します。

重みが高々 ``floor(n/2)`` の根付き部分木を保持し、成分合成では連結代表も
保持します。辺リスト1件の復元・整列は ``O(n + m log(m+1))`` で、木生成と
ラベル展開には別途費用が必要です。全出力とラベル展開の訪問済み集合を
メモリに保持し、論文の木表現に対する線形遅延は主張しません。

.. code-block:: bash

   echo "6" | ./bin/block_labeled_enum --algo block_graph_gen
   echo "6" | ./bin/block_labeled_enum --algo reverse_search
   echo "6" | ./bin/block_unlabeled_enum --connected --algo block_graph_gen

Python は ``enumerate_block_labeled_graphs(n, algorithm="block_graph_gen")`` と
``enumerate_block_unlabeled_graphs(n, connected_only=False,
algorithm="block_graph_gen")`` を公開します。ラベル付き版の従来方式は
``algorithm="reverse_search"`` で選べます。Python の範囲はラベル付きが1〜6頂点、
ラベルなしが1〜8頂点です。6頂点ではラベル付き7,739件、ラベルなし50件、
連結ラベルなし22件です。

.. doxygenenum:: graph_recognition::BlockLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenenum:: graph_recognition::BlockUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BlockUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/block.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_block(g);

       std::cout << std::boolalpha << result.is_block << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/block_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_block_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* T. Ekim, M. Shalom, M. A. Yirik. "Generation of weighted trees, block trees
  and block graphs." 2024.
  `arXiv:2401.09764 <https://arxiv.org/abs/2401.09764>`_
* F. Harary. "A characterization of block-graphs."
  *Canadian Mathematical Bulletin*, 6(1):1--6, 1963.
  `DOI:10.4153/CMB-1963-001-x <https://doi.org/10.4153/CMB-1963-001-x>`_
