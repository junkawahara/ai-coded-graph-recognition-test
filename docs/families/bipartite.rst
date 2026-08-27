二部グラフ
====================

グラフが二部グラフであるかを判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_69.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - 説明
   * - ``BFS`` **(既定)**
     - BFS により 2 彩色を試み、最初の色衝突で失敗を報告する。
       二部グラフなら 2 彩色を、そうでなければ衝突辺を閉じる奇閉路を返す。
       計算量: O(n + m)。

.. doxygenenum:: graph_recognition::BipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::BipartiteLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction
path 法 (``geng`` / ``genbg`` の同型除去方式) です。二部性は遺伝的なので、
頂点を 1 つずつ追加してグラフを成長させてよく、枝刈りは認識器の呼び出しでは
なく 2 彩色の判定になります。連結な二部グラフの 2 彩色は左右の入れ替えを
除いて一意なので、新しい頂点の近傍が二部性を保つのは、それが各連結成分の
二部分割の片側だけと交わるとき、かつそのときに限ります。子グラフは、追加した
頂点がその子の正準ラベリングで最後に置かれる頂点の自己同型軌道に属するときに
限り採用されます。個数は OEIS A033995(n) (1, 2, 3, 7, 13, 35, 88, 303, ...)、
``connected_only`` を指定した場合は A005142(n)
(1, 1, 1, 3, 5, 17, 44, 182, ...) です。

.. doxygenenum:: graph_recognition::BipartiteUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BipartiteUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付き二部グラフの個数が
`OEIS A047864 <https://oeis.org/A047864>`_ と一致することを検証した:
``2, 7, 41, 376, 5177``。非同型列挙については ``n = 10`` まで
`OEIS A033995 <https://oeis.org/A033995>`_ の
``1, 2, 3, 7, 13, 35, 88, 303, 1119, 5479`` と、``connected_only``
指定時は ``n = 8`` まで `OEIS A005142 <https://oeis.org/A005142>`_ の
``1, 1, 1, 3, 5, 17, 44, 182`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bipartite(g);

       std::cout << std::boolalpha << result.is_bipartite << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 13 = A033995(5)
       return 0;
   }


参考文献
------------

* D. König. *Theorie der endlichen und unendlichen Graphen.*
  Akademische Verlagsgesellschaft, Leipzig, 1936.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
