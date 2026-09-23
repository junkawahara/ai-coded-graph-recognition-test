三角形フリーグラフ
==========================

グラフが三角形フリーかどうかを判定する。
互いに隣接する 3 頂点を含まない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_371.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::TriangleFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_triangle_free
   :project: graph_recognition


列挙
----------

両列挙器の既定は ``INDEPENDENT_SETS`` です。新頂点の追加が三角形フリー性を
保つことと、その近傍が独立集合であることは同値です。共通の生成処理は頂点を
選ぶ・選ばないで分岐し、選ぶ場合はその隣接頂点を残りの候補から除外します。
全部分集合の走査や認識器の呼び出しを行わず、空集合・非極大集合を含む全独立集合を
1回ずつ生成します。k 頂点の親に対する近傍生成は1件 O(k) 時間・O(k) スタック領域で、
子の探索・正準化・出力構築の費用は別途必要です。

ラベル付き版はラベル順に頂点を追加し、最大ラベルの削除を一意な親規則とします。
従来の全部分集合走査と認識は ``TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH``、
ラベルなし版の全部分集合走査と独立性判定は
``TriangleFreeUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION`` で選択できます。
近傍マスクを昇順で生成するため、新方式も従来の出力順序と代表元のラベル付けを保ちます。

CLI は ``--algo independent_sets`` （既定）、ラベル付き版は ``--algo reverse_search``、
ラベルなし版は ``--algo canonical_augmentation`` に対応します。
ラベルなし版の ``--connected`` も維持します。既存の Python 関数
``enumerate_triangle_free_unlabeled_graphs(n, connected_only=False, algorithm=None)`` は
``"independent_sets"`` と ``"canonical_augmentation"`` を選択でき、``None`` は新方式です。
Python の範囲は従来どおり ``1 <= n <= 8`` です。

C++・CLI の範囲はラベル付きが ``0 <= n <= 64``、ラベルなしが ``0 <= n <= 63`` です。
n = 0 は ``connected_only`` を含め両方式とも空グラフ1個を出力します。
全出力とラベルなし探索の親ごとの子の正準形集合を保持し、正準化の費用が実用的な規模を制限します。

.. doxygenenum:: graph_recognition::TriangleFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction
path 法 (``geng -t`` の背後にある手法) です。頂点を 1 つずつ追加してグラフを
成長させるとき、新しい頂点の近傍が独立集合であることが三角形フリー性の保存
とちょうど同値なので、これが枝刈りになります。子グラフは、追加した頂点が
その子の正準ラベリングで最後に置かれる頂点の自己同型軌道に属するときに限り
採用されます。個数は OEIS A006785(n) (1, 2, 3, 7, 14, 38, 107, 410, ...)、
``connected_only`` を指定した場合は A024607(n)
(1, 1, 1, 3, 6, 19, 59, 267, ...) です。

.. doxygenenum:: graph_recognition::TriangleFreeUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriangleFreeUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TriangleFreeUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_triangle_free_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付き三角形フリー
グラフの個数が `OEIS A213434 <https://oeis.org/A213434>`_ の
``2, 7, 41, 388, 5789`` と一致することを検証した。非同型列挙については
``n = 10`` まで `OEIS A006785 <https://oeis.org/A006785>`_ の
``1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172`` と、``connected_only``
指定時は ``n = 8`` まで `OEIS A024607 <https://oeis.org/A024607>`_ の
``1, 1, 1, 3, 6, 19, 59, 267`` と一致することを検証した。


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/triangle_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_triangle_free(g);

       std::cout << std::boolalpha << result.is_triangle_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/triangle_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_triangle_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/triangle_free_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_triangle_free_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 14 = A006785(5)
       return 0;
   }


参考文献
------------

* A. Itai, M. Rodeh. "Finding a minimum circuit in a graph."
  *SIAM Journal on Computing*, 7(4):413--423, 1978.
  `DOI:10.1137/0207033 <https://doi.org/10.1137/0207033>`_
* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
