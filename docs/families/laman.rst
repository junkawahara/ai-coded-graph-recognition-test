Laman グラフ
========================

グラフが Laman グラフかどうかを判定する。
Laman 条件、すなわち m = 2n - 3 かつ任意の k 頂点上の部分グラフの辺数が
高々 2k - 3 であることを満たすグラフである。Laman グラフは平面上の極小剛性
フレームワークを特徴づける。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1206.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::LamanLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_labeled_graphs
   :project: graph_recognition

上記の列挙はラベル付きグラフを出力する。もう一方の列挙は同型類ごとに
代表元を 1 つ出力する (nauty-laman-plugin 流)。アルゴリズムは
(2,3)-スパースグラフの範囲を成長する McKay の canonical construction
path 法である。Laman グラフ自体は頂点削除で閉じていない (辺数 2n - 3
が崩れる) が、(2,3)-スパース性は遺伝的なので、探索の中間レベルは
(2,3)-スパースグラフの範囲を動き、タイト性 (m = 2n - 3) は出力時に
検査する。候補の子はスパース性の辺数上界、タイト性への到達可能性
(以後の各頂点は既存頂点ごとに高々 1 本しか辺を追加できない)、最後の
2 レベルでの最小次数 2 の実現可能性、および新頂点を含む部分集合に
限定した増分 (2,3)-スパース性検査で枝刈りされる。Laman グラフは常に
連結なので ``connected_only`` フラグはない。

.. doxygenenum:: graph_recognition::LamanUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

非同型列挙については、``n = 9`` までの個数が
`OEIS A227117 <https://oeis.org/A227117>`_ (``n = 1, ..., 9`` に対して
``1, 1, 1, 1, 3, 13, 70, 608, 7222``) と一致することを検証した
(``n = 10`` の出力も数列の 110132 と一致する)。``n = 6`` までは
ラベル付き列挙の出力を正準形化して得られる同型類の集合と一致することも
検証した。静的テストケースは ``n = 9`` までである (``n = 10`` は約
90 秒かかる)。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "laman.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}});
       auto result = check_laman(g);

       std::cout << std::boolalpha << result.is_laman << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "laman_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "laman_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 13
       return 0;
   }


参考文献
------------

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
