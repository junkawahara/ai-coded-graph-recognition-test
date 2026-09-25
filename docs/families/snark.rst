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

両列挙器の既定は ``SNARKHUNTER``。Brinkmann--Goedgebeur--McKay (2011)
の Figure 2 にある3操作で K4 から prime cubic graph を構成し、異なる2辺を
細分して新しい頂点間に辺を挿入する。正準ラベル上の最初の有効な辺縮約を
親とし、親の正準形の一致と兄弟の正準形集合によって同型重複を除く。
中間グラフは連結な単純3正則グラフであり、三角形や橋も許容する。
短い閉路の枝刈りは、残りの挿入回数で内周5を達成できない場合に限る。

最後の挿入では、3辺彩色から得られる二色閉路上の辺対を除外する。
同じ二色閉路上の辺対への挿入は必ず彩色可能になるためである
(Brinkmann ら 2013, Theorem 3.3)。非 Hamilton 閉路上での1回の Kempe
交換による追加の先読みも行う。残った候補には、正確な辺彩色と巡回辺連結性の
検査を行う。ラベル付き版は各代表元を ``n! / |Aut(G)|`` 個の異なる
ラベル付きグラフに展開する。

旧方式は ``SnarkLabeledEnumAlgorithm::REVERSE_SEARCH`` と
``SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION`` で選択でき、
出力順序・代表元のラベルも維持する。CLI はそれぞれ
``--algo reverse_search`` と ``--algo canonical_augmentation``。
新方式は ``--algo snarkhunter`` で明示指定できる。

新方式の上限は63頂点。従来方式はラベル付き30頂点・ラベルなし63頂点を維持する。
奇数頂点・10頂点未満は0件。Snark は連結なので ``connected_only`` はない。
列挙は C++・CLI のみで、Python は認識のみ対応する。

外部 snarkhunter・nauty に依存せず、共有の幅優先ラベル付けによる正準化
(最悪指数時間)、独自の縮約順序と兄弟集合を使う。三角形操作の一括処理・
拡張軌道による枝刈り・第2の独立彩色・大きな内周向けの追加アルゴリズムは未実装。
prime の正準形集合・探索経路上の兄弟集合・全出力・ラベル展開の訪問済み集合を
保持し、原実装の速度や多項式遅延は主張しない。

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

``n = 10, 12, 14, 16, 18`` の件数 ``1, 0, 0, 0, 2``
(`OEIS A130315 <https://oeis.org/A130315>`_)、認識と非同型性をテストする。
Petersen グラフのラベル付き30,240件を旧方式と辺集合で比較し、
ラベルなし版も14頂点まで正準形集合を比較する。基礎となる3正則生成は
12頂点まで既存の頂点追加方式と比較し、prime の件数は18頂点まで確認する。
10頂点以下の cubic 親で先読みが除外した全候補を独立した辺彩色器で確認し、
先読みを無効にしても出力が変わらないことをテストする。
別途20頂点で6件の出力も確認した。


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

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "Generation of Cubic graphs."
  *Discrete Mathematics & Theoretical Computer Science* 13(2):69–80, 2011.
  `論文 <https://dmtcs.episciences.org/551/pdf>`_.
* G. Brinkmann, J. Goedgebeur, J. Hägglund, K. Markström. "Generation and properties of snarks."
  *Journal of Combinatorial Theory, Series B*, 103(4):468--488, 2013.
  `DOI:10.1016/j.jctb.2013.05.001 <https://doi.org/10.1016/j.jctb.2013.05.001>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
