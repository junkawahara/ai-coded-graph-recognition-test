k-正則グラフ
========================

グラフが k-正則であるかどうかを判定する。
k-正則グラフとはすべての頂点が同じ次数 k を持つグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1149.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::KRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_kregular
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::KRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_kregular_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙はラベル付きグラフを出力する。もう一方の列挙は同型類ごとに
代表元を 1 つ出力する。アルゴリズムは McKay の canonical construction
path 法に次数制約を組み合わせたものである (GENREG の orderly generation
を共通の canonical augmentation 機構の上で実現したもので、三正則の
ラベルなし列挙を次数 k にパラメータ化した一般化)。k-正則グラフは遺伝的
ではないが、k-正則グラフの誘導部分グラフの最大次数は k 以下なので、
頂点を 1 つずつ追加する探索の中間レベルは最大次数 k 以下のグラフの範囲を
動く。新しい頂点の近傍は次数 k 未満の頂点からなる高々 k 元の部分集合の
みを走り、さらに完成可能性の必要条件 (残り頂点数を ``r`` とすると、
各頂点の次数不足は ``r`` 以下、その総和は ``kr`` 以下、``kr`` から総和を
引いた値は偶数) で枝刈りする。この条件により最終レベルに到達したグラフは
必ず k-正則になる。子グラフは、追加した頂点がその子の正準ラベリングで
最後に置かれる頂点の自己同型軌道に属するときに限り採用される。

.. doxygenenum:: graph_recognition::KRegularUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::KRegularUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_kregular_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

非同型列挙については、個数が三角数列
`OEIS A051031 <https://oeis.org/A051031>`_ (第 n 行第 k 列) と一致する
こと (各行内の列 ``k`` と列 ``n-1-k`` の補グラフ対称性を含む) を検証した。
``k = 2`` では 3 以上の部分への n の分割数 (サイクルの非交和)、``k = 3``
では `OEIS A005638 <https://oeis.org/A005638>`_ (``n = 10`` まで三正則
専用列挙器とも一致)、``k = 4`` では
`OEIS A033301 <https://oeis.org/A033301>`_ (``n = 5, ..., 12`` に対して
``1, 1, 2, 6, 16, 60, 266, 1547``) と一致する。``connected_only`` を
指定した場合の ``k = 4`` の個数は
`OEIS A006820 <https://oeis.org/A006820>`_ (``n = 5, ..., 11`` に対して
``1, 1, 2, 6, 16, 59, 265``) と一致する。``n = 6`` までは、すべての
``0 <= k < n`` についてラベル付き列挙の出力を正準形化して得られる同型類の
集合と一致することを検証した。静的テストケースは ``n = 10`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/kregular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_kregular(g);

       std::cout << std::boolalpha << result.is_kregular << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/kregular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_kregular_labeled_graphs_reverse_search(4, 2);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/kregular_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_kregular_unlabeled_graphs(9, 4);
       std::cout << result.graphs.size() << '\n';  // 16
       return 0;
   }


参考文献
----------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_

* M. Meringer. "Fast generation of regular graphs and construction of cages."
  *Journal of Graph Theory*, 30(2):137--146, 1999.
  `DOI:10.1002/(SICI)1097-0118(199902)30:2\<137::AID-JGT7\>3.0.CO;2-G <https://doi.org/10.1002/(SICI)1097-0118(199902)30:2%3C137::AID-JGT7%3E3.0.CO;2-G>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
