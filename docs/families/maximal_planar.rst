極大平面グラフ
====================

**極大平面グラフ** (三角形分割) は、どの辺を追加しても平面性が失われる平面グラフである。
極大平面グラフでは、(外面を含む) すべての面が三角形になる。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_981.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::MaximalPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_planar
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::MaximalPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは平面グラフの列挙器と同じ機構による
McKay の canonical construction path 法です。このクラス自体は遺伝的ではありません
(三角形分割から頂点を削除すると、平面グラフではあるものの辺極大ではなくなります) が、
頂点を削除した部分グラフは常に平面グラフなので、探索は*平面グラフ*を頂点 1 つずつ
成長させ、辺数が ``3n - 6`` に達した n 頂点グラフだけを出力します。候補となる
子グラフごとに ``check_planar`` 呼び出しと辺数ウィンドウ (残りの頂点で
``3n - 6`` 本に到達できること) で枝刈りします。どちらの条件も同型不変量なので、
McKay の「1 クラスにつき親は 1 つ」という議論はそのまま成り立ちます。
このクラスの専用生成器は plantri (Brinkmann, McKay) による、埋め込まれた平面
三角形分割そのものの上での canonical construction path (毎秒数百万グラフ) ですが、
本実装は共有の canonical-augmentation 機構を再利用する方式で、``n = 10`` 程度まで
実用的です。個数は n = 1, 2 で 1 (K1, K2)、n = 3 以降は OEIS A000109
(1, 1, 1, 2, 5, 14, 50, 233, ...) です。極大平面グラフはすべて連結なので、
``connected_only`` フラグはありません。

.. doxygenenum:: graph_recognition::MaximalPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_unlabeled_graphs
   :project: graph_recognition

カウント検証
--------------------------------

非同型列挙は ``n = 10`` まで OEIS A000109 を再現する (A000109(10) = 233、
約 16 秒)。また ``n <= 6`` では、ラベル付き逆探索列挙器の出力を正準化した
同型類の集合と完全に一致する。静的テストケースは ``n = 9`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "maximal_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_maximal_planar(g);

       std::cout << std::boolalpha << result.is_maximal_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "maximal_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "maximal_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 14
       return 0;
   }


参考文献
------------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
