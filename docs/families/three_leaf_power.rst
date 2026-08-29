3-leaf power グラフ
========================================

グラフが 3-leaf power かどうかを判定する。
Brandstadt & Le (2006) により (bull, dart, gem)-free な弦グラフと同値であり、
critical clique graph が森であることを確認することで認識できる。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_651.html>`_ を参照。

認識
----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_three_leaf_power
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。連結な 3-leaf power は critical clique graph が
木で、隣接する critical clique 間が完全結合になっているグラフちょうど
(Brandstädt & Le 2006)、すなわち木の各頂点をクリークで置換したグラフです。
そのため Bandelt & Mulder (1986) の 1 頂点拡張のうち true twin の追加は常に
クラスを保ち、ペンダント頂点の追加は対象頂点が true twin を持たないか
グラフが完全グラフのときに限りクラスを保ちます (それ以外では critical
clique が分裂して critical clique graph に三角形ができます)。false twin の
追加は不要です: 非自明な critical clique を持つ連結メンバーは true twin
拡張で得られ、すべて singleton なら木なのでペンダント拡張で得られます。
K1 からレベルごとに生成し、各レベルで正準形の集合により同型なものを
除去します。認識器の呼び出しは不要です。非連結なものはクラスが非交和で
閉じていることから、成分サイズの整数分割ごとに連結代表元の多重集合として
合成します。個数は ``connected_only`` 指定時に OEIS A277863
(1, 1, 2, 5, 12, 32, 82, 227, 629, ...)、指定しない場合はその Euler 変換
(1, 2, 4, 10, 24, 65, 171, 478, 1341, ...) です。

.. doxygenenum:: graph_recognition::ThreeLeafPowerUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThreeLeafPowerUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ThreeLeafPowerUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
------------------------------

非同型列挙については、``connected_only`` 指定時に ``n = 10`` まで
`OEIS A277863 <https://oeis.org/A277863>`_ の
``1, 1, 2, 5, 12, 32, 82, 227, 629, 1840`` と一致することを検証した。
全体 (非連結を含む) の個数 ``1, 2, 4, 10, 24, 65, 171, 478, 1341`` は、
``n = 7`` までラベル付き列挙器の出力を正準化した同型類の集合と一致する
ことを確認した。


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "three_leaf_power.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_three_leaf_power(g);

       std::cout << std::boolalpha << result.is_three_leaf_power << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "three_leaf_power_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_three_leaf_power_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "three_leaf_power_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_three_leaf_power_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 24
       return 0;
   }


参考文献
------------

* N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees."
  *Journal of Algorithms*, 42(1):69--108, 2002.
  `DOI:10.1006/jagm.2001.1195 <https://doi.org/10.1006/jagm.2001.1195>`_

* A. Brandstädt, V. B. Le. "Structure and linear-time recognition of 3-leaf powers."
  *Information Processing Letters*, 98(4):133--138, 2006.
  `DOI:10.1016/j.ipl.2006.01.004 <https://doi.org/10.1016/j.ipl.2006.01.004>`_

* M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems."
  *Algorithmica*, 44(4):363--381, 2006.
  `DOI:10.1007/s00453-005-1180-z <https://doi.org/10.1007/s00453-005-1180-z>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_
