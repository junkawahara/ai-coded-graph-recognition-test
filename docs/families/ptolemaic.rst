Ptolemaic グラフ
================================

グラフが **ptolemaic** であるとは、弦グラフかつ距離遺伝グラフ
(distance-hereditary graph) であることをいう。これは gem-free かつ弦グラフで
あることと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_95.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - 説明
   * - ``DH_HASHMAP``
     - ハッシュマップを用いた距離遺伝性の検査
   * - ``DH_SORTED`` **(既定)**
     - ソート済み隣接リストを用いた距離遺伝性の検査

.. doxygenenum:: graph_recognition::PtolemaicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_ptolemaic
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::PtolemaicLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ptolemaic_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは距離遺伝グラフを生成する
Bandelt & Mulder (1986) の 1 頂点拡張 (ペンダント頂点の追加 / true twin の
追加 / false twin の追加) のうち、弦性も保つものだけに制限したものです。
制限が必要なのは false twin だけで、``v`` の false twin ``u`` は ``N(v)``
の非隣接な 2 頂点 ``a, b`` に対して誘導 4 閉路 ``u-a-v-b-u`` を作るため、
``v`` が単体的 (simplicial) のときに限り許されます。すると連結な
ptolemaic グラフは、1 頂点少ない連結な ptolemaic グラフに制限された拡張を
施したものちょうどなので、K1 からレベルごとに生成し、各レベルで正準形の
集合により同型なものを除去します。認識器の呼び出しは不要です。
非連結なものはクラスが非交和で閉じていることから、成分サイズの整数分割ごとに
連結代表元の多重集合として合成します。個数は ``connected_only`` 指定時に
OEIS A287888(n) (1, 1, 2, 5, 14, 47, 170, 676, 2834, ...)、指定しない場合は
その Euler 変換 (1, 2, 4, 10, 26, 82, 278, 1053, 4251, ...) です。

.. doxygenenum:: graph_recognition::PtolemaicUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PtolemaicUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ptolemaic_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
------------------------------

非同型列挙については、``connected_only`` 指定時に ``n = 9`` まで
`OEIS A287888 <https://oeis.org/A287888>`_ の
``1, 1, 2, 5, 14, 47, 170, 676, 2834`` と一致することを検証した。
全体 (非連結を含む) の個数 ``1, 2, 4, 10, 26, 82, 278, 1053, 4251`` は、
``n = 7`` までラベル付き列挙器の出力を正準化した同型類の集合と一致する
ことを確認した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "ptolemaic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_ptolemaic(g);

       std::cout << std::boolalpha << result.is_ptolemaic << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ptolemaic_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ptolemaic_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ptolemaic_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ptolemaic_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 26
       return 0;
   }


参考文献
----------------

* E. Howorka. "A characterization of Ptolemaic graphs."
  *Journal of Graph Theory*, 5(3):323--331, 1981.
  `DOI:10.1002/jgt.3190050314 <https://doi.org/10.1002/jgt.3190050314>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_
