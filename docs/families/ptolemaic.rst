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

ラベル付き列挙の既定方式は ``VERTEX_INCREMENTAL`` です。下記の構造的な
ラベルなし列挙器と非連結成分の合成を再利用し、各代表元を全ラベルに展開します。
pendant・true twin 追加は常に弦性を保ち、false twin は simplicial 頂点にのみ
追加するため、認識器の呼び出しは不要です。距離遺伝グラフと共通の
``util/labeled_expansion.h`` で隣接ラベル交換を探索し、twin や同型な成分の
交換による重複を辺集合で排除して、各代表元から ``n! / |Aut(G)|`` 個を出力します。
最大ラベルが中央頂点である P5 などを漏らさないため、このラベル展開が必要です。

``PtolemaicLabeledEnumAlgorithm::REVERSE_SEARCH`` は従来の弦グラフ逆探索・
Ptolemaic 認識方式とその出力順を維持します。C++ の関数名も維持しています。
両方式は同じラベル付きグラフを異なる順序で返し、5頂点では762個、6頂点では
13,534個です。C++・CLI は ``n = 0`` で空グラフを1個返します。新方式は
ビットマスクの制約により ``n <= 63``、従来方式は ``max_vertex_count`` までです。

CLI は ``--algo vertex_incremental`` (既定) または ``--algo reverse_search``、
Python は ``enumerate_ptolemaic_labeled_graphs(n, algorithm="vertex_incremental")``
または ``algorithm="reverse_search"`` で選択します。
Python の入力範囲は従来どおり ``1 <= n <= 6`` です。

正準形集合、代表元、ラベル展開の訪問済み集合、全出力をメモリに保持します。
Yamazaki・Qian・Uehara (2024) の専用の正準親方式や遅延保証は実装していません。

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
   #include "util/graph.h"
   #include "recognizers/ptolemaic.h"

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
   #include "enumerators/ptolemaic_labeled_enum.h"

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
   #include "enumerators/ptolemaic_unlabeled_enum.h"

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

* K. Yamazaki, M. Qian, R. Uehara. "Efficient enumeration of non-isomorphic
  distance-hereditary graphs and related graphs."
  *Discrete Applied Mathematics*, 342:190--199, 2024.
  `DOI:10.1016/j.dam.2023.09.002 <https://doi.org/10.1016/j.dam.2023.09.002>`_
