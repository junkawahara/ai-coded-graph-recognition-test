Trivially Perfect グラフ
================================================

グラフが **trivially perfect** であるとは、弦グラフかつコグラフであることを
いう。同値な特徴付けとして、すべての連結な誘導部分グラフが全域頂点
(universal vertex) を持つこと、あるいは隣接関係が DFS 木における祖先・子孫
関係と一致することが挙げられる。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_327.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TriviallyPerfectAlgorithm``
     - 説明
   * - ``DFS`` **(既定)**
     - DFS に基づく認識

.. doxygenenum:: graph_recognition::TriviallyPerfectAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_trivially_perfect
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::TriviallyPerfectLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trivially_perfect_labeled_graphs_uvd
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は、根付き森との
古典的な全単射 (Wolk) を用いて、同型類ごとに代表元を 1 つだけ出力します。
根付き木の各頂点をそのすべての祖先と結ぶと連結な trivially perfect グラフが
得られ、全域頂点分解 (UVD) の一意性からこの対応は同型類上の全単射になります。
そこで ``tree_unlabeled_enum.h`` の正準根付き木を閉包し、非連結グラフは整数
分割上で連結成分を合成すれば、各同型類がちょうど 1 回ずつ現れます。個数は
OEIS A000081(n+1) (1, 2, 4, 9, 20, 48, 115, 286, ...)、``connected_only`` を
指定した場合は A000081(n) (1, 1, 2, 4, 9, 20, 48, 115, ...) です。

.. doxygenenum:: graph_recognition::TriviallyPerfectUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TriviallyPerfectUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trivially_perfect_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き trivially perfect
グラフの個数が `OEIS A058864 <https://oeis.org/A058864>`_ の値
``1, 2, 8, 49, 402`` と一致することを検証済み。非同型列挙については
``n = 9`` まで `OEIS A000081 <https://oeis.org/A000081>`_ を 1 つ
シフトした値 (``1, 2, 4, 9, 20, 48, 115, 286, 719``) と、
``connected_only`` 指定時は ``A000081`` そのもの (``1, 1, 2, 4, 9, 20,
48, 115``) と一致することを検証済み。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "trivially_perfect.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_trivially_perfect(g);

       std::cout << std::boolalpha << result.is_trivially_perfect << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "trivially_perfect_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_trivially_perfect_labeled_graphs_uvd(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* E. S. Wolk. "The comparability graph of a tree."
  *Proceedings of the American Mathematical Society*, 13(5):789--795, 1962.
  `DOI:10.1090/S0002-9939-1962-0172273-0 <https://doi.org/10.1090/S0002-9939-1962-0172273-0>`_

* M. C. Golumbic. "Trivially perfect graphs."
  *Discrete Mathematics*, 24(1):105--107, 1978.
  `DOI:10.1016/0012-365X(78)90178-4 <https://doi.org/10.1016/0012-365X(78)90178-4>`_

* T. Beyer, S. M. Hedetniemi. "Constant time generation of rooted trees."
  *SIAM Journal on Computing*, 9(4):706--712, 1980.
  `DOI:10.1137/0209055 <https://doi.org/10.1137/0209055>`_
