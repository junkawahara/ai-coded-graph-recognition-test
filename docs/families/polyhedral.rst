多面体グラフ
====================

**多面体グラフ** は 3-連結平面グラフである。Steinitz の定理により、
これはちょうど凸多面体のグラフに一致する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_986.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::PolyhedralAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_polyhedral
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::PolyhedralLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_labeled_graphs
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは平面グラフの列挙器と同じ機構による
McKay の canonical construction path 法です。このクラス自体は遺伝的ではありません
(頂点を削除すると 3-連結性が壊れうる) が、頂点を削除した部分グラフは常に
平面グラフなので、探索は*平面グラフ*を頂点 1 つずつ成長させ、n 頂点の
グラフに対してのみ ``check_polyhedral`` による完全な検査を行います。候補となる
子グラフごとに ``check_planar`` 呼び出しと辺数ウィンドウ (n 頂点の多面体グラフは
最小次数 3 と平面性により ``ceil(3n/2) <= m <= 3n - 6``) で枝刈りします。
どちらの条件も同型不変量なので、McKay の「1 クラスにつき親は 1 つ」という
議論はそのまま成り立ちます。このクラスの専用生成器は plantri (Brinkmann, McKay)
による、埋め込まれた 3-連結平面グラフそのものの上での canonical construction path
(毎秒数百万グラフ) ですが、本実装は共有の canonical-augmentation 機構を再利用する
方式で、``n = 9`` 程度まで実用的です。個数は n = 4 以降で OEIS A000944
(1, 2, 7, 34, 257, 2606, ...) です。4 頂点未満の 3-連結グラフは存在しないので、
それより小さい n では何も出力されません。多面体グラフはすべて連結なので、
``connected_only`` フラグはありません。

.. doxygenenum:: graph_recognition::PolyhedralUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 4, 5, 6`` について、列挙されたラベル付き多面体グラフの個数が
`OEIS A096330 <https://oeis.org/A096330>`_ の値 ``1, 25, 1227`` と
一致することを検証した。

非同型列挙は ``n = 9`` まで `OEIS A000944 <https://oeis.org/A000944>`_ を
再現する (A000944(9) = 2606、約 14 秒)。また ``n <= 6`` では、ラベル付き
逆探索列挙器の出力を正準化した同型類の集合と完全に一致する。静的テストケースは
``n = 8`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/polyhedral.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_polyhedral(g);

       std::cout << std::boolalpha << result.is_polyhedral << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 7
       return 0;
   }


参考文献
------------

* E. Steinitz. "Polyeder und Raumeinteilungen."
  *Encyclopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.

* B. Grünbaum. *Convex Polytopes.*
  Graduate Texts in Mathematics 221, Springer, 2nd edition, 2003.
  `DOI:10.1007/978-1-4613-0019-9 <https://doi.org/10.1007/978-1-4613-0019-9>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
