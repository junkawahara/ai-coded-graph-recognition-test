スナーク
================

グラフがスナークかどうかを判定する。
スナークとは、橋を持たず、内周 (girth) が 5 以上で、巡回 4-辺連結であり、
彩色指数が 4 である (3-辺彩色可能でない) 3-正則グラフである。
最小のスナークは Petersen グラフ (n = 10) なので、頂点数 10 未満のグラフは
すべて非スナークと判定する。

認識
----------

.. doxygenenum:: graph_recognition::SnarkAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_snark
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::SnarkLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_labeled_graphs
   :project: graph_recognition

上記の列挙はラベル付きグラフを出力する。もう一方の列挙は同型類ごとに
代表元を 1 つ出力する (snarkhunter 流)。アルゴリズムは 3-正則グラフの
非同型列挙 (次数制約付きの McKay の canonical construction path 法) に
内周制約を組み込んだものである。内周 5 以上という条件は頂点削除で
保存されるので、探索の中間レベルは最大次数 3 以下かつ内周 5 以上の
グラフの範囲を動く。新しい頂点の近傍に距離 2 以下の 2 頂点を含めると
長さ 4 以下の閉路ができるため、そのような候補は枝刈りされる。最終
レベルに到達したグラフは構成により内周 5 以上の 3-正則グラフであり、
残りのスナーク条件 (巡回 4-辺連結性と 3-辺彩色不可能性) は構成に
沿って単調ではないため、出力時に ``check_snark`` で検査する。

.. doxygenenum:: graph_recognition::SnarkUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SnarkUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

非同型列挙については、``n = 16`` までの個数が
`OEIS A130315 <https://oeis.org/A130315>`_ (``n = 10, 12, 14, 16`` に対して
``1, 0, 0, 0``) と一致することを検証した。``n = 10`` の唯一の出力は
Petersen グラフである。``n = 6`` まではラベル付き列挙の出力を正準形化
して得られる同型類の集合と一致することも検証した (最小のスナークは
10 頂点なので、この範囲では両者とも空)。静的テストケースは ``n = 14``
までである (``n = 16`` は約 6 分かかる)。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/snark.h"

   int main() {
       using namespace graph_recognition;

       // Petersen graph (the smallest snark)
       Graph g(10, {
           {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
           {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6},
           {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}
       });
       auto result = check_snark(g);

       std::cout << std::boolalpha << result.is_snark << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/snark_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_labeled_graphs(10);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/snark_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_unlabeled_graphs(10);
       std::cout << result.graphs.size() << '\n';  // 1 (Petersen グラフ)
       return 0;
   }


参考文献
------------

* R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable."
  *The American Mathematical Monthly*, 82(3):221--239, 1975.
  `DOI:10.2307/2319844 <https://doi.org/10.2307/2319844>`_

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, J. Hägglund, K. Markström. "Generation and properties of snarks."
  *Journal of Combinatorial Theory, Series B*, 103(4):468--488, 2013.
  `DOI:10.1016/j.jctb.2013.05.001 <https://doi.org/10.1016/j.jctb.2013.05.001>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
