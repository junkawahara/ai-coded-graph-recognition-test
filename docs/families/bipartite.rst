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

両列挙器の既定は ``BIPARTITE_COLORING`` です。各連結成分を二部彩色し、
新しい頂点の近傍を各成分の左右いずれかの部分集合に限定して直接生成します。
空集合は成分ごとに一度だけ生成します。これらの選択の直積により、成分の彩色を
独立に反転して結合する場合も含め、二部性を保つ追加を重複なく網羅します。
ラベル付き版ではラベル順に頂点を追加するため、ラベル展開は不要です。

ラベル付き版の ``REVERSE_SEARCH`` は従来の全近傍走査と認識器による判定、
ラベルなし版の ``CANONICAL_AUGMENTATION`` は従来の全近傍走査と彩色条件による
判定を保持します。両者とも従来の出力順序を維持します。新方式では順序や
ラベルなし代表元のラベル付けが変わる場合があります。全出力をメモリに保持します。

CLI は ``--algo bipartite_coloring``（既定）で新方式を、ラベル付き版では
``--algo reverse_search``、ラベルなし版では ``--algo canonical_augmentation``
で従来方式を選べます。ラベルなし版はどちらも ``--connected`` を受け付けます。
Python では同じ名前を ``algorithm=`` に指定し、ラベルなし版の第2引数
``connected_only`` も維持します。C++・CLI の上限はラベル付き64頂点、ラベルなし
63頂点で、``n = 0`` は連結指定の有無によらず空グラフ1個を返します。
Python の範囲はそれぞれ ``1 <= n <= 6``、``1 <= n <= 8`` のままです。

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
頂点を 1 つずつ追加してグラフを成長させられます。連結な二部グラフの
2 彩色は左右の入れ替えを除いて一意です。新しい頂点の近傍が二部性を保つ
必要十分条件は、各連結成分の二部分割の片側だけと交わることです。
候補生成ではこの条件を使います。子グラフは、追加した
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
   #include "util/graph.h"
   #include "recognizers/bipartite.h"

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
   #include "enumerators/bipartite_labeled_enum.h"

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
   #include "enumerators/bipartite_unlabeled_enum.h"

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
