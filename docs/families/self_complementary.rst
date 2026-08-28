自己補グラフ
====================

**自己補グラフ** は自身の補グラフと同型なグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1059.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SelfComplementaryAlgorithm``
     - 説明
   * - ``ISOMORPHISM_CHECK`` **(既定)**
     - グラフが自身の補グラフと同型かどうかを判定する。

.. doxygenenum:: graph_recognition::SelfComplementaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_self_complementary
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::SelfComplementaryLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_self_complementary_labeled_graphs
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。構成法は同じ補完置換 (complementing
permutation) によるものですが、すべての置換ではなく、許される巡回型ごとに
代表となる置換 1 つだけを用います。同じ巡回型の置換どうしは共役であり、
補完置換を共役で移すと、それが補完するグラフの集合もその像に移るので、
巡回型ごとに代表を 1 つ取れば、その巡回型が生み出しうる同型類はすべて
得られるからです。固定した置換に対するグラフは、頂点対の軌道から読み取り
ます。軌道に沿って辺と非辺が交互に現れ、かつ軌道の長さは必ず偶数なので、
``r`` 個の軌道それぞれにちょうど 2 通りの割り当てがあります。さらに
すべての軌道を反転させても補グラフになるだけなので、候補は ``2^(r-1)``
個で十分です。同型除去は正準形の集合で大域的に行います。1 つのグラフが
複数の巡回型の補完置換を持ちうるからです。

.. doxygenenum:: graph_recognition::SelfComplementaryUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SelfComplementaryUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_self_complementary_unlabeled_graphs
   :project: graph_recognition


OEIS カウント検証
--------------------------------

非同型列挙については ``n = 9`` まで
`OEIS A000171 <https://oeis.org/A000171>`_ の
``1, 0, 0, 1, 2, 0, 0, 10, 36`` と一致することを検証した。列挙器自身とは
独立にも、ラベル付き列挙器の出力全体を正準化して検証した (``n = 8`` の
98280 個、``n = 9`` の 4123728 個が、ここで出力される 10 個・36 個の
同型類にちょうど一致する)。静的テストケースは ``n = 9`` までである。
コストは巡回型ごとに ``2^(r-1)`` 回 (``r`` は頂点対の軌道数) の正準化で、
対称性の高いグラフに対する厳密な正準化が実用範囲を決めている。``n <= 9``
は 1 秒未満で終わるが、``n = 12`` (巡回型 (4, 4, 4) で ``r = 18``) は
数時間かかる。


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "self_complementary.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_self_complementary(g);

       std::cout << std::boolalpha << result.is_self_complementary << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "self_complementary_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_self_complementary_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "self_complementary_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_self_complementary_unlabeled_graphs(9);
       std::cout << result.graphs.size() << '\n';  // 36 = A000171(9)
       return 0;
   }


参考文献
------------

* H. Sachs. "Über selbstkomplementäre Graphen."
  *Publicationes Mathematicae Debrecen*, 9:270--288, 1962.

* G. Ringel. "Selbstkomplementäre Graphen."
  *Archiv der Mathematik*, 14(1):354--358, 1963.
  `DOI:10.1007/BF01234967 <https://doi.org/10.1007/BF01234967>`_

* R. C. Read. "On the number of self-complementary graphs and digraphs."
  *Journal of the London Mathematical Society*, s1-38(1):99--104, 1963.
  `DOI:10.1112/jlms/s1-38.1.99 <https://doi.org/10.1112/jlms/s1-38.1.99>`_
