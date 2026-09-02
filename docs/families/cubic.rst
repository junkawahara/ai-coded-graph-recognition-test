3-正則グラフ (cubic)
========================================

グラフが 3-正則 (cubic) であるかどうかを判定する。
3-正則グラフとはすべての頂点の次数がちょうど 3 であるグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1100.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::CubicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CubicLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_labeled_graphs
   :project: graph_recognition

上記の列挙はラベル付きグラフを出力する。もう一方の列挙は同型類ごとに
代表元を 1 つ出力する。アルゴリズムは McKay の canonical construction
path 法に次数制約を組み合わせたものである (snarkhunter の canonical
deletion の頂点版)。3-正則グラフは遺伝的ではないが、3-正則グラフの誘導
部分グラフの最大次数は 3 以下なので、頂点を 1 つずつ追加する探索の中間
レベルは最大次数 3 以下のグラフの範囲を動く。新しい頂点の近傍は次数 3
未満の頂点からなる高々 3 元の部分集合のみを走り、さらに完成可能性の
必要条件 (残り頂点数を ``r`` とすると、各頂点の次数不足は ``r`` 以下、
その総和は ``3r`` 以下、``3r`` から総和を引いた値は偶数) で枝刈りする。
この条件により最終レベルに到達したグラフは必ず 3-正則になる。子グラフ
は、追加した頂点がその子の正準ラベリングで最後に置かれる頂点の自己同型
軌道に属するときに限り採用される。

.. doxygenenum:: graph_recognition::CubicUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CubicUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 4, 6, 8`` に対して、列挙されたラベル付き 3-正則グラフの個数が
`OEIS A002829 <https://oeis.org/A002829>`_ (``a(k)`` は頂点数 ``2k`` で添字付けされて
いるため、ここでの頂点数 ``n`` は OEIS の添字 ``n/2`` に対応する) の
``1, 70, 19355`` と一致することを検証した。

非同型列挙については、``n = 14`` までの個数が
`OEIS A005638 <https://oeis.org/A005638>`_ (同じ添字付け;
``n = 4, 6, ..., 14`` に対して ``1, 2, 6, 21, 94, 540``) と一致すること、
および ``n = 6`` まではラベル付き列挙の出力を正準形化して得られる同型類の
集合と一致することを検証した。``connected_only`` を指定した場合の個数は
`OEIS A002851 <https://oeis.org/A002851>`_ (``1, 2, 5, 19, 85, 509``) と
一致する。静的テストケースは ``n = 12`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/cubic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic(g);

       std::cout << std::boolalpha << result.is_cubic << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cubic_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cubic_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 6
       return 0;
   }


参考文献
----------------

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "Generation of cubic graphs."
  *Discrete Mathematics and Theoretical Computer Science*, 13(2):69--80, 2011.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
