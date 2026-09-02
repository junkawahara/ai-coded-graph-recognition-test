2-連結グラフ
========================

グラフが 2-連結 (biconnected) であるかどうかを判定する。
2-連結グラフとは、頂点数が 3 以上で切断点を持たない連結グラフである。
頂点数が 3 未満のグラフは 2-連結でないと判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_771.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::BiconnectedAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconnected
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::BiconnectedLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_labeled_graphs
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction
path 法に geng ``-C`` 型の連結性制約を組み合わせたものです。2-連結グラフは
遺伝的ではありません (頂点を削除するとグラフが非連結になり得ます) が、頂点を
1 つずつ追加する探索の中間レベルではすべてのグラフを生成します。代わりに
2-連結性は最後の 2 レベルを制約します: 2-連結グラフ ``G`` では任意の頂点
``v`` について ``G - v`` が連結なので、レベル ``n - 1`` では連結グラフのみを
生成し、最後の頂点は次数 2 以上を持ち、かつ次数が 2 未満のすべての頂点を
近傍に含まなければなりません (最終グラフの最小次数は 2 です)。これらの条件は
必要条件であって十分条件ではないため、出力時に切断点の完全な検査を行います。
子グラフは、追加した頂点がその子の正準ラベリングで最後に置かれる頂点の
自己同型軌道に属するときに限り採用されます。2-連結グラフは定義により連結
なので ``connected_only`` フラグはありません。個数は ``n = 9`` まで
`OEIS A002218 <https://oeis.org/A002218>`_ の
``1, 3, 10, 56, 468, 7123, 194066`` (``n = 3, 4, ...``) と一致することを
検証し、``n = 6`` まではラベル付き列挙器の出力を正準化した集合とも照合した。
静的テストケースは ``n = 8`` までである。

.. doxygenenum:: graph_recognition::BiconnectedUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BiconnectedUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/biconnected.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_biconnected(g);

       std::cout << std::boolalpha << result.is_biconnected << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/biconnected_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/biconnected_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 56 = A002218(6)
       return 0;
   }


参考文献
----------------

* R. Tarjan. "Depth-first search and linear graph algorithms."
  *SIAM Journal on Computing*, 1(2):146--160, 1972.
  `DOI:10.1137/0201010 <https://doi.org/10.1137/0201010>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
