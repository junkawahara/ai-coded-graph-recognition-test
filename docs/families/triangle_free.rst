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
