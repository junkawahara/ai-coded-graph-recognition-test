有向グラフ
====================

一般の有向グラフに対する認識およびユーティリティ。

認識
----------

.. doxygenenum:: graph_recognition::DigraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DigraphResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_digraph
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_digraphs
   :project: graph_recognition

上記の列挙器はラベル付き有向グラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction path 法です
(nauty スイートは ``geng n | directg`` で同じリストを得ます)。有向グラフは頂点を
1 つずつ追加して成長させ (有向グラフからどの頂点を削除しても有向グラフのままなので
すべての同型類に到達できる)、子グラフは既存の k 頂点に対する ``4^k`` 通りの
出隣接・入隣接部分集合の組のいずれかで新頂点を追加します。構成の各中間段階が
定義上単純有向グラフなので認識フィルタは不要です。同型除去には順序対ごとに
2 ビット (弧とその逆向きの弧) を詰め込む真に有向な正準形を用います。これにより
非隣接・片方向の弧・双方向の弧の組が区別されます — トーナメント列挙器が再利用する
無向の正準形は台グラフが完全グラフのときにのみ正当です。``n = 6`` (1540944 類、
約 7 秒) 程度まで実用的です。ラベル付き列挙器に合わせて ``connected_only``
フラグはありません。

.. doxygenenum:: graph_recognition::DigraphUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DigraphUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DigraphUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_digraph_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4`` について、列挙されたラベル付き有向グラフの個数が
`OEIS A053763 <https://oeis.org/A053763>`_ (コメントで ``a(n) = 2^(n^2 - n)``
が ``n`` 個のラベル付き頂点上の自己ループを持たない単純有向グラフの個数である
と述べられている) と一致することを検証した: ``1, 4, 64, 4096``。

非同型列挙は `OEIS A000273 <https://oeis.org/A000273>`_ (ラベルなし
有向グラフの個数) を再現する: 静的テストケースの ``n = 1, ..., 5`` で
``1, 3, 16, 218, 9608``、さらに ``n = 6`` の ``1540944`` (約 7 秒) を手動で
確認した。``n <= 4`` ではラベル付き列挙器の出力を正準化した同型類の集合と
完全に一致する。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "recognizers/digraph.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 1}, {2, 3}};
       auto result = check_digraph(3, arcs);

       std::cout << std::boolalpha << result.is_digraph << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/digraph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_digraphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/digraph_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_digraph_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';  // 218
       return 0;
   }


参考文献
------------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
